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
 * File:   json_worker.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 27. November 2012, 19:04
 */

#include <json-c/json.h>
#include <uriparser/Uri.h>
#include <math.h>

#include "defines/used_includes.h"

#include "../services/webserver/webserver.h"
#include "../hashtable.h"

#include "json_worker.h"


/*#### DEFINES ####################*/
#define String_FromTextRange(tr) \
        PyString_FromStringAndSize((tr).first, \
                                   (size_t)((tr).afterLast - (tr).first))

#define DEFAULT_PAGE        "index.html"

/*#### TYPEDEFS ###################*/
struct url_struct {
  char* scheme;
  char* user;
  char* host;
  char* port;
  char* filename;
};

/*#### Function Prototyps ###################*/
static int create_ds18b20_json(char **result);
static int create_log_json(char **result);
static int create_status_json(char **result);
static int create_config_json(char **result);
static int create_shaker_json(char **result);
static int create_sekair_json(char **result);
static int ruettler_json_status(json_object * jobj);

void post_json_parse(json_object* jobj);

int json_Shaker_Post(char **result, struct connection_info_struct *con_info);
int json_DS18B20_Post(char **result, struct connection_info_struct *con_info);
int json_status_Post(char **result, struct connection_info_struct *con_info);

/*#### VARIABLE ###################*/
char * json_data_type;

//--------------------------------------------------------------------------------------------------
// Name:      init_json
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int init_json()
{
  register_http_handler((http_request_handler) & handle_http_request);
  register_json_handler((http_json_handler) & handle_json);
}


//--------------------------------------------------------------------------------------------------
// Name:      browser_get
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int browser_get(const UriUriA uri, char **result)
{
  int rest_command = HVS_URI_ERROR;

  char* filename = NULL;

  if (uri.pathHead != NULL)
    filename = strndup(uri.pathHead->text.first - 1,
                       uri.pathTail->text.afterLast - uri.pathHead->text.first + 1);


  if (filename != NULL)
  {
    *result = strdup(filename);
    if (strlen(filename) < 2)
    {
      *result = strdup(DEFAULT_PAGE);
    }
    else
    {
      if (!strcmp(filename, "/jsonsekair"))
      {
        rest_command = HVS_SEND_JSON_SEKAIR;
      }
      else if (!strcmp(filename, "/jsonshaker"))
      {
        rest_command = HVS_SEND_JSON_SHAKER;
      }
      else if (!strcmp(filename, "/jsonstatus"))
      {
        rest_command = HVS_SEND_JSON_STATUS;
      }
      else if (!strcmp(filename, "/jsonconfig"))
      {
        rest_command = HVS_SEND_JSON_CONFIG;
      }
      else if (!strcmp(filename, "/jsonlog"))
      {
        rest_command = HVS_SEND_JSON_LOG;
      }
      else if (!strcmp(filename, "/jsonds18b20"))
      {
        rest_command = HVS_SEND_JSON_DS18B20;
      }
      else
        rest_command = HVS_SEND_PAGE;
    }
    free(filename);
  }
  else
  {
    *result = strdup(DEFAULT_PAGE);
    rest_command = HVS_SEND_PAGE;
  }

  return rest_command;
}

//--------------------------------------------------------------------------------------------------
// Name:      browser_post
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int browser_post(const UriUriA uri)
{
  int rest_command = HVS_URI_ERROR;

  /*
    char* scheme = strndup(uri.scheme.first, uri.scheme.afterLast - uri.scheme.first);
    char* user = strndup(uri.userInfo.first, uri.userInfo.afterLast - uri.userInfo.first);
    char* host = strndup(uri.hostText.first, uri.hostText.afterLast - uri.hostText.first);
    char* port = strndup(uri.portText.first, uri.portText.afterLast - uri.portText.first);
   */

  char* filename = strndup(uri.pathHead->text.first - 1,
                           uri.pathTail->text.afterLast - uri.pathHead->text.first + 1);


  if (filename != NULL)
  {
    if (!strcmp(filename, "/jsonds18b20"))
    {
      rest_command = HVS_JSON_DS18B20_POST;
    }
    else if (!strcmp(filename, "/shaker"))
    {
      rest_command = HVS_JSON_SHAKER_POST;
    }
    else if (!strcmp(filename, "/jsonconfig"))
    {
      rest_command = HVS_JSON_CONFIG_POST;
    }
    else if (!strcmp(filename, "/status"))
    {
      rest_command = HVS_JSON_STATUS_POST;
    }
    free(filename);
  }
  return rest_command;
}


struct JsonParser* create_json_processor() { }

//--------------------------------------------------------------------------------------------------
// Name:      json_parser_new
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

JsonParser *json_parser_new(void)
{
  JsonParser *parser;

  parser = calloc(1, sizeof (JsonParser));
  parser->tok = json_tokener_new();

  return parser;
}

