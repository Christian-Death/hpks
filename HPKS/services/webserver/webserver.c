/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 * 
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   webserver.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 27. November 2012, 19:04
 */

#include <stdlib.h>
#include <string.h>/* strrchr */
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <microhttpd.h>

#include "defines/used_includes.h"
#include "../json_worker.h"
#include "webserver.h"

/*#### DEFINES ###################*/
#define PORT 8888
#define MAXCLIENTS      2
#define POSTBUFFERSIZE  2048
#define MAX_STRING_POST_SIZE 20000

//#define ROOT_PATH "/home/pi/test/pufferkontrollsystem/www/"

#define PAGE_FILE_NOT_FOUND "<html><head><title>File not found</title></head><body>File not found</body></html>"
#define PAGE_JSONRPC_INFO   "<html><head><title>JSONRPC</title></head><body>JSONRPC active and working</body></html>"
#define NOT_SUPPORTED       "<html><head><title>Not Supported</title></head><body>The method you are trying to use is not supported by this server</body></html>"
#define SERVICE_UNAVAILABLE "<html><body>This server is busy, please try again later.</body></html>"
#define BAD_REQUEST  "<html><body>This doesn't seem to be right.</body></html>"

#define ASK_PAGE "<html><body>\n\
                       Upload a file, please!<br>\n\
                       There are %u clients uploading at the moment.<br>\n\
                       <form action=\"/filepost\" method=\"post\" enctype=\"multipart/form-data\">\n\
                       <input name=\"file\" type=\"file\">\n\
                       <input type=\"submit\" value=\" Send \"></form>\n\
                       </body></html>"
#define COMPLETE_PAGE "<html><body>The upload has been completed.</body></html>"

#define ERROR_PAGE "<html><body>An internal server error has occured.</body></html>"
//const char *fileexistspage =
#define fileexistspage "<html><body>This file already exists.</body></html>"

#define DEFAULT_PAGE        "index.html"
#define MAXANSWERSIZE 1024

/*#### TYPEDEFS ###################*/
typedef enum {
  UNKNOWN,
  POST,
  GET,
  HEAD
} HTTPMethod;


/*#### VARIABLE ###################*/
static unsigned int nr_of_clients = 0;
char *string_to_base64(const char *message);
const char *CreateMimeTypeFromExtension(const char *ext);

char *www_root_path;

static uint8_t m_running;
static struct MHD_Daemon *m_daemon;

static http_request_handler http_handler;
static http_json_handler json_handler;

//--------------------------------------------------------------------------------------------------
// Name:      register_http_handler
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

void register_http_handler(http_request_handler handler)
{
  http_handler = handler;
}

//--------------------------------------------------------------------------------------------------
// Name:      register_json_handler
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

void register_json_handler(http_json_handler handler)
{
  json_handler = handler;
}

//--------------------------------------------------------------------------------------------------
// Name:      GetMethod
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

HTTPMethod GetMethod(const char *method)
{
  if (strcmp(method, "GET") == 0)
    return GET;
  if (strcmp(method, "POST") == 0)
    return POST;
  if (strcmp(method, "HEAD") == 0)
    return HEAD;

  return UNKNOWN;
}

//--------------------------------------------------------------------------------------------------
// Name:      CreateErrorResponse
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int CreateErrorResponse(struct MHD_Connection *connection, int responseType, HTTPMethod method)
{
  int ret = MHD_NO;
  size_t payloadSize = 0;
  void *payload = NULL;

  if (method != HEAD)
  {
    switch (responseType)
    {
      case MHD_HTTP_NOT_FOUND:
        payloadSize = strlen(PAGE_FILE_NOT_FOUND);
        payload = (void *)PAGE_FILE_NOT_FOUND;
        break;
      case MHD_HTTP_NOT_IMPLEMENTED:
        payloadSize = strlen(NOT_SUPPORTED);
        payload = (void *)NOT_SUPPORTED;
        break;
      case MHD_HTTP_SERVICE_UNAVAILABLE:
        payloadSize = strlen(SERVICE_UNAVAILABLE);
        payload = (void *)SERVICE_UNAVAILABLE;
        break;
      case MHD_HTTP_BAD_REQUEST:
        payloadSize = strlen(BAD_REQUEST);
        payload = (void *)BAD_REQUEST;
        break;
    }
  }

  struct MHD_Response *response = MHD_create_response_from_data(payloadSize, payload, MHD_NO, MHD_NO);
  ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
  MHD_destroy_response(response);
  return ret;
}

