/* 
 * File:   system.c
 * Author: Rost
 *
 * Created on 13. Oktober 2014, 15:18
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/reboot.h>

//--------------------------------------------------------------------------------------------------
// Name:      system_reboot
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------
void system_reboot() 
{
  //reboot(RB_AUTOBOOT);
  system("sleep 5; shutdown -r now");
}

//--------------------------------------------------------------------------------------------------
// Name:      system_shutdown
// Function:  -
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------
void system_shutdown() 
{
//reboot(RB_HALT_SYSTEM);
system("sleep 5; shutdown -h now");
}

