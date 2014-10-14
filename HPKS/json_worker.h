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
 * File:   json_worker.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 20. August 2013, 19:43
 */

#ifndef JSON_WORKER_H
#define	JSON_WORKER_H

#ifdef	__cplusplus
extern "C" {
#endif



#define HVS_SEND_PAGE 1
#define HVS_URI_ERROR 255
#define HVS_SEND_MEMORY 2
    //#define MONETDB_REST_CREATE_DB 3
    //#define MONETDB_REST_DELETE_DB 4
    //#define MONETDB_REST_DB_INFO 5

    typedef enum JSON_GET_enum {
        HVS_SEND_JSON_STATUS = 6,
        HVS_SEND_JSON_CONFIG,
        HVS_SEND_JSON_REGELUNGACTIVE,
        HVS_SEND_JSON_LOG,
        HVS_SEND_JSON_DS18B20,
        HVS_SEND_JSON_SEKAIR,
        HVS_SEND_JSON_SHAKER,
    } JSON_GET_e;

    typedef enum JSON_POST_enum {
        HVS_JSON_DS18B20_POST = 20,
        HVS_JSON_SHAKER_POST,
        HVS_JSON_CONFIG_POST,
        HVS_JSON_STATUS_POST,
        HVS_JSON_SYSTEM_POST,
    } JSON_POST_e;

    //#define MONETDB_REST_PATH_ALLDBS "_all_dbs"
    //#define MONETDB_REST_PATH_UUIDS "_uuids"

    typedef struct {
        struct json_tokener *tok;
        struct json_object *root;
    } JsonParser;

    int init_json();
    int handle_http_request(const char *url, const char *method, char **page,
            void ** postdata);
    int handle_json(void *coninfo_cls, size_t size);
    JsonParser* json_parser_new();


#ifdef	__cplusplus
}
#endif

#endif	/* JSON_WORKER_H */