//--------------------------------------------------------------------------------------------------
// Name:      handle_json
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int handle_json(void *coninfo_cls, size_t size)
{
  struct connection_info_struct *con_info = coninfo_cls;
  JsonParser *parser = con_info->parser;


  if (parser->tok == NULL)
    return FALSE;

  parser->root = json_tokener_parse_ex(parser->tok, (char*)con_info->data, (int)size);

  if (parser->tok->err != json_tokener_success)
  {
    json_object_put(parser->root);
    parser->root = NULL;
    return FALSE;
  }

  return TRUE;
}

//--------------------------------------------------------------------------------------------------
// Name:      handle_http_request
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int handle_http_request(const char *url, const char *method, char **page,
                    void **con_cls)
{
  int ret;
  int req_command = 0;


  UriParserStateA state;
  UriUriA uri;

  state.uri = &uri;
  if (uriParseUriA(&state, url) != URI_SUCCESS)
  {
    /* Failure */
    printf("failed parse");
    uriFreeUriMembersA(&uri);
  }

  if ((strcmp(method, "GET")) == 0)
  {
    req_command = browser_get(uri, page);


  }
  else if ((strcmp(method, "POST")) == 0)
  {
    req_command = browser_post(uri);

  }
  else
  {
    // error
  }

  struct connection_info_struct *con_info = *con_cls;

  switch (req_command)
  {

    case HVS_SEND_PAGE:

      ret = req_command;
      break;
    case HVS_SEND_JSON_STATUS:
      create_status_json(page);
      ret = HVS_SEND_MEMORY;
      break;
    case HVS_SEND_JSON_CONFIG:
      create_config_json(page);
      ret = HVS_SEND_MEMORY;
      break;
    case HVS_SEND_JSON_LOG:
      create_log_json(page);
      ret = HVS_SEND_MEMORY;
      break;
    case HVS_SEND_JSON_DS18B20:
      create_ds18b20_json(page);
      ret = HVS_SEND_MEMORY;
      break;
    case HVS_SEND_JSON_SEKAIR:
      create_sekair_json(page);
      ret = HVS_SEND_MEMORY;
      break;
    case HVS_SEND_JSON_SHAKER:
      create_shaker_json(page);
      ret = HVS_SEND_MEMORY;
      break;

      /*** POST FUNCTION ****/
    case HVS_JSON_CONFIG_POST:
      json_Config_Post(page, con_info);
      ret = HVS_SEND_MEMORY;
      break;
    case HVS_JSON_DS18B20_POST:
      json_DS18B20_Post(page, con_info);
      ret = HVS_SEND_MEMORY;
      break;
    case HVS_JSON_SHAKER_POST:
      json_Shaker_Post(page, con_info);
      ret = HVS_SEND_MEMORY;
      break;
    case HVS_JSON_STATUS_POST:
      json_status_Post(page, con_info);
      ret = HVS_SEND_MEMORY;
      break;

    default:
      // error, unknown command
      ret = 1;
  }

  uriFreeUriMembersA(&uri);

  return ret;
}

//--------------------------------------------------------------------------------------------------
// Name:      get_shaker_json_state
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int get_shaker_json_state(char **result, char *msg)
{


  /*Creating a json object*/
  json_object * jobj = json_object_new_object();

  ruettler_json_status(jobj);
  json_object_object_add(jobj, "message", json_object_new_string(msg));


  /*Now printing the json object*/
  //printf("The json object created: %sn", json_object_to_json_string(jobj));
  *result = strdup(json_object_to_json_string(jobj));
  json_object_put(jobj);
  return 1;
}

//--------------------------------------------------------------------------------------------------
// Name:      json_status_Post
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int json_status_Post(char **result, struct connection_info_struct *con_info)
{
  if (strcmp(con_info->action, "control_reset") == 0)
  {
    if (get_regelung_active())
    {
      regelung_activ(EVENT_INP_REGELUNG_DEACTIVE);
      regelung_activ(EVENT_INP_REGELUNG_ACTIVE);
      *result = strdup("{\"message\":\"Regelung zurück gesetzt\"}");
    }
    else
      *result = strdup("{\"message\":\"Regelung nicht aktiv\"}");

  }
  else
  {
    *result = strdup("{\"message\":\"fehler\"}");
  }
}

//--------------------------------------------------------------------------------------------------
// Name:      json_Shaker_Post
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int json_Shaker_Post(char **result, struct connection_info_struct *con_info)
{

  if (strcmp(con_info->action, "shaker_on") == 0)
  {
    run_shaker(0, 0);
    get_shaker_json_state(result, "Rüttler gestartet");

  }
  else if (strcmp(con_info->action, "shaker_off") == 0)
  {
    stop_shaker(0);
    get_shaker_json_state(result, "Rüttler gestoppt");
  }
  else if (strcmp(con_info->action, "shaker_reset") == 0)
  {
    reset_shaker();
    *result = strdup("{\"message\":\"zurückgesetzt\"}");
  }
  else
  {
    *result = strdup("{\"message\":\"fehler\"}");
  }

}

