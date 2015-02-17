
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
 * File:   webserver.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 19. Dezember 2012, 20:40
 */

#ifndef WEBSERVER_H
#define	WEBSERVER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    
/*#### TYPEDEFS ###################*/
typedef enum {
  UNKNOWN,
  POST,
  GET,
  HEAD
} HTTPMethod;

#include "../../json_worker.h"
    
    
struct connection_info_struct {
  int connectiontype;
  struct MHD_PostProcessor *postprocessor;
  int answercode;
  char *action;
  char *answerstring;
  char *data;
  char *page;
  JsonParser *parser;
};



typedef int (*http_request_handler)
     (const char *url, HTTPMethod method, char **page, void **);

typedef int (*http_json_handler)
     (void **, size_t size);

void register_http_handler(http_request_handler handler); 
void register_json_handler(http_json_handler handler); 

uint8_t webserver_start(int port);
uint8_t webserver_stop();
uint8_t webserver_isStarted();


#ifdef	__cplusplus
}
#endif

#endif	/* WEBSERVER_H */

