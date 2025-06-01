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

#include "source_decay.h"
#include "chronic_idle.h"
#include "pwr_mon_read_error.h"

#define MAXIMUM_EXPECTED_CURRENT 32768 //Placeholder: unspecified on data sheet
#define MONTHS_LOG_SZ 128 //Tentative size of log of monthly rolling averages

static uint16_t pass_num = 0; //counter tracking iterations of the while loop, gets reset when 'pass_num == g_const_PASS_REQ'
static float minutes_roll_avg = 0; //rolling average of power readings over an hour
static float hours_roll_avg = 0; //rolling average of power readings over a day
static float days_roll_avg = 0; //rolling average of power readings over a month
static float months_log[MONTHS_LOG_SZ] = {0}; //log of monthly rolling averages
static uint8_t minutes_pos = 0; //counter tracking number of readings logged for minutes_roll_avg
static uint8_t hours_pos = 0; //counter tracking number of readings logged for hours_roll_avg
static uint8_t days_pos = 0; //counter tracking number of readings logged for days_roll_avg
static uint8_t months_pos = 0; //counter tracking the next available position for logging in months_log
static float baseline_avg = 0; //records month 1 average power; sets baseline for future monthly comparisons
static int32_t raw_power_val; //records raw power value provided by the power monitor
static uint8_t perform_monthly_check = FALSE; //flags when a new monthly average is ready to be compared to the baseline_avg
static const float CAP_THRESHOLD = 0.8; //tentative threshold of source capability (80%) whereby handler kicks in
uint8_t g_source_decay = 0;


/**
  * @brief provides 'CURRENT_LSB' value required to convert raw power to watts as per data sheet
  * 
  * @param None
  *
  * @retval None
*/
float CURRENT_LSB(){

    return MAXIMUM_EXPECTED_CURRENT / 32768;

}

/**
  * @brief converts raw power value provided by power monitor to watts
  * 
  * @param raw_power_val raw power value provided by power monitor
  *
  * @retval None
*/
float convert_raw_to_watts(int32_t raw_power_val){

    return 0.2 * CURRENT_LSB() * raw_power_val; //data sheet hardware-specified conversion factor

}

/**
  * @brief logs current power to the appropriate log or rolling average; data is aggregated over time to conserve memory
  * 
  * @param None
  *
  * @retval 0 or -1, indicating operation success or ERROR respectively
*/
int8_t log_current_power(){

    char out_message[50];
    raw_power_val = eps_get_power_monitor_power_func(POWER_MONITOR_ADDRESS, SECONDARY_DEVICE_ADDRESS, out_message);

    if (strcmp(out_message, "ERRORP\r\n") == 0){
        return ERROR;

    } else {
        minutes_roll_avg += convert_raw_to_watts(raw_power_val);
        minutes_pos++;

        if (minutes_pos == 60){
            minutes_pos = 0;

            hours_roll_avg += minutes_roll_avg / 60;
            minutes_roll_avg = 0;
            hours_pos++;

            if (hours_pos == 24){
                hours_pos = 0;

                days_roll_avg += hours_roll_avg / 24;
                hours_roll_avg = 0;
                days_pos++;

                if (days_pos == 30){
                    days_pos = 0;

                    months_log[months_pos] = days_roll_avg / 30;
                    days_roll_avg = 0;

                    if (baseline_avg == 0){
                        baseline_avg = months_log[months_pos];

                    } else {
                        perform_monthly_check = TRUE;

                    }
                    months_pos++;

                    if (months_pos == MONTHS_LOG_SZ){
                        months_pos = 0;
                    }
                }
            }
        }
        return 0;
    }
}

/**
  * @brief detects fault case: source_decay; calls log_current_power() intermittently, calling 
  * handle_source_decay() when the first monthly check fails
  * 
  * @param None
  *
  * @retval None
*/
void detect_source_decay(){

    if (g_source_decay != 1){
        
        if (pass_num < g_const_PASS_REQ){
            
            pass_num++;

        } else {

            if (log_current_power() == ERROR){

                g_read_error = TRUE;

            } else {

                if (perform_monthly_check == TRUE){
                
                    if (months_log[months_pos] < baseline_avg * CAP_THRESHOLD){

                        handle_source_decay();

                    }
                    perform_monthly_check = FALSE;

                }
            }
            pass_num = 0;
        }
    }
}

/**
  * @brief handles fault case: source_decay; sets global variable g_source_decay to 1 which affects 'chronic_idle.c'
  * 
  * @param None
  *
  * @retval None
*/
void handle_source_decay(){

    g_source_decay = 1;
    printf("Fault: source_decay\n");
}
