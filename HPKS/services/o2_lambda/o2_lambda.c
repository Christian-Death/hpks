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
 * File:   o2_lambda.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 2. Dezember 2012, 11:52
 */


#include <defines/used_includes.h>
#include "../../driver/usart/usart.h"
#include "o2_lambda.h"

#define MAX_BUFFER_SIZE 1024  
#define CARRIAGE_RETURN '\r'
 
#define CALIBRATION_MODE "C\r" // Calibration Mode
#define NORMAL_MODE  "N\r" // Normal Mode
#define START_MEASUREMENT "H\r" //  Starte Messung
#define END_MEASUREMENT "D\r" //  Beende Messung
#define HIGH_COMMUNICATION "F\r" //  Schnelle �bertragung (5Hz)
#define LOW_COMMUNICATION "S\r" //  Langsame �bertragung (1Hz)
#define PLAINTEXT_MODE "T\r" //  Klartext Modus
#define CSVTEXT_MODE "E\r" //  ".csv" Modus (Excel-Kompatibles Format)

static bool is_Init = false;

MEASUREMENT_VIEW_struct O2Mem;
static int lambda_fd = -1; // filedescriptor f�r USART 
static unsigned int last_ms;

static int _lambda;
static double _Ip;
static double _o2;
static int _ref;
static double _Ubetrieb;
static int _bat;
static double _Ubat;


union {
  struct{
    uint8_t CJFehler:1;
    uint8_t Sondeoverheat:1;
    uint8_t UbatLow:1;
    uint8_t UbatHigh:1;
    uint8_t SPIFehler:1;
    uint8_t Systembereit:1;
    uint8_t Watchdog:1;
    uint8_t Kalibrierungsmodus:1;
  };
  uint8_t data;
}_status;
union {
  struct{
    uint8_t VN:2;
    uint8_t UN:2;
    uint8_t IA_IP:2;
    uint8_t DIAHG_DIAHD:2;
  };
  uint8_t data;
}_cj;

void lambda_auswerten(char *buffer);
uint8_t lambda_state = 0;
#define RUNNING 1
int lambda_init(void)
{
	uint8_t i;

	for(i=0;i<128;i++)  O2Mem.Mem[i] = 0;
  
  sprintf(O2Mem.cur_value, "XXX");
  // USART �ffnen
  if((lambda_fd = usart_open( "/dev/ttyAMA0",115200)) == -1)
    return -1;
  
  // Einstellungen senden
  usart_puts(lambda_fd, NORMAL_MODE);
  usart_puts(lambda_fd, LOW_COMMUNICATION);
  usart_puts(lambda_fd, CSVTEXT_MODE);
  
  usart_flush(lambda_fd);
  
  is_Init = true;
}

void lambda_startmeasurment() {
  uint8_t i;
  
  if(!lambda_fehler() && _status.Systembereit == 0 ) {
    usart_puts(lambda_fd, START_MEASUREMENT);
  	for(i=0;i<128;i++)  O2Mem.Mem[i] = 0;
  
    sprintf(O2Mem.cur_value, "XXX");
    lambda_state = RUNNING;
  }
}

void lambda_stopmeasurment() {
  if(_status.Systembereit && lambda_state == RUNNING) {
    usart_puts(lambda_fd, END_MEASUREMENT);
    lambda_state = 0;
  }
  
}
void lambda_getData()
{
  if(!is_Init) return;
  
	uint16_t s_temp;
  uint16_t i_temp;
	uint16_t i;
  int c = -1;
  char buffer[MAX_BUFFER_SIZE] = {0};
  char * pbuf ;
  int result;
		
  if(lambda_fd == -1)
    return;

    
  result = usart_get(lambda_fd, buffer, MAX_BUFFER_SIZE);
  if(result > 0) {
    pbuf = strtok(buffer, "\r\n"); // split using space as divider
    
    while (pbuf != NULL) {
      lambda_auswerten(pbuf);
      pbuf = strtok(NULL, "\r\n"); // split using space as divider
    }
  }
    


};


int lambda_fehler(void) {
  return (_status.Watchdog || _status.SPIFehler || _status.UbatHigh || _status.UbatLow || _status.Sondeoverheat || _status.CJFehler);
}