//--------------------------------------------------------------------------------------------------
// Name:      CreateMemoryDownloadResponse
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int CreateMemoryDownloadResponse(struct MHD_Connection *connection, void *data, size_t size, const char* content_type)
{
  struct MHD_Response *response = MHD_create_response_from_data(size, data,
                                                                MHD_NO, MHD_NO);
  if (!response)
    return MHD_NO;

  MHD_add_response_header(response, "Content-Type", content_type);

  int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);
  return ret;
}

//--------------------------------------------------------------------------------------------------
// Name:      iterate_post
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int iterate_post(void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
                        const char *filename, const char *content_type,
                        const char *transfer_encoding, const char *data, uint64_t off,
                        size_t size)
{
  struct connection_info_struct *con_info = coninfo_cls;

  dbg_printf("key %s\r\n", key);
  dbg_printf("filename %s\r\n", filename);
  dbg_printf("content_type %s\r\n", content_type);
  dbg_printf("transfer_encoding %s\r\n", transfer_encoding);
  dbg_printf("data %s\r\n", data);
  dbg_printf("off %lu\r\n", off);
  dbg_printf("size %lu\r\n", size);

  con_info->answerstring = ERROR_PAGE;
  con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;

  if (!con_info->action)
  {
    if (0 == strcmp(key, "action"))
    {
      con_info->action = strdup(data);
      if (!con_info->action)
        return MHD_NO;
      return MHD_YES;
    }
  }
  else
  {
    if (0 == strcmp(key, "json"))
    {
      con_info->data = strdup(data);
      if (!con_info->data)
        return MHD_NO;
      con_info->answerstring = COMPLETE_PAGE;
      con_info->answercode = MHD_HTTP_OK;
      int rest;
      rest = (*json_handler)(coninfo_cls, size);

      return MHD_YES;
    }
  }

  return MHD_NO;
}

//--------------------------------------------------------------------------------------------------
// Name:      ContentReaderCallback
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static
#if (MHD_VERSION >= 0x00040001)
  ssize_t ContentReaderCallback(void *cls, uint64_t pos, char *buf, size_t max)
#else   //libmicrohttpd < 0.4.0
  ssize_t ContentReaderCallback(void *cls, size_t pos, char *buf, size_t max)
#endif
{
  FILE *file = cls;

  (void)fseek(file, pos, SEEK_SET);
  return fread(buf, 1, max, file);

}

//--------------------------------------------------------------------------------------------------
// Name:      ContentReaderFreeCallback
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static void ContentReaderFreeCallback(void *cls)
{
  FILE *file = cls;
  fclose(file);
}

//--------------------------------------------------------------------------------------------------
// Name:      CreateFileDownloadResponse
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int CreateFileDownloadResponse(struct MHD_Connection *connection, const char *strURL)
{
  int ret = MHD_NO;
  FILE *file;
  struct stat buf;

  char fullpath[PATH_MAX];

  snprintf(fullpath, PATH_MAX, "%s%s",  www_root_path, strURL);
  if ((0 == stat(fullpath, &buf)) &&(S_ISREG(buf.st_mode)))
    file = fopen(fullpath, "rb");
  else
    file = NULL;

  if (file == NULL)
  {
    /* error accessing file */
    return CreateErrorResponse(connection, MHD_HTTP_NOT_FOUND, GET); /* GET Assumed Temporarily */
  }
  else
  {
    struct MHD_Response *response;

    response = MHD_create_response_from_callback(buf.st_size,
                                                 2048,
                                                 &ContentReaderCallback, file,
                                                 &ContentReaderFreeCallback);

    char * ext = strrchr(strURL, '.');
    if (!ext)
    {
      /* no extension */
    }
    else
    {
      
    }

    const char *mime = CreateMimeTypeFromExtension(ext);
    if (mime)
      MHD_add_response_header(response, "Content-Type", mime);

    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);

    MHD_destroy_response(response);
  }

  return ret;
}