//--------------------------------------------------------------------------------------------------
// Name:      send_default_config
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int send_default_config(char **result)
{
  json_object * ResultObj_Values = json_object_new_object();
  json_object *jarray_values = json_object_new_array();

  /*Creating a json object*/
  json_object * jobj = json_object_new_object();

  json_object_object_add(jobj, "spez_wasser", json_object_new_double(DEF_SPEZ_WAERMEKAP_WASSER / 1000.0));
  json_object_object_add(jobj, "heizwert_holz", json_object_new_double(DEF_HEIZWERT_HOLZ));

  json_object_object_add(jobj, "max_puffer_temp", json_object_new_int(DEF_MAX_PUFFER_TEMP));
  json_object_object_add(jobj, "puffer_count", json_object_new_int(1));

  json_object_array_add(jarray_values, json_object_new_int(DEF_PUFFERVOLUMEN));
  json_object_object_add(jobj, "volume", jarray_values);

  json_object_object_add(jobj, "abgas_end_temp", json_object_new_int(DEF_ABGAS_END_TEMP));
  json_object_object_add(jobj, "shake_start", json_object_new_double(DEF_SHAKER_AKTIVE));
  json_object_object_add(jobj, "shaking", json_object_new_double(DEF_SHAKING));
  json_object_object_add(jobj, "shake_time", json_object_new_int(DEF_SHAKER_RUN));
  json_object_object_add(jobj, "shake_pause", json_object_new_int(DEF_SHAKER_PAUSE));
  json_object_object_add(jobj, "shake_max_count", json_object_new_int(DEF_SHAKING_COUNT));

  //DEF_PUFFERVOLUMEN 2000

  /*Now printing the json object*/
  //printf("The json object created: %sn", json_object_to_json_string(jobj));
  *result = strdup(json_object_to_json_string(jobj));
  json_object_put(jobj);
  return 1;
}

//--------------------------------------------------------------------------------------------------
// Name:      json_Config_Post
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int json_Config_Post(char **result, struct connection_info_struct *con_info)
{
  //post_json_parse(con_info->data);
  if (strcmp(con_info->action, "default") == 0)
  {
    send_default_config(result);
  }
  else if (strcmp(con_info->action, "save") == 0)
  {
    JsonParser *parser = con_info->parser;
    if (parser->tok->err == json_tokener_success)
    {
      post_json_parse(parser->root);
      *result = strdup("{\"msg\":\"gespeichert\"}");
    }
    else
    {
      *result = strdup("{\"msg\":\"fehler\"}");
    }

  }

  //*result = strdup("{\"alert\":\"gespeichert\"}");
}

//--------------------------------------------------------------------------------------------------
// Name:      json_DS18B20_Post
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int json_DS18B20_Post(char **result, struct connection_info_struct *con_info)
{
  JsonParser *parser = con_info->parser;
  if (parser->tok->err == json_tokener_success)
  {
    post_json_parse(parser->root);
    *result = strdup("{\"message\":\"gespeichert\"}");
  }
  else
  {
    *result = strdup("{\"message\":\"fehler\"}");
  }
}

//--------------------------------------------------------------------------------------------------
// Name:      config_data_evaluate
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

void config_data_evaluate()
{

  char * sShakeTime = get_hashtab("shake_time");
  char * sShaking = get_hashtab("shaking");
  char * sPufferCount = get_hashtab("puffer_count");
  int iPufferCount = atoi(sPufferCount);

  char * sMaxPufferTemp = get_hashtab("max_puffer_temp");
  char * sMinPufferTemp = get_hashtab("min_puffer_temp");
  char * sShakeMaxCount = get_hashtab("shake_max_count");
  char * sShakePause = get_hashtab("shake_pause");

  char * sHeizwertHolz = get_hashtab("heizwert_holz");
  char * sShakeStart = get_hashtab("shake_start");
  char * sAbgas_end_temp = get_hashtab("abgas_end_temp");

  if (iPufferCount > 0)
  {
    app_config_set_puffer_count(iPufferCount);
    char buf[9];
    for (int i = 0; i < iPufferCount; i++)
    {
      sprintf(buf, "volume%d", i);
      char*volume = get_hashtab(buf);
      if (volume != NULL)
        app_config_set_puffer_volume(i, atoi(volume));
    }

  }

  if (sHeizwertHolz != NULL) app_config_set_heizwert_holz(atof(sHeizwertHolz));
  if (sMaxPufferTemp != NULL) app_config_set_max_puffer_temp(atoi(sMaxPufferTemp));
  if (sMinPufferTemp != NULL) app_config_set_min_puffer_temp(atoi(sMinPufferTemp));
  if (sShakeStart != NULL) app_config_set_ruettler_activ(atof(sShakeStart));
  if (sShaking != NULL) app_config_set_ruettler_deactiv(atof(sShaking));
  if (sShakeTime != NULL) app_config_set_ruettler_on_time(atoi(sShakeTime));
  if (sShakePause != NULL) app_config_set_ruettler_off_time(atoi(sShakePause));
  if (sShakeMaxCount != NULL) app_config_set_ruettler_loop(atoi(sShakeMaxCount));
  if (sAbgas_end_temp != NULL) app_config_set_abgas_end_temp(atoi(sAbgas_end_temp));

}

