/*
 * Date Created: 19/08/24
 * Last Modified: 08/09/24
 *
 * Header file for EPS fault detection: source_decay
 * Used in fault detection firmware for a CubeSat Electrical Power System (EPS).
 * Provides functions to log and predict long-term decay in input power.
 *
 * Author(s): Winston Fournier
 */

#ifndef SOURCE_DECAY_H_
#define SOURCE_DECAY_H_

#include <stdint.h>

extern uint8_t g_source_decay; //Global flag for occurence of 'source_decay' fault


/************** FUNCTION DEFS **************/

/**
  * @brief provides 'CURRENT_LSB' value required to convert raw power to watts as per data sheet
  * 
  * @param None
  *
  * @retval None
*/
float CURRENT_LSB();

/**
  * @brief converts raw power value provided by power monitor to watts
  * 
  * @param raw_power_val raw power value provided by power monitor
  *
  * @retval None
*/
float convert_raw_to_watts(int32_t raw_power_val);

/**
  * @brief logs current power to the appropriate log or rolling average; data is aggregated over time to conserve memory
  * 
  * @param None
  *
  * @retval 0 or -1, indicating operation success or ERROR respectively
*/
int8_t log_current_power();

/**
  * @brief detects fault case: source_decay; calls log_current_power() intermittently, calling 
  * handle_source_decay() when the first monthly check fails
  * 
  * @param None
  *
  * @retval None
*/
void detect_source_decay();

/**
  * @brief handles fault case: source_decay; sets global variable g_source_decay to 1 which affects 'chronic_idle.c'
  * 
  * @param None
  *
  * @retval None
*/
void handle_source_decay();


#endif // SOURCE_DECAY_H_
