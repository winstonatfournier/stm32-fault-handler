/*
 * Date Created: 15/08/24
 * Last Modified: 14/09/24
 *
 * Header file for EPS fault detection: chronic_idle
 * Used in fault detection firmware for a CubeSat Electrical Power System (EPS).
 * Implements logic for identifying chronic idle behavior in a solar MPPT (maximum power point tracking).
 *
 * Author(s): Winston Fournier
 */

#ifndef CHRONIC_IDLE_H_
#define CHRONIC_IDLE_H_

#include <stdint.h>

#define SECONDARY_DEVICE_ADDRESS 0x00 //Placeholder: address depends on hardware configuration
#define POWER_MONITOR_ADDRESS 0 //Placeholder: address depends on hardware configuration

const uint16_t g_const_PASS_REQ = 7999; //Placeholder: number of loop iterations for ~1 minute delay
const float TEMP_CONVERT_FAC = 0.125; //Data sheet conversion factor in [°C/LSB]
const float VOLT_CONVERT_FAC = 3.125; //Data sheet conversion factor in [mV/LSB]
const uint8_t TRUE = 1;
const uint8_t FALSE = 0;
const int8_t ERROR = -1;


/************** FUNCTION DEFS **************/

/**
  * @brief detects fault case: chronic_idle;
  * every interval, checks if mppt is idle; prolonged consecutive idles triggers handler function (~4h)
  *
  * @param None
  *
  * @retval None
*/
void detect_chronic_idle();

/**
  * @brief converts raw temperature data from power monitor to degrees Celsius
  *
  * @param raw_temp_val raw temperature data obtained from power monitor
  *
  * @retval temperature in degrees Celsius
*/
float convert_raw_to_celsius(int16_t raw_temp_val);

/**
  * @brief compares detected temperature with established threshold to deduct if system is receiving adequate sun exposure,
  * suggesting that charging should be occuring
  * 
  * @param None
  *
  * @retval 1, 0, or -1 in the case of TRUE, FALSE or ERROR respectively
*/
int8_t check_if_in_daylight_temp();

/**
  * @brief converts raw voltage data from power monitor to millivolts
  *
  * @param raw_volt_val raw voltage data obtained from power monitor
  *
  * @retval power monitor shunt voltage in millivolts
*/
float convert_raw_to_mv(int16_t raw_volt_val);

/**
  * @brief compares detected voltage with established threshold to deduct if system is receiving adequate sun exposure
  * indicating that charging should be occuring
  *
  * @param None
  *
  * @retval 1, 0, or -1 in the case of TRUE, FALSE or ERROR respectively
*/
int8_t check_if_in_daylight_volt();

/**
  * @brief runs helper functions, power cycles mppt if necessary
  *
  * @param None
  *
  * @retval None
*/
void handle_chronic_idle();

#endif // CHRONIC_IDLE_H_