//--------------------------------------------------------------------------------------------------
// Name:      ds18b20_data_evaluate
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

void ds18b20_data_evaluate()
{
  char * sId = get_hashtab("ID");
  char * sPuffer = get_hashtab("Puffer");
  char * sPos = get_hashtab("Position");
  int nPuffer;

  if (!strcmp(sPuffer, "no"))
  {
    nPuffer = -1;
  }
  else if (!strcmp(sPuffer, "out"))
  {
    nPuffer = 255;
  }
  else
    nPuffer = atoi(sPuffer);

  app_config_set_pufferpos(sId, nPuffer, atoi(sPos));
}


//--------------------------------------------------------------------------------------------------
// Name:      print_json_value
// Function:  printing the value corresponding to boolean, double, integer and strings
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

void print_json_value(json_object *jobj)
{
  enum json_type type;
  printf("type: ", type);
  type = json_object_get_type(jobj); /*Getting the type of the json object*/
  switch (type)
  {
    case json_type_boolean: printf("json_type_boolean\n");
      printf("value: %s\n", json_object_get_boolean(jobj) ? "true" : "false");
      break;
    case json_type_double: printf("json_type_double\n");
      printf("          value: %lf\n", json_object_get_double(jobj));
      break;
    case json_type_int: printf("json_type_int\n");
      printf("          value: %d\n", json_object_get_int(jobj));
      break;
    case json_type_string: printf("json_type_string\n");
      printf("          value: %s\n", json_object_get_string(jobj));
      break;
  }

}

//--------------------------------------------------------------------------------------------------
// Name:      json_parse_array
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

void json_parse_array(json_object *jobj, char *key)
{
  int json_parse(json_object * jobj); /*Forward Declaration*/
  enum json_type type;

  json_object *jarray = jobj; /*Simply get the array*/
  if (key)
  {
    json_object_object_get_ex(jobj, key, &jarray); /*Getting the array if it is a key value pair*/
  }

  int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
  printf("Array Length: %d\n", arraylen);
  int i;
  json_object * jvalue;

  for (i = 0; i < arraylen; i++)
  {
    jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
    type = json_object_get_type(jvalue);
    if (type == json_type_array)
    {
      json_parse_array(jvalue, NULL);
    }
    else if (type != json_type_object)
    {
      char *arraykey = calloc(strlen(key) + 3, sizeof (char));

      if (arraykey)
      {
        sprintf(arraykey, "%s%d", key, i);
        add_hashtab(arraykey, (char*)json_object_get_string(jvalue));
      }
      printf("value[%d]: ", i);
      print_json_value(jvalue);
      free(arraykey);
    }
    else
    {
      json_parse(jvalue);
      if (!strcmp(json_data_type, "ds18b20_data"))
      {
        ds18b20_data_evaluate();
      }
      else if (!strcmp(json_data_type, "config_data"))
      {
        config_data_evaluate();
      }
      /*
            if (!strcmp(json_data_type, "/jsonconfig")) {
              return JSONCONFIG(con_cls, connection, upload_data, upload_data_size);
            }
            if (!strcmp(json_data_type, "/jsonlog")) {
              return JSONLOG(con_cls, connection, upload_data, upload_data_size);
            }
            if (!strcmp(json_data_type, "/jsonds18b20")) {
              return JSONDS18B20(con_cls, connection, upload_data, upload_data_size);
            }
       */
    }
  }
}


//--------------------------------------------------------------------------------------------------
// Name:      json_parse
// Function:  Parsing the json object
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

int json_parse(json_object * jobj)
{
  enum json_type type;

  json_object_object_foreach(jobj, key, val)
  { /*Passing through every array element*/
    if (!json_data_type)
    {
      json_data_type = strdup(key);
      if (!json_data_type) return 1;
    }
    printf("type: ", type);
    type = json_object_get_type(val);
    switch (type)
    {
      case json_type_boolean:
      case json_type_double:
      case json_type_int:
        print_json_value(val);
        break;
      case json_type_string:
        add_hashtab(key, (char*)json_object_get_string(val));
        print_json_value(val);
        break;
      case json_type_object: printf("json_type_object\n");
        json_object_object_get_ex(jobj, key, &jobj);
        json_parse(jobj);
        break;
      case json_type_array: printf("type: json_type_array, ");
        json_parse_array(jobj, key);
        break;
    }
  }
  return 0;
}

//--------------------------------------------------------------------------------------------------
// Name:      post_json_parse
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

