
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
 * File:   utils.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 27. November 2012, 19:04
 */

//=== Includes =====================================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <termios.h>
#include <time.h>
#include <math.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>

#include "../windows.h"

#include "../defines/used_includes.h"
#include "utils.h"

//=== Preprocessing directives (#define) ===========================================================

//=== Type definitions (typedef) ===================================================================

//=== Global constants =============================================================================

//=== Global variables =============================================================================

//=== Local constants  =============================================================================

//=== Local variables ==============================================================================

//=== Local function prototypes ====================================================================

int getkey() {
    int character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    character = fgetc(stdin);

    /* restore the original terminal attributes */
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    return character;
}

const char *byte_to_binary(int x)
{
    static char b[9];
    char *p = b;
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        //strcat(b, ((x & z) == z) ? "1" : "0");
        *p++ = (x & z) ? '1' : '0';
    }

    return b;
}

int countchr(const char *str, char ch) {
   int count = 0;
   while (*str != '\0') {
      if (*str++ == ch) {
         count++;
      }
   }
   return count;
}

int roundUp(double numToRound) 
{ 
  return ceil(numToRound/10)*10;
} 

int roundDown(double numToRound) 
{ 
  return floor(numToRound/10)*10;
}

//--------------------------------------------------------------------------------------------------
// Name:        GetRaspberryHwRevision
// Function:  	Check wich Hardware is used:
//				http://www.raspberrypi.org/archives/1929
//	
//				Model B Revision 1.0 									2
//				Model B Revision 1.0 + ECN0001 (no fuses, D14 removed) 	3
//				Model B Revision 2.0 									4, 5, 6
//            
// Parameter: 	-
// Return:      0=no info , 1=HW Rev.1, 2=HW Rev.2
//--------------------------------------------------------------------------------------------------
/*
 * 
 */
int GetRaspberryHwRevision(void)
{	
	FILE *fp;
	char line[32];
	char s[32];
	int i;
	
	fp = fopen("/proc/cpuinfo", "r");		// open as file
	if(fp != NULL)
	{	
		while(fgets(line,32,fp))			// get line
		{
			sscanf(line,"%s : %x",(char*)&s,&i);		// parse for key and value
			if(strcmp(s,"Revision") == 0)				// check for "Revision"
			{			
				//printf("Found: %s=%i\r\n",s,i);
				if(i < 4)  return 1;
				else		return 2;
			}
		}
	}
	else
	{
		//printf("cpuinfo not available.\r\n"); 
		return 0;
	}
	//printf("no revision info available.\r\n"); 
	return 0;
}

//--------------------------------------------------------------------------------------------------
// Name:    	SleepMs
// Function:  	Sleep /Delay in Milliseconds
//            
// Parameter: 	Milliseconds
// Return:      -
//--------------------------------------------------------------------------------------------------
void SleepMs(uint32_t ms)
{
	bcm2835_delay(ms);
}

// Time for easy calculations

static unsigned long long epoch ;
// Initialise our epoch for millis()
void millis_init() {
   struct timeval tv ;
  gettimeofday (&tv, NULL) ;
  epoch = (tv.tv_sec * 1000000 + tv.tv_usec) / 1000 ;
}
unsigned int millis (void)
{
  struct timeval tv ;
  unsigned long long t1 ;

  gettimeofday (&tv, NULL) ;

  t1 = (tv.tv_sec * 1000000 + tv.tv_usec) / 1000 ;

  return (uint32_t)(t1 - epoch) ;
}


int get_ipaddr(char *buf, int buf_len)
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) 
    {
        perror("getifaddrs");
        return EXIT_FAILURE;
    }


    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr == NULL)
            continue;  

        s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        if((strcmp(ifa->ifa_name,"eth0")==0)&&(ifa->ifa_addr->sa_family==AF_INET))
        {
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                return EXIT_FAILURE;
            }
            printf("\tInterface : <%s>\n",ifa->ifa_name );
            printf("\t  Address : <%s>\n", host); 
            snprintf(buf, buf_len, "%s", host);
        }
    }

    freeifaddrs(ifaddr);
    return EXIT_SUCCESS;
}

void dbg_printf(char *str,...) {
  va_list arglist; 
  va_start(arglist, str); 
  vprintf(str, arglist);
  if(app_config_get_use_window()) {
    dbg_wnd_printf(str, arglist);
  }
 
}

void removeAll(char *p, char c) {
  char *f;

  while( f=strchr(p,c) )
    memmove(f,f+1,strlen(f));

}