//--------------------------------------------------------------------------------------------------
// Name:      request_completed
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static void request_completed(void *cls, struct MHD_Connection *connection,
                  void **con_cls, enum MHD_RequestTerminationCode toe)
{
  struct connection_info_struct *con_info = *con_cls;

  if (NULL == con_info)
    return;

  if (con_info->page)
    free(con_info->page);

  if (con_info->connectiontype == POST)
  {
    if (NULL != con_info->postprocessor)
    {
      MHD_destroy_post_processor(con_info->postprocessor);
      if (con_info->action)
        free(con_info->action);
      if (con_info->data)
        free(con_info->data);
      nr_of_clients--;
    }
    JsonParser *parser = con_info->parser;
    if (NULL != parser)
    {
      json_tokener_free(parser->tok);
      json_object_put(parser->root);
      free(parser);
    }
  }

  free(con_info);
  *con_cls = NULL;
}

//--------------------------------------------------------------------------------------------------
// Name:      ask_for_authentication
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int ask_for_authentication(struct MHD_Connection *connection, const char *realm)
{
  int ret;
  struct MHD_Response *response;
  char *headervalue;
  const char *strbase = "Basic realm=";

  response = MHD_create_response_from_data(0, NULL, MHD_NO, MHD_NO);
  if (!response)
    return MHD_NO;

  headervalue = malloc(strlen(strbase) + strlen(realm) + 1);
  if (!headervalue)
    return MHD_NO;

  strcpy(headervalue, strbase);
  strcat(headervalue, realm);


  ret = MHD_add_response_header(response, MHD_HTTP_HEADER_WWW_AUTHENTICATE, headervalue);
  ret |= MHD_add_response_header(response, MHD_HTTP_HEADER_CONNECTION, "close");
  free(headervalue);
  if (!ret)
  {
    MHD_destroy_response(response);
    return MHD_NO;
  }

  ret = MHD_queue_response(connection, MHD_HTTP_UNAUTHORIZED, response);

  MHD_destroy_response(response);

  return ret;
}


#define REALM     "\"Maintenance\""
#define USER      "test"
#define PASSWORD  "test"
//--------------------------------------------------------------------------------------------------
// Name:      is_authenticated
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int is_authenticated(struct MHD_Connection *connection,
                 const char *username, const char *password)
{
  const char *headervalue;
  char *expected_b64, *expected;
  const char *strbase = "Basic ";
  int authenticated;

  headervalue =
    MHD_lookup_connection_value(connection, MHD_HEADER_KIND,
                                "Authorization");
  if (NULL == headervalue)
    return 0;
  if (0 != strncmp(headervalue, strbase, strlen(strbase)))
    return 0;

  expected = malloc(strlen(username) + 1 + strlen(password) + 1);
  if (NULL == expected)
    return 0;

  strcpy(expected, username);
  strcat(expected, ":");
  strcat(expected, password);

  expected_b64 = string_to_base64(expected);
  if (NULL == expected_b64)
    return 0;

  strcpy(expected, strbase);
  authenticated =
    (strcmp(headervalue + strlen(strbase), expected_b64) == 0);

  free(expected_b64);

  return authenticated;
}

//--------------------------------------------------------------------------------------------------
// Name:      AnswerToConnection
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

#if (MHD_VERSION >= 0x00040001)
  int AnswerToConnection(void *cls, struct MHD_Connection *connection,
                       const char *url, const char *method,
                       const char *version, const char *upload_data,
                       size_t *upload_data_size, void **con_cls)
#else

  int AnswerToConnection(void *cls, struct MHD_Connection *connection,
                       const char *url, const char *method,
                       const char *version, const char *upload_data,
                       unsigned int *upload_data_size, void **con_cls)
