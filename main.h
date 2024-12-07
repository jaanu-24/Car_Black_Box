/* 
 * File:   main.h
 * Author: Jana Ranjani
 *
 * Created on November 13, 2024, 12:55 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include "adc.h" // ADC
#include "clcd.h" // CLCD
#include "ds1307.h" // RTC
#include "i2c.h" // I2C
#include "digital_keypad.h" // DKP
#include "car_black_box.h" // DECLARATIONS
#include <string.h>  // STRINGS
#include "External_EEPROM.h"
#include "timers.h"
#include "uart.h"
#include <stdlib.h>

#define RESET_NOTHING                                 0x00
#define DASH_BOARD_SCREEN                             0x01
#define LOGIN_SCREEN                                  0x02
#define MAIN_MENU_SCREEN                              0x03

#define VIEW_LOG_SCREEN                               0x04
#define CLEAR_LOG_SCREEN                              0x05
#define DOWNLOAD_LOG_SCREEN                           0x06
#define SET_TIME_SCREEN                               0x07
#define CHANGE_PASSWRD_SCREEN                         0x08

#define TASK_FAIL                                     0x09
#define TASK_SUCCESS                                  0x10

#define RESET_PASSWORD                                0x11

#define RETURN_BACK                                   0x33

#define LOGIN_SUCCESS                                 0x44

#define RESET_VIEW_LOG                                0x66
#define RESET_LOG_MEMORY                              0x77
#define RESET_SET_TIME                                0x88
#define RESET_MAIN_MENU                               0x99


#endif	/* MAIN_H */