void post_json_parse(json_object* jobj)
{
  // char * string = "{\"abc\":[{\"ID\":\"28-0000031bd73c\",\"CRC\":\"1\",\"Puffer\":\"0\",\"Position\":\"\",\"Temperatur\":\"21\"},{\"ID\":\"28-0000031bb5c2\",\"CRC\":\"1\",\"Puffer\":\"0\",\"Position\":\"\",\"Temperatur\":\"21.062\"},{\"ID\":\"28-0000031be1ca\",\"CRC\":\"1\",\"Puffer\":\"0\",\"Position\":\"\",\"Temperatur\":\"20.75\"},{\"ID\":\"28-0000031b94da\",\"CRC\":\"1\",\"Puffer\":\"0\",\"Position\":\"\",\"Temperatur\":\"21.062\"},{\"ID\":\"28-000004b59a05\",\"CRC\":\"1\",\"Puffer\":\"0\",\"Position\":\"\",\"Temperatur\":\"21.062\"},{\"ID\":\"28-000004b6309b\",\"CRC\":\"1\",\"Puffer\":\"0\",\"Position\":\"\",\"Temperatur\":\"20.687\"}]}";
  init_hashtab();
  // json_object * jobj = json_tokener_parse(json);
  json_parse(jobj);
  if (!strcmp(json_data_type, "config_data"))
  {
    config_data_evaluate();
    app_config_save();
  }
  else if (!strcmp(json_data_type, "ds18b20_data"))
  {
    app_config_write_ds18b20();
    app_config_refresh_pufferpos();
  }
  cleanup_hashtab();
}

static int create_log_json(char **result) { }

//--------------------------------------------------------------------------------------------------
// Name:      create_ds18b20_json
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int create_ds18b20_json(char **result)
{
  DS18B20_struct *ds18b20_dev;
  int count = ds18b20_getDeviceCount();

  //json_object * ResultObj_Values = json_object_new_object();
  json_object *jarray_values = json_object_new_array();

  for (int i = 0; i < count; i++)
  {
    ds18b20_dev = ds18b20_getDevice(i);

    json_object * ResultObj_Values = json_object_new_object();

    json_object_object_add(ResultObj_Values, "puffer", json_object_new_int(ds18b20_dev->puf_id));
    json_object_object_add(ResultObj_Values, "pos", json_object_new_int(ds18b20_dev->puf_pos));
    json_object_object_add(ResultObj_Values, "id", json_object_new_string(ds18b20_dev->_deviceid));
    json_object_object_add(ResultObj_Values, "crc", json_object_new_int(ds18b20_dev->crc));
    if (ds18b20_dev->crc)
    {
      json_object_object_add(ResultObj_Values, "temp", json_object_new_double(ds18b20_dev->tempGrad));
    }
    else
    {
      json_object_object_add(ResultObj_Values, "temp", json_object_new_string("Fehler"));
    }

    json_object_array_add(jarray_values, ResultObj_Values);
  }

  /*Creating a json object*/
  json_object * jobj = json_object_new_object();

  json_object_object_add(jobj, "puffer_count", json_object_new_int(app_config_get_puffer_count()));
  json_object_object_add(jobj, "count", json_object_new_int(count));
  json_object_object_add(jobj, "aaData", jarray_values);

  /*Now printing the json object*/
  //dbg_printf("The json object created: %sn", json_object_to_json_string(jobj));
  *result = strdup(json_object_to_json_string(jobj));
  json_object_put(jobj);
  return 1;
}

//--------------------------------------------------------------------------------------------------
// Name:      create_sekair_json
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int create_sekair_json(char **result)
{
  /*Creating a json object*/
  json_object * jobj = json_object_new_object();

  if (app_config_get_use_lambdacheck())
    json_object_object_add(jobj, "lambdacheck", json_object_new_double(lambda_check_last_value()));

  *result = strdup(json_object_to_json_string(jobj));
  json_object_put(jobj);
  return 1;
}

//--------------------------------------------------------------------------------------------------
// Name:      create_shaker_json
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int create_shaker_json(char **result)
{
  /*Creating a json object*/
  json_object * jobj = json_object_new_object();

  ruettler_json_status(jobj);


  *result = strdup(json_object_to_json_string(jobj));
  json_object_put(jobj);
  return 1;
}