#endif
{
  //char *strURL = strdup(url);
  //char *originalURL = strdup(url);
  HTTPMethod methodType = GetMethod(method);

#if 0  
  if (!is_authenticated(connection, USER, PASSWORD))
    return ask_for_authentication(connection, REALM);
#endif
  if (methodType != GET && methodType != POST) /* Only GET and POST supported, catch other method types here to avoid continual checking later on */
    return CreateErrorResponse(connection, MHD_HTTP_NOT_IMPLEMENTED, methodType);

  if (NULL == *con_cls)
  {
    struct connection_info_struct *con_info;

    if (nr_of_clients >= MAXCLIENTS)
      return CreateErrorResponse(connection, MHD_HTTP_SERVICE_UNAVAILABLE, methodType);


    con_info = malloc(sizeof (struct connection_info_struct));
    if (NULL == con_info)
      return MHD_NO;

    con_info->action = NULL;
    con_info->data = NULL;
    con_info->page = NULL;

    if (methodType == POST)
    {
      con_info->postprocessor =
        MHD_create_post_processor(connection, POSTBUFFERSIZE,
                                  iterate_post, (void *)con_info);


      if (NULL == con_info->postprocessor)
      {
        free(con_info);
        return MHD_NO;
      }
      con_info->parser = json_parser_new();

      nr_of_clients++;

      con_info->connectiontype = POST;
      con_info->answercode = MHD_HTTP_OK;
      con_info->answerstring = COMPLETE_PAGE;
    }
    else
      con_info->connectiontype = GET;

    *con_cls = (void *)con_info;

    return MHD_YES;
  }


  struct connection_info_struct *con_info = *con_cls;
  if (methodType == POST)
  {
    if (0 != *upload_data_size)
    {
      MHD_post_process(con_info->postprocessor, upload_data,
                       *upload_data_size);
      *upload_data_size = 0;

      return MHD_YES;
      /*
          } else {
            if (!strcmp(strURL, "/jsonds18b20")) {
              return JSONDS18B20_Post(con_cls, connection);
            }
       */

      //return CreateMemoryDownloadResponse(connection, (void *) con_info->answerstring,
      //       strlen(con_info->answerstring));
      //send_page (connection, con_info->answerstring, con_info->answercode);
    }
  }
  char * page = con_info->page;
  int rest;
  int ret;
  rest = (*http_handler)(url, method, &page, con_cls);
  switch (rest)
  {
    case HVS_SEND_PAGE:
      ret = CreateFileDownloadResponse(connection, page);
      break;
    case HVS_SEND_MEMORY:
      ret = CreateMemoryDownloadResponse(connection, (void *)page,
                                         strlen(page), "application/json");
      break;
    default:
      ret = CreateErrorResponse(connection, MHD_HTTP_BAD_REQUEST, methodType);
      break;

  }

  /*
    if (page)
      free(page);
   */


  return ret;




  //#endif
  //#endif
  return MHD_NO;
}