int lambda_run() {
  return lambda_state;
}

int lambda_get_single_error(int nError) {

  if(nError == WATCHDOG) return _status.Watchdog;
  else if(nError == SPI_ERROR) return _status.SPIFehler;
  else if(nError == UBAT_HIGH) return _status.UbatHigh;
  else if(nError == UBAT_LOW) return _status.UbatLow;
  else if(nError == SONDE_OVERHEAT) return _status.Sondeoverheat;
  else if(nError == CJ_ERROR) return _status.CJFehler;
  
  return -1;
}

int lambda_CJFehler(void) {
  return (_status.CJFehler);
}

int lambda_get_single_cjerror(int nError) {
  if(nError == DIAHG_DIAHD_ERROR) return _cj.DIAHG_DIAHD;
  else if(nError == IA_IP_ERROR) return _cj.IA_IP;
  else if(nError == UN_ERROR) return _cj.UN;
  else if(nError == VM_ERROR) return _cj.VN;

  
  return -1;
}

#define WATCHDAG_ERROR_STRING "Watchdog error!"
void lambda_auswerten(char *buffer) {

  char * nextWordPtr;
  int i;
  //printf("%s\r\n",buffer);
  if( countchr(buffer, ';') != 4) {
    if(strncmp(buffer, WATCHDAG_ERROR_STRING, strlen(WATCHDAG_ERROR_STRING)) == 0) {
      sprintf(O2Mem.cur_value, "ERROR" );
      _status.Watchdog = 1;
      lambda_state = 0;
    }
    return;
  }
  
  nextWordPtr = strtok(buffer, ";"); // split using space as divider
  _lambda = atoi(nextWordPtr);
  
  nextWordPtr = strtok(NULL,";");
  _ref = atoi(nextWordPtr);
 
  nextWordPtr = strtok(NULL,";");
  _bat = atoi(nextWordPtr);
 
  nextWordPtr = strtok(NULL,";");
  _status.data = atoi(nextWordPtr);  

  nextWordPtr = strtok(NULL,";");
  _cj.data = atoi(nextWordPtr);  
  
   _Ubetrieb = (1.22/_ref)*1023;
  _Ubat = ((5.0 * 4.9) / 1023) * _bat;
   
   
  if(lambda_fehler()){
    sprintf(O2Mem.cur_value, "ERROR" );
    
  }else if(lambda_state == RUNNING) {
    
    _Ip =  (_lambda * 1.22)/(250.0*17.0*62.0);  //in A
    
    _o2 = (_Ip*1000.0+0.035)/0.1221;
    //printf("  %u : %f : %f\r\n",_lambda, _Ip, _o2);
    sprintf(O2Mem.cur_value, "%3.1f%%", _o2);
    for(i=126;i>0;i--) {
        O2Mem.Mem[i+1] = O2Mem.Mem[i];
       
    }
    O2Mem.Mem[1] = O2Mem.Mem[0];
    O2Mem.Mem[0] = _o2;
  } else {
    sprintf(O2Mem.cur_value, "STOP");
  }
  
}