//--------------------------------------------------------------------------------------------------
// Name:      create_config_json
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int create_config_json(char **result)
{
  json_object * ResultObj_Values = json_object_new_object();
  json_object *jarray_values = json_object_new_array();

  int app_config_get_puffer_volume();

  /*Creating a json object*/
  json_object * jobj = json_object_new_object();

  json_object_object_add(jobj, "spez_wasser", json_object_new_double(app_config_get_spez_wasser() / 1000.0));
  json_object_object_add(jobj, "heizwert_holz", json_object_new_double(app_config_get_heizwert_holz()));

  json_object_object_add(jobj, "max_puffer_temp", json_object_new_int(app_config_get_max_puffer_temp()));
  json_object_object_add(jobj, "min_puffer_temp", json_object_new_int(app_config_get_min_puffer_temp()));
  int iPufferCount = app_config_get_puffer_count();
  json_object_object_add(jobj, "puffer_count", json_object_new_int(iPufferCount));

  for (int i = 0; i < iPufferCount; i++)
  {
    json_object_array_add(jarray_values, json_object_new_int(app_config_get_puffer_volume(i)));
  }
  json_object_object_add(jobj, "volume", jarray_values);


  json_object_object_add(jobj, "abgas_end_temp", json_object_new_int(app_config_get_abgas_end_temp()));
  json_object_object_add(jobj, "shake_start", json_object_new_double(app_config_get_ruettler_activ()));
  json_object_object_add(jobj, "shaking", json_object_new_double(app_config_get_ruettler_deactiv()));
  json_object_object_add(jobj, "shake_time", json_object_new_int(app_config_get_ruettler_on_time()));
  json_object_object_add(jobj, "shake_pause", json_object_new_int(app_config_get_ruettler_off_time()));
  json_object_object_add(jobj, "shake_max_count", json_object_new_int(app_config_get_ruettler_loop()));

  /*Now printing the json object*/
  //printf("The json object created: %sn", json_object_to_json_string(jobj));
  *result = strdup(json_object_to_json_string(jobj));
  json_object_put(jobj);
  return 1;
}

//--------------------------------------------------------------------------------------------------
// Name:      used_json_status
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int used_json_status(json_object * jobj)
{
  json_object * ResultObj_Use = json_object_new_object();
  /*Each of these is like a key value pair*/

  //json_object_object_add(ResultObj_Use, "abgas", json_object_new_boolean(app_config_get_use_pt100()));
  json_object_object_add(ResultObj_Use, "abgas_temp", json_object_new_boolean(app_config_get_use_abgas_temp()));

  json_object_object_add(ResultObj_Use, "brenner_temp", json_object_new_boolean(app_config_get_use_k_sonde()));
  json_object_object_add(ResultObj_Use, "o2", json_object_new_boolean(app_config_get_use_lambdasonde()));

  json_object_object_add(ResultObj_Use, "temp", json_object_new_boolean(app_config_get_use_ds18b20()));
  json_object_object_add(ResultObj_Use, "abgasverlust", json_object_new_boolean(app_config_get_use_abgas_temp() && app_config_get_use_lambdasonde()));
  json_object_object_add(ResultObj_Use, "nachlegen", json_object_new_boolean(app_config_get_use_ds18b20()));
  json_object_object_add(ResultObj_Use, "lambdacheck", json_object_new_boolean(app_config_get_use_lambdacheck()));
  json_object_object_add(ResultObj_Use, "ruettler", json_object_new_boolean(app_config_get_use_ruettler()));

  json_object_object_add(jobj, "uses", ResultObj_Use);
}

//--------------------------------------------------------------------------------------------------
// Name:      values_json_status
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int values_json_status(json_object * jobj)
{
  char buffer[20];
  json_object * ResultObj_Values = json_object_new_object();

  if (app_config_get_use_abgas_temp())
  {
    if (app_config_get_used_abgas_sonde() == PT100)
    {
      json_object_object_add(ResultObj_Values, "abgas_temp", json_object_new_int(pt100_last_value()));
    }
    else
    {
      json_object_object_add(ResultObj_Values, "abgas_temp", json_object_new_int(temp_abgas_last_value()));
    }
  }
  if (app_config_get_use_k_sonde())
    json_object_object_add(ResultObj_Values, "brenner_temp", json_object_new_int(temp_brenner_last_value()));

  if (app_config_get_use_lambdasonde())
    json_object_object_add(ResultObj_Values, "o2", json_object_new_int(o2_last_value()));

  if (app_config_get_use_abgas_temp() && app_config_get_use_lambdasonde())
  {
    double abgasverlust = get_abgasverlust();
    sprintf(buffer, "%.2f", abgasverlust);

    json_object_object_add(ResultObj_Values, "abgasverlust", json_object_new_string(buffer));
  }

  if (app_config_get_use_lambdacheck())
    json_object_object_add(ResultObj_Values, "lambdacheck", json_object_new_double(lambda_check_last_value()));

  json_object_object_add(jobj, "values", ResultObj_Values);
}

//--------------------------------------------------------------------------------------------------
// Name:      max_json_status
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int max_json_status(json_object * jobj)
{
  json_object * ResultObj_Max = json_object_new_object();


  if (app_config_get_use_abgas_temp())
  {
    if (app_config_get_used_abgas_sonde() == PT100)
    {
      json_object_object_add(ResultObj_Max, "abgas_temp", json_object_new_int(pt100_max_value()));
    }
    else
    {
      json_object_object_add(ResultObj_Max, "abgas_temp", json_object_new_int(temp_abgas_max_value()));
    }
  }
  if (app_config_get_use_k_sonde())
    json_object_object_add(ResultObj_Max, "brenner_temp", json_object_new_int(temp_brenner_max_value()));



  json_object_object_add(ResultObj_Max, "lambdacheck", json_object_new_double(lambda_check_max_value()));

  json_object_object_add(jobj, "max", ResultObj_Max);
}

