/*
 * Date Created: 07/09/24
 * Last Modified: 14/09/24
 *
 * Source file for EPS fault detection: pwr_mon_read_error
 * Used in fault detection firmware for a CubeSat Electrical Power System (EPS).
 * Implements polling process to gauge Power Monitor responsiveness.
 *
 * Author(s): Winston Fournier
 */

#include "pwr_mon_read_error.h"
#include "chronic_idle.h"
#include "load_switches.h"
#include <string.h>

static uint64_t pass_num = 0; //Main loop iteration counter periodically prompting device checks
static uint32_t delay_counter = 0; //Delay counter following a failure to read device
static uint8_t last_test_failed = FALSE; //Flag result for the last daily pwr_mon_read_error check
static const uint32_t read_error_delay = g_const_PASS_REQ*60; //Software loop based delay equivalent to 1 hour
static const uint16_t read_error_pass_constant = 60*24; //Minutes per day
uint8_t g_read_error = 0; //Flag for recent failure to read device

/**
  * @brief attempts to check power monitor's detected temperature
  *
  * @param None
  *
  * @retval 0 or -1, reflecting whether or not the operation has succeeded
*/
int8_t temp_check(){
    
    char out_message[50];
    eps_get_power_monitor_temp_func(POWER_MONITOR_ADDRESS, SECONDARY_DEVICE_ADDRESS, out_message);

    if (strcmp(out_message, "ERRORT\r\n") == 0){
        
        return ERROR;
    }

    return 0;
}

/**
  * @brief attempts to check power monitor's detected voltage
  *
  * @param None
  *
  * @retval 0 or -1, reflecting whether or not the operation has succeeded
*/
int8_t volt_check(){

    char out_message[50];
    eps_get_power_monitor_v_bus_val_func(POWER_MONITOR_ADDRESS, SECONDARY_DEVICE_ADDRESS, out_message);

    if (strcmp(out_message, "ERRORV\r\n") == 0){
        
        return ERROR;
    }

    return 0;
}

/**
  * @brief attempts to check power monitor's detected current
  *
  * @param None
  *
  * @retval 0 or -1, reflecting whether or not the operation has succeeded
*/
int8_t current_check(){

    char out_message[50];
    eps_get_power_monitor_current_func(POWER_MONITOR_ADDRESS, SECONDARY_DEVICE_ADDRESS, out_message);

    if (strcmp(out_message, "ERRORC\r\n") == 0){
        
        return ERROR;
    }

    return 0;
}

/**
  * @brief attempts to check power monitor's detected power
  *
  * @param None
  *
  * @retval 0 or -1, reflecting whether or not the operation has succeeded
*/
int8_t power_check(){

    char out_message[50];
    eps_get_power_monitor_power_func(POWER_MONITOR_ADDRESS, SECONDARY_DEVICE_ADDRESS, out_message);

    if (strcmp(out_message, "ERRORP\r\n") == 0){
        
        return ERROR;
    }

    return 0;
}

/**
  * @brief if power monitor has previously failed to check a register, attemps to check registers again after a delay
  *
  * @param None
  *
  * @retval 0 or -1, reflecting whether or not the operation has succeeded
*/
int8_t follow_up_read(){

    if (g_read_error == 1){
        
        if (delay_counter >= read_error_delay){

            delay_counter = 0;
            g_read_error = 0;

            if (temp_check() == ERROR || volt_check() == ERROR || current_check() == ERROR || power_check() == ERROR){

                return ERROR;
            }

        } else{

            delay_counter++;
        }   
    }

    return 0;
}

/**
  * @brief checks daily if power monitor registers are operational
  *
  * @param None
  *
  * @retval 0 or -1, reflecting whether or not the operation has succeeded
*/
int8_t daily_read(){

    if (pass_num >= g_const_PASS_REQ * read_error_pass_constant){
        
        pass_num = 0;

        if (temp_check() == ERROR || volt_check() == ERROR || current_check() == ERROR || power_check() == ERROR){
    
            if (last_test_failed == TRUE){

                last_test_failed = FALSE;
                return ERROR;

            } else {

                last_test_failed = TRUE;
            }
        } else {
            
            last_test_failed = FALSE;
        }

    } else {

        pass_num++;
    }
    return 0;
}

/**
  * @brief detects pwr_mon_read_error, running appropriate helper functions
  *
  * @param None
  *
  * @retval None
*/
void detect_pwr_mon_read_error(){

    if (follow_up_read() == ERROR || daily_read() == ERROR){

        handle_pwr_mon_read_error();
    }
     
}

/**
  * @brief handles pwr_mon_read_error, entering safety mode
  *
  * @param None
  *
  * @retval None
*/
void handle_pwr_mon_read_error(){
    
    printf("Entering Safety Mode\n");
    printf("Fault: pwr_mon_read_error\n");
}