//--------------------------------------------------------------------------------------------------
// Name:      webserver_start
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------
uint8_t webserver_start(int port)
{
  const char*a = MHD_get_version();
  if (!m_running)
  {
    char rnd[8];
    size_t off = 0;
    size_t len;
    int fd;
    fd = open("/dev/urandom", O_RDONLY);
    if (-1 == fd)
    {
      fprintf(stderr, "Failed to open `%s': %s\n",
              "/dev/urandom",
              strerror(errno));
      return 1;
    }
    while (off < 8)
    {
      len = read(fd, rnd, 8);
      if (len == -1)
      {
        fprintf(stderr, "Failed to read `%s': %s\n",
                "/dev/urandom",
                strerror(errno));
        (void)close(fd);
        return 1;
      }
      off += len;
    }
    (void)close(fd);

    www_root_path = app_config_get_www_path();
    dbg_printf("WWW Root Path: %s \n", www_root_path);
      
    unsigned int timeout = 60 * 60 * 24;
    
    struct sockaddr_in daemon_ip_addr;
#if HAVE_INET6
  struct sockaddr_in6 daemon_ip_addr6;
#endif

  memset (&daemon_ip_addr, 0, sizeof (struct sockaddr_in));
  daemon_ip_addr.sin_family = AF_INET;
  daemon_ip_addr.sin_port = htons (port);

#if HAVE_INET6
  memset (&daemon_ip_addr6, 0, sizeof (struct sockaddr_in6));
  daemon_ip_addr6.sin6_family = AF_INET6;
  daemon_ip_addr6.sin6_port = htons (port);
#endif

  inet_pton (AF_INET, "127.0.0.1", &daemon_ip_addr.sin_addr);
#if HAVE_INET6
  inet_pton (AF_INET6, "::ffff:127.0.0.1", &daemon_ip_addr6.sin6_addr);
#endif
  
#if HAVE_INET6
    
    // MHD_USE_THREAD_PER_CONNECTION = one thread per connection
    // MHD_USE_SELECT_INTERNALLY = use main thread for each connection, can only handle one request at a time [unless you set the thread pool size]
    m_daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_IPv6 | MHD_USE_DEBUG,
                                port,
                                NULL, NULL,
                                &AnswerToConnection, NULL,
                                //MHD_OPTION_DIGEST_AUTH_RANDOM, sizeof (rnd), rnd,
                                //MHD_OPTION_NONCE_NC_SIZE, 300,
                                MHD_OPTION_SOCK_ADDR, &daemon_ip_addr6,
                                MHD_OPTION_THREAD_POOL_SIZE, 8,
                                MHD_OPTION_CONNECTION_LIMIT, 512,
                                MHD_OPTION_CONNECTION_TIMEOUT, timeout,
                                MHD_OPTION_NOTIFY_COMPLETED, request_completed, NULL,
                                MHD_OPTION_END);
    if (!m_daemon) //try IPv4
#endif
      m_daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG,
                                  port,
                                  NULL, NULL,
                                  &AnswerToConnection, NULL,
                                  //MHD_OPTION_DIGEST_AUTH_RANDOM, sizeof (rnd), rnd,
                                  //MHD_OPTION_NONCE_NC_SIZE, 600,
                                  MHD_OPTION_SOCK_ADDR, &daemon_ip_addr,
                                  MHD_OPTION_THREAD_POOL_SIZE, 8,
                                  MHD_OPTION_CONNECTION_LIMIT, 512,
                                  MHD_OPTION_CONNECTION_TIMEOUT, timeout,
                                  MHD_OPTION_NOTIFY_COMPLETED, request_completed, NULL,
                                  MHD_OPTION_END);
    m_running = m_daemon != NULL;
    
    if(m_running)
      dbg_printf("Webserver gestartet \n");
    else
      dbg_printf("Webserver start fehlgeschlagen\n");
    /*
        if (m_running)
          CLog::Log(LOGNOTICE, "WebServer: Started the webserver");
        else
          CLog::Log(LOGERROR, "WebServer: Failed to start the webserver");
     */
  }
  return m_running;
}

//--------------------------------------------------------------------------------------------------
// Name:      webserver_stop
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

uint8_t webserver_stop()
{
  if (m_running)
  {
    MHD_stop_daemon(m_daemon);
    m_running = 0;
  }

  return !m_running;
}

//--------------------------------------------------------------------------------------------------
// Name:      webserver_isStarted
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

uint8_t webserver_isStarted()
{
  return m_running;
}

//--------------------------------------------------------------------------------------------------
// Name:      CreateMimeTypeFromExtension
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