//--------------------------------------------------------------------------------------------------
// Name:      control_json_status
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int control_json_status(json_object * jobj)
{
  json_object * ResultObj_Values = json_object_new_object();
  json_object * Obj_Values = json_object_new_object();
  json_object *jarray = json_object_new_array();
  char buffer[20];

  json_object_object_add(ResultObj_Values, "state", json_object_new_boolean(get_regelung_active()));

  time_t zeit = get_starttime();

  if (zeit)
  {
    struct tm *stime;
    stime = localtime(&zeit);
    Obj_Values = json_object_new_object();
    strftime(buffer, sizeof (buffer), "%H:%M:%S", stime);
    json_object_object_add(Obj_Values, "Startzeit", json_object_new_string(buffer));
    json_object_array_add(jarray, Obj_Values);
  }

  zeit = get_endtime();
  if (zeit)
  {
    struct tm *etime;
    etime = localtime(&zeit);
    Obj_Values = json_object_new_object();
    strftime(buffer, sizeof (buffer), "%H:%M:%S", etime);
    json_object_object_add(Obj_Values, "Endzeit", json_object_new_string(buffer));
    json_object_array_add(jarray, Obj_Values);
  }
  json_object_object_add(ResultObj_Values, "times", jarray);

  //json_object_array_add(jarray_sollwert, ResultObj_Values);
  //printf("The json object created: %s\n", json_object_to_json_string(jarray_sollwert));
  json_object_object_add(jobj, "control", ResultObj_Values);
}

//--------------------------------------------------------------------------------------------------
// Name:      sollwert_json_status
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int sollwert_json_status(json_object * jobj)
{

  json_object * Obj_Values = json_object_new_object();
  char buffer[20];

  double ist = get_ist_soll();
  double ist_nicht = get_ist_nicht_soll();

  sprintf(buffer, "%02u:%02u", (uint)(ist / 60.0), (uint)(fmod(ist, 60.0)));
  json_object_object_add(Obj_Values, "ist", json_object_new_string(buffer));
  sprintf(buffer, "%02u:%02u", (uint)(ist_nicht / 60.0), (uint)(fmod(ist_nicht, 60.0)));
  json_object_object_add(Obj_Values, "nicht", json_object_new_string(buffer));
  if (ist > 0)
  {
    double faktor = (ist - ist_nicht)*100.0 / ist;
    sprintf(buffer, "%.2f", faktor);
  }
  else
    sprintf(buffer, "---");

  json_object_object_add(Obj_Values, "faktor", json_object_new_string(buffer));

  //json_object_array_add(jarray_sollwert, ResultObj_Values);
  //printf("The json object created: %s\n", json_object_to_json_string(jarray_sollwert));
  json_object_object_add(jobj, "nominal_value", Obj_Values);

}

//--------------------------------------------------------------------------------------------------
// Name:      leistung_json_status
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int leistung_json_status(json_object * jobj)
{
  char buffer[20];
  time_t endtime = get_endtime();
  json_object * Obj_Values = json_object_new_object();
  if (endtime)
  {
    double diff = difftime(time(NULL), endtime);
    double verlust = app_config_get_endleistung() / diff /*/ 20.0*/;
    sprintf(buffer, "%02u:%02u", (uint)(diff / 60.0), (uint)(fmod(diff, 60.0)));
    json_object_object_add(Obj_Values, "zeit", json_object_new_string(buffer));
    json_object_object_add(Obj_Values, "verlust", json_object_new_double(verlust));
    json_object_object_add(jobj, "leistung", Obj_Values);
  }

}

//--------------------------------------------------------------------------------------------------
// Name:      ruettler_json_status
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int ruettler_json_status(json_object * jobj)
{
  json_object * ResultObj_Values1 = json_object_new_object();
  json_object * ResultObj_Values = json_object_new_object();

  json_object *jruettler_state;
  if (app_config_get_use_ruettler())
  {

    jruettler_state = json_object_new_boolean(get_shaker_state());

    // Rüttler
    json_object *jarray_values = json_object_new_array();

    json_object_object_add(ResultObj_Values1, "shaker_armed", json_object_new_boolean(get_control_armed()));
    json_object_object_add(ResultObj_Values1, "shaker_state", jruettler_state);

    struct shake_info* sinfo = NULL;
    char buffer[20];
    time_t time = get_starttime();
    double diff;
    while ((sinfo = get_shake_info(sinfo)) != NULL)
    {
      ResultObj_Values = json_object_new_object();
      diff = difftime(sinfo->begin, time);
      sprintf(buffer, "%02u:%02u", (uint)(diff / 60.0), (uint)(fmod(diff, 60.0)));
      json_object_object_add(ResultObj_Values, "start", json_object_new_string(buffer));
      if (sinfo->dauer)
      {
        diff = difftime(sinfo->dauer, sinfo->begin);
        sprintf(buffer, "%02u:%02u", (uint)(diff / 60.0), (uint)(fmod(diff, 60.0)));
      }
      else
      {
        sprintf(buffer, "--:--");
      }
      json_object_object_add(ResultObj_Values, "dauer", json_object_new_string(buffer));
      json_object_object_add(ResultObj_Values, "count", json_object_new_int(sinfo->count));
      json_object_array_add(jarray_values, ResultObj_Values);
    }
    json_object_object_add(ResultObj_Values1, "data", jarray_values);


    ResultObj_Values = json_object_new_object();

    json_object_object_add(ResultObj_Values, "state", jruettler_state);
    json_object_object_add(ResultObj_Values, "shake_count", json_object_new_int(get_shaker_shake_count()));
    json_object_object_add(ResultObj_Values, "run_count", json_object_new_int(get_shaker_run_count()));
    jarray_values = json_object_new_array();
    json_object_array_add(jarray_values, ResultObj_Values);

    json_object_object_add(ResultObj_Values1, "sum", ResultObj_Values);
  }
  json_object_object_add(jobj, "shaker", ResultObj_Values1);

}

