/*
 * Date Created: 10/08/24
 * Last Modified: 14/09/24
 *
 * Source file for EPS fault detection: chronic_idle
 * Used in fault detection firmware for a CubeSat Electrical Power System (EPS).
 * Implements logic for identifying chronic idle behavior in a solar MPPT (maximum power point tracking).
 *
 * Author(s): Winston Fournier
 */

#include "chronic_idle.h"
#include "mppt.h"
#include "load_switches.h"
#include "pwr_mon_read_error.h"
#include "source_decay.h"

static uint16_t pass_num = 0; //Main loop iteration counter regularly prompting system checks
static uint8_t consecutive_idles = 0; //Bitfield tracking recent MPPT idles; 0xFF means persistent idle (~4h)
static uint8_t mppt_was_reset = FALSE; //Flag for recent MPPT resets by 'handle_chronic_idle'
static const float DAYLIGHT_TEMP_LIM = 50; //Tentative sunlight exposure threshold
static const float DAYLIGHT_VOLT_LIM = 0; //Tentative sunlight exposure threshold


/**
  * @brief converts raw temperature data from power monitor to degrees Celsius
  *
  * @param raw_temp_val raw temperature data obtained from power monitor
  *
  * @retval temperature in degrees Celsius
*/
float convert_raw_to_celsius(int16_t raw_temp_val){

    float temperature_celsius = raw_temp_val * TEMP_CONVERT_FAC;

    return temperature_celsius;

}

/**
  * @brief compares detected temperature with established threshold to deduct if system is receiving adequate sun exposure,
  * indicating that charging should be occurring
  * 
  * @param None
  *
  * @retval 1, 0, or -1 in the case of TRUE, FALSE or ERROR respectively
*/
int8_t check_if_in_daylight_temp(){

    char out_message[50];
    int16_t raw_temp_val = eps_get_power_monitor_temp_func(POWER_MONITOR_ADDRESS, SECONDARY_DEVICE_ADDRESS, out_message);

    if (strcmp(out_message, "ERRORT\r\n") == 0){
        
        return ERROR;
    }

    float temperature_celsius = convert_raw_to_celsius(raw_temp_val);

    if (temperature_celsius >= DAYLIGHT_TEMP_LIM){
        return TRUE;
    } else{
        return FALSE;
    }
}

/**
  * @brief converts raw voltage data from power monitor to millivolts
  *
  * @param raw_volt_val raw voltage data obtained from power monitor
  *
  * @retval power monitor shunt voltage in millivolts
*/
float convert_raw_to_mv(int16_t raw_volt_val){

    float voltage_mv = raw_volt_val * VOLT_CONVERT_FAC;

    return voltage_mv;

}

/**
  * @brief compares detected voltage with established threshold to deduct if system is receiving adequate sun exposure
  * indicating that charging should be occurring
  *
  * @param None
  *
  * @retval 1, 0, or -1 in the case of TRUE, FALSE or ERROR respectively
*/
int8_t check_if_in_daylight_volt(){

    char out_message[50];
    int16_t raw_volt_val = eps_get_power_monitor_v_bus_val_func(POWER_MONITOR_ADDRESS, SECONDARY_DEVICE_ADDRESS, out_message);

    if (strcmp(out_message, "ERRORT\r\n") == 0){
        return ERROR;
    }

    float voltage_mv = convert_raw_to_mv(raw_volt_val);

    if (voltage_mv >= DAYLIGHT_VOLT_LIM){
        return TRUE;
    } else{
        return FALSE;
    }
}

/**
  * @brief detects fault case: chronic_idle;
  * every interval, checks if mppt is idle; prolonged consecutive idles triggers handler function (~4h)
  *
  * @param None
  *
  * @retval None
*/
void detect_chronic_idle(){

    if (pass_num <= g_const_PASS_REQ / (g_source_decay + 1) ) {
        pass_num++;

    } else {
        eps_mppt_status out = mppt_get_charge_status();

        if (out == EPS_MPPT_CHARGING_IDLE) {

            consecutive_idles = (consecutive_idles << 1) | 1;
            
            if (consecutive_idles == 0xFF) {
                handle_chronic_idle();
            }

        } else {
            consecutive_idles = 0;
            mppt_was_reset = FALSE;
        }
        pass_num = 0;
    }
}

/**
  * @brief runs helper functions, power cycles mppt if necessary
  *
  * @param None
  *
  * @retval None
*/
void handle_chronic_idle(){

    if (mppt_was_reset == FALSE){

        int8_t temp_check = check_if_in_daylight_temp();
        int8_t volt_check = check_if_in_daylight_volt();

        if (temp_check == ERROR || volt_check == ERROR){
            g_read_error = TRUE;

        } else if (temp_check == TRUE && volt_check == TRUE){
            mppt_init();
            mppt_was_reset = TRUE;
        }
    } else if (mppt_was_reset == TRUE){
        
        printf("Entering Safety Mode\n");
        printf("Fault: chronic_idle\n");
    }
}