const char *CreateMimeTypeFromExtension(const char *ext)
{
  if (strcmp(ext, ".aif") == 0) return "audio/aiff";
  if (strcmp(ext, ".aiff") == 0) return "audio/aiff";
  if (strcmp(ext, ".asf") == 0) return "video/x-ms-asf";
  if (strcmp(ext, ".asx") == 0) return "video/x-ms-asf";
  if (strcmp(ext, ".avi") == 0) return "video/avi";
  if (strcmp(ext, ".avs") == 0) return "video/avs-video";
  if (strcmp(ext, ".bin") == 0) return "application/octet-stream";
  if (strcmp(ext, ".bmp") == 0) return "image/bmp";
  if (strcmp(ext, ".dv") == 0) return "video/x-dv";
  if (strcmp(ext, ".fli") == 0) return "video/fli";
  if (strcmp(ext, ".gif") == 0) return "image/gif";
  if (strcmp(ext, ".htm") == 0) return "text/html";
  if (strcmp(ext, ".html") == 0) return "text/html";
  if (strcmp(ext, ".htmls") == 0) return "text/html";
  if (strcmp(ext, ".ico") == 0) return "image/x-icon";
  if (strcmp(ext, ".it") == 0) return "audio/it";
  if (strcmp(ext, ".jpeg") == 0) return "image/jpeg";
  if (strcmp(ext, ".jpg") == 0) return "image/jpeg";
  if (strcmp(ext, ".json") == 0) return "application/json";
  if (strcmp(ext, ".kar") == 0) return "audio/midi";
  if (strcmp(ext, ".list") == 0) return "text/plain";
  if (strcmp(ext, ".log") == 0) return "text/plain";
  if (strcmp(ext, ".lst") == 0) return "text/plain";
  if (strcmp(ext, ".m2v") == 0) return "video/mpeg";
  if (strcmp(ext, ".m3u") == 0) return "audio/x-mpequrl";
  if (strcmp(ext, ".mid") == 0) return "audio/midi";
  if (strcmp(ext, ".midi") == 0) return "audio/midi";
  if (strcmp(ext, ".mod") == 0) return "audio/mod";
  if (strcmp(ext, ".mov") == 0) return "video/quicktime";
  if (strcmp(ext, ".mp2") == 0) return "audio/mpeg";
  if (strcmp(ext, ".mp3") == 0) return "audio/mpeg3";
  if (strcmp(ext, ".mpa") == 0) return "audio/mpeg";
  if (strcmp(ext, ".mpeg") == 0) return "video/mpeg";
  if (strcmp(ext, ".mpg") == 0) return "video/mpeg";
  if (strcmp(ext, ".mpga") == 0) return "audio/mpeg";
  if (strcmp(ext, ".pcx") == 0) return "image/x-pcx";
  if (strcmp(ext, ".png") == 0) return "image/png";
  if (strcmp(ext, ".rm") == 0) return "audio/x-pn-realaudio";
  if (strcmp(ext, ".s3m") == 0) return "audio/s3m";
  if (strcmp(ext, ".sid") == 0) return "audio/x-psid";
  if (strcmp(ext, ".tif") == 0) return "image/tiff";
  if (strcmp(ext, ".tiff") == 0) return "image/tiff";
  if (strcmp(ext, ".txt") == 0) return "text/plain";
  if (strcmp(ext, ".uni") == 0) return "text/uri-list";
  if (strcmp(ext, ".viv") == 0) return "video/vivo";
  if (strcmp(ext, ".wav") == 0) return "audio/wav";
  if (strcmp(ext, ".xm") == 0) return "audio/xm";
  if (strcmp(ext, ".xml") == 0) return "text/xml";
  if (strcmp(ext, ".zip") == 0) return "application/zip";
  if (strcmp(ext, ".tbn") == 0) return "image/jpeg";
  if (strcmp(ext, ".js") == 0) return "application/javascript";
  if (strcmp(ext, ".css") == 0) return "text/css";


  return NULL;

}

//--------------------------------------------------------------------------------------------------
// Name:      string_to_base64
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

char *string_to_base64(const char *message)
{
  const char *lookup =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  unsigned long l;
  int i;
  char *tmp;
  size_t length = strlen(message);

  tmp = malloc(length * 2);
  if (NULL == tmp)
    return tmp;

  tmp[0] = 0;

  for (i = 0; i < length; i += 3)
  {
    l = (((unsigned long)message[i]) << 16)
      | (((i + 1) < length) ? (((unsigned long)message[i + 1]) << 8) : 0)
      | (((i + 2) < length) ? ((unsigned long)message[i + 2]) : 0);


    strncat(tmp, &lookup[(l >> 18) & 0x3F], 1);
    strncat(tmp, &lookup[(l >> 12) & 0x3F], 1);

    if (i + 1 < length)
      strncat(tmp, &lookup[(l >> 6) & 0x3F], 1);
    if (i + 2 < length)
      strncat(tmp, &lookup[l & 0x3F], 1);
  }

  if (length % 3)
    strncat(tmp, "===", 3 - length % 3);

  return tmp;
}