/*
Die erste Zeile des Datenpakets bzw. der erste Wert enth�lt den aktuellen 
Pumpstrom welcher in der Sonde flie�t.
Aus diesem Pumpstrom kann der exakte Lambdawert ermittelt werden.
   
Ip = (Lambda * 1.22)/(250*V*62)
        
V steht hier f�r den Verst�rkungsfaktor.
Befindet sich die Sonde in einer mageren Atmosph�re so ist der Wert "Lambda" 
positiv und die Verst�rkung V=17.
In fetten Atmosph�ren ist der Wert negativ und die Verst�rkung betr�gt V=8.

�Ref�
"Ref" steht f�r die Spannung der eingebauten Referenzspannung. Diese Spannung 
betr�gt 1,22V �1% w�hrend die Betriebsspannung zwischen 4,75 und 5,25V 
schwanken kann.
Da aus Kostengr�nden die Betriebsspannung als Referenz des ADC verwendet wird, 
kann mit der gegebenen Referenz die Schwankung ausgeglichen werden.

  U_betrieb = (Uref/Ref)*1023

�Bat�
�Bat� stellt den digitalisierten Wert f�r die Batterie- bzw. Versorgungsspannung dar. 
Die Spannung wird an einem Spannungsteiler gemessen (39k? zu 10k?)

  U_bat = ((5.0 * 4.9) / 1023) * Bat

 
Unterschreitet der Wert die 420er Marke (=10,5V) oder steigt er �ber 700 (=17,0V),
so wird die Messung beendet und in den Stand-By Modus gewechselt.
Es wurde eine Hysterese einprogrammiert. Dies bedeutet, dass der Betrieb erst bei 
11,0V bzw. 16,5V wieder aufgenommen wird.

�Status�
Im Register �Status� werden allgemeine Fehler und Betriebszust�nde, welche nicht 
vom CJ125 erfasst werden, abgelegt. Dies geschieht nach dem gleichen System wie 
bei Register �CJ�.
Das Register in Bin�rformat sieht aus wie folgt (MSB first):
IntF.7             |          |        |        |      |      |            | IntF.0
Kalibrierungsmodus | Watchdog | System | SPI    | Ubat | Ubat | Sonde      | CJ
                   |          | bereit | Fehler | high | low  | �berhitzt  | Fehler

Die Werte stehen f�r folgende Aussagen:
 Kalibrierungsm.: Die Elektronik befindet sich im Kalibrierungsmodus. Die 
                  angezeigten Werte entsprechen also nicht den real 
                  vorhandenen!
 Watchdog: Interner Programmfehler � Die Schaltung muss neu 
           gestartet werden.
 System bereit: Die Werte von �Lambda� sind verwertbar. 
                Betriebstemperatur ist erreicht.
 SPI Fehler: Fehler in der internen Kommunikation.
 Ubat high: Versorgungsspannung zu hoch (> 17,0V)
 Ubat low: Versorgungsspannung zu niedrig (< 10,5V)
 CJ-Fehler: Interner Fehler im CJ125. Siehe �CJ�.

�CJ�
Der Wert �CJ� enth�lt den Inhalt des CJ125 Diagnoseregisters und muss in einen 
Bin�rwert �bersetzt werden. Dann gilt folgende Auswertungstabelle (MSB first):
CJF.7  |        |       |       |    |    |    | CJF.0
DIAHG  | DIAHD  | IA/IP | IA/IP | UN | UN | VM | VM

Mit der im Datenblatt angegebenen Tabelle l�sst sich nun der Fehler in Klartext 
darstellen: 
Fehler-Bits |     DIAHG/DIAHD         |     IA/IP, UN, VM
    0 0     | Kurzschluss nach Masse  | Kurzschluss nach Masse
    0 1     | Heizung nicht angeschl. | Batterie schwach
    1 0     | Kurzschluss nach UBat   | Kurzschluss nach UBat
    1 1     | Kein Fehler             | Kein Fehler

Ist der Wert des �CJ� 255, so liegt kein Fehler vor und die Schaltung ist 
Betriebsbereit.
Nach jedem auslesen des Fehlerregisters, wird dieses gel�scht. Liegt ein Fehler vor, 
so wird automatisch die Heizung der Sonde abgeschaltet um eine �berhitzung zu 
vermeiden. Au�erdem wird der Anschluss �Error_out� auf High-Pegel gezogen.
Im Fehlerfall wird der Pumpstrom durch die Sonde und die Bestimmung des 
Nernstzellenwiderstandes gestoppt. Die Werte f�r UR und UA k�nnen also in dieser 
Zeit nicht verwendet werden.
Bei �lteren Sonden kann es vor allem w�hrend der Aufheizphase vermehrt zu 
Fehlern kommen. Diese verschwinden i. d. R. wenn die Betriebstemperatur erreicht 
ist.
Werden zu viele Fehler angezeigt, so ist dies ein Indiz daf�r, dass die Sonde 
ausgetauscht werden muss.
*/

MEASUREMENT_VIEW_struct * lambda_getViewMem() {
  return &O2Mem;
}

int o2_last_value() {
  return O2Mem.Mem[0];
}

int get_lastO2Value(double *O2value) {
  if(lambda_state == RUNNING) {
    *O2value = _o2;
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}