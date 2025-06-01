/*
 * Date Created: 07/09/24
 * Last Modified: 14/09/24
 *
 * Header file for EPS fault detection: pwr_mon_read_error
 * Used in fault detection firmware for a CubeSat Electrical Power System (EPS).
 * Implements polling process to gauge Power Monitor responsiveness.
 *
 * Author(s): Winston Fournier
 */

#ifndef PWR_MON_READ_ERROR_H_
#define PWR_MON_READ_ERROR_H_

#include <stdint.h>
#include <string.h>

extern uint8_t g_read_error; //Flag for recent failure to read device


/************** FUNCTION DEFS **************/

/**
  * @brief attempts to check power monitor's detected temperature
  *
  * @param None
  *
  * @retval 0 or -1, reflecting whether or not the operation has been successful
*/
int8_t temp_check();

/**
  * @brief attempts to check power monitor's detected voltage
  *
  * @param None
  *
  * @retval 0 or -1, reflecting whether or not the operation has been successful
*/
int8_t volt_check();

/**
  * @brief attempts to check power monitor's detected current
  *
  * @param None
  *
  * @retval 0 or -1, reflecting whether or not the operation has been successful
*/
int8_t current_check();

/**
  * @brief attempts to check power monitor's detected power
  *
  * @param None
  *
  * @retval 0 or -1, reflecting whether or not the operation has been successful
*/
int8_t power_check();

/**
  * @brief if power monitor has previously failed to check a register, attemps to check registers again after a delay
  *
  * @param None
  *
  * @retval 0 or -1, reflecting whether or not the operation has been successful
*/
int8_t follow_up_read();

/**
  * @brief checks daily if power monitor registers are operational
  *
  * @param None
  *
  * @retval 0 or -1, reflecting whether or not the operation has been successful
*/
int8_t daily_read();

/**
  * @brief detects pwr_mon_read_error, running appropriate helper functions
  *
  * @param None
  *
  * @retval None
*/
void detect_pwr_mon_read_error();

/**
  * @brief handles pwr_mon_read_error, entering safety mode
  *
  * @param None
  *
  * @retval None
*/
void handle_pwr_mon_read_error();


#endif // PWR_MON_READ_ERROR_H_
