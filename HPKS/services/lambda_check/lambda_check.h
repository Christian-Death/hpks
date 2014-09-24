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
 * File:   lambda_check.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 11. April 2013, 20:27
 */

#ifndef LAMBDA_CHECK_H
#define	LAMBDA_CHECK_H

#ifdef	__cplusplus
extern "C" {
#endif

  void lambda_check_init(void);
  void lambda_check_getData();
  MEASUREMENT_VIEW_struct * lambda_check_getViewMem();
  double lambda_check_last_value();
  double lambda_check_max_value();
  void reset_lambda_check_max();

#ifdef	__cplusplus
}
#endif

#endif	/* LAMBDA_CHECK_H */