//--------------------------------------------------------------------------------------------------
// Name:      puffer_json_status
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int puffer_json_status(json_object * jobj)
{
  char buffer[20];

  json_object * ResultObj_Values1 = json_object_new_object();

  // DS18B20
  DS18B20_struct *ds18b20_dev;
  int ds18b20_count = ds18b20_getDeviceCount();
  json_object *jDS18b20array_values = json_object_new_array();
  double puffer_temp = 0.0, out_temp = 0.0;
  int puffer_temp_count = 0, out_temp_count = 0;

  for (int i = 0; i < ds18b20_count; i++)
  {
    ds18b20_dev = ds18b20_getDevice(i);
    json_object * ResultObj_Values = json_object_new_object();

    json_object_object_add(ResultObj_Values, "puffer", json_object_new_int(ds18b20_dev->puf_id));
    json_object_object_add(ResultObj_Values, "pos", json_object_new_int(ds18b20_dev->puf_pos));
    if (ds18b20_dev->crc)
    {
      if (ds18b20_dev->puf_id == 255)
      {
        out_temp += ds18b20_dev->tempGrad;
        out_temp_count++;
      }
      else if (ds18b20_dev->puf_id != -1)
      {
        puffer_temp += ds18b20_dev->tempGrad;
        puffer_temp_count++;
      }
      json_object_object_add(ResultObj_Values, "temp", json_object_new_int((int)ds18b20_dev->tempGrad));

    }
    else
    {
      json_object_object_add(ResultObj_Values, "temp", json_object_new_string("Fehler"));
    }

    json_object_array_add(jDS18b20array_values, ResultObj_Values);
  }

  if (app_config_get_use_ds18b20())
  {
    double durch_temp = puffer_temp / (double)puffer_temp_count;
    double aussen_temp = out_temp / (double)out_temp_count;

    //jtemp_use = json_object_new_boolean(1);
    double kg = ((double)app_config_get_max_volume() * app_config_getd_spez_wasser() * (app_config_get_max_puffer_temp() - durch_temp)) / (3600.0 * app_config_get_heizwert_holz());
    double Q = ((double)app_config_get_max_volume() * app_config_getd_spez_wasser() * (ds18b20_get_durch_temp() - app_config_get_min_puffer_temp())) / (3600.0);

    sprintf(buffer, "%.2f", Q);
    json_object_object_add(ResultObj_Values1, "leistung", json_object_new_string(buffer));
    sprintf(buffer, "%.2f", kg);
    json_object_object_add(ResultObj_Values1, "nachlegen", json_object_new_string(buffer));
    sprintf(buffer, "%.2f", durch_temp);
    json_object_object_add(ResultObj_Values1, "puffer_durchschnitt", json_object_new_string(buffer));
    sprintf(buffer, "%.2f", aussen_temp);
    json_object_object_add(ResultObj_Values1, "outtemp", json_object_new_string(buffer));
  }

  json_object_object_add(ResultObj_Values1, "count", json_object_new_int(app_config_get_puffer_count()));
  json_object_object_add(ResultObj_Values1, "temps", jDS18b20array_values);

  json_object_object_add(jobj, "puffer", ResultObj_Values1);
}

//--------------------------------------------------------------------------------------------------
// Name:      create_status_json
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------

static int create_status_json(char **result)
{

  json_object * jobj = json_object_new_object();
  puffer_json_status(jobj);

  /*Each of these is like a key value pair*/
  values_json_status(jobj);
  max_json_status(jobj);


  used_json_status(jobj);

  control_json_status(jobj);
  // Rüttler
  ruettler_json_status(jobj);


  // Sollwerte
  sollwert_json_status(jobj);

  leistung_json_status(jobj);

  /*Now printing the json object*/
  //printf("The json object created: %s\n", json_object_to_json_string(jobj));
  *result = strdup(json_object_to_json_string(jobj));
  json_object_put(jobj);
  return 1;

}