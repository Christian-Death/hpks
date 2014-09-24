/* 
 * File:   k_sonde.h
 * Author: The Death
 *
 * Created on 11. Dezember 2012, 20:44
 */

#ifndef K_SONDE_H
#define	K_SONDE_H

#ifdef	__cplusplus
extern "C" {
#endif

  void k_sonde_init(void);
  void k_sonde_getData();
  MEASUREMENT_VIEW_struct * k_sonde_getViewMem();
  int k_sonde_last_value();
  int k_sonde_max_value();
  void reset_k_sonde_max();

#ifdef	__cplusplus
}
#endif

#endif	/* K_SONDE_H */

