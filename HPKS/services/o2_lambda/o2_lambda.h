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
 * File:   o2_lambda.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 11. Dezember 2012, 21:46
 */

#ifndef O2_LAMBDA_H
#define	O2_LAMBDA_H

#ifdef	__cplusplus
extern "C" {
#endif

#define WATCHDOG        0b01000000
#define SPI_ERROR       0b00010000
#define UBAT_HIGH       0b00001000
#define UBAT_LOW        0b00000100
#define SONDE_OVERHEAT  0b00000010
#define CJ_ERROR        0b00000001

#define DIAHG_DIAHD_ERROR 0b11000000
#define IA_IP_ERROR       0b00110000
#define UN_ERROR          0b00001100
#define VM_ERROR          0b00000011
  
int lambda_init(void);
void lambda_getData();
int lambda_run();
int lambda_fehler(void);
int lambda_get_single_error(int nError);
int lambda_CJFehler(void);
int lambda_get_single_cjerror(int nError);
MEASUREMENT_VIEW_struct * lambda_getViewMem();
int get_lastO2Value(double *O2value);
void lambda_startmeasurment();
void lambda_stopmeasurment();
int o2_last_value();

#ifdef	__cplusplus
}
#endif

#endif	/* O2_LAMBDA_H */

