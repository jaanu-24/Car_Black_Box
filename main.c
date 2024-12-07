/*
 * File:   main.c
 * Author: Jana Ranjani
 *
 * Created on November 13, 2024, 12:56 PM
 */


#include <xc.h>
#include "main.h"

#pragma config WDTE = OFF

char previous_control_flag = RESET_NOTHING;
int press = 0;
extern char time[7];
unsigned char updated_sec, updated_min, updated_hr;
char key_flag;
unsigned char return_time;

static void init_config(void) {

    // I2C
    init_i2c(100000);
    // RTC
    init_ds1307();
    // CLCD
    init_clcd();
    // DKP
    init_digital_keypad();
    // ADC
    init_adc();
    // TIMER 2 ON
    init_timer2();
    // UART
    init_uart(9600);
    // GLOBAL INTERRUPT ENABLE
    GIE = 1;

    PEIE = 1;

    puts("LOGS:\n\r");

}

void main(void) {
    // VARIABLE DECLARATION
    unsigned char control_flag = DASH_BOARD_SCREEN;
    unsigned char reset_flag;
    unsigned char speed = 0, key; // character because the speed value is small 
    char event[3] = "ON";
    char menu_pos, return_val;
    char *gear[] = {"GR", "GN", "G1", "G2", "G3", "G4"};
    //char *menu_event[] = {"VL", "CL", "DL", "ST", "CP"};
    unsigned char gr = 0;

    init_config();

    // LOG INIIIAL CAR EVENTS
    log_car_event(event, speed);
    eeprom_at24c04_str_write(0x00, "1010");

    while (1) {
        // READ SPEED VIA ADC
        speed = read_adc() / 10.25; // 0 to 1023 to 0 to 99

        // READ INPUT THROUGH SWITCHES
        key = read_digital_keypad(LEVEL);
        if ((key != ALL_RELEASED) && (key != SW6)) {
            if (previous_control_flag != MAIN_MENU_SCREEN) {
                press++;
                key_flag = key;
            }
        } else if (key == ALL_RELEASED) {
            press = previous_control_flag = RESET_NOTHING;
        }

        if (key == ALL_RELEASED) {
            if (control_flag == DASH_BOARD_SCREEN) {

                if (key_flag == SW1) {
                    strcpy(event, "C0");
                    log_car_event(event, speed);
                    key_flag = press = 0;
                } else if (key_flag == SW2 && gr <= 5) { // GEAR UP
                    strcpy(event, gear[gr]);
                    gr++;
                    log_car_event(event, speed);
                    key_flag = press = 0;
                } else if (key_flag == SW3 && gr > 0) { // GEAR DOWN
                    gr--;
                    strcpy(event, gear[gr]);
                    log_car_event(event, speed);
                    key_flag = press = 0;
                } else if ((key_flag == SW4 || key_flag == SW5) && control_flag == DASH_BOARD_SCREEN) { // LOGIN TRANSITION
                    control_flag = LOGIN_SCREEN;
                    reset_flag = RESET_PASSWORD;
                    clear_screen();
                    clcd_print("ENTER PASSWORD", LINE1(1));
                    clcd_write(LINE2(4), INST_MODE);
                    clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
                    __delay_us(100);
                    TMR2ON = 1;
                    key_flag = press = 0;
                }
            }
        }

        if (press > 10) {
            if (key_flag == SW4) {

                if (control_flag == MAIN_MENU_SCREEN) {

                    previous_control_flag = MAIN_MENU_SCREEN;

                    switch (menu_pos) {
                        case 0:
                            log_car_event("VL", speed);
                            control_flag = VIEW_LOG_SCREEN;
                            reset_flag = RESET_VIEW_LOG;
                            break;
                        case 1:
                            log_car_event("CL", speed);
                            control_flag = CLEAR_LOG_SCREEN;
                            reset_flag = RESET_LOG_MEMORY;
                            break;
                        case 2:
                            log_car_event("DL", speed);
                            control_flag = DOWNLOAD_LOG_SCREEN;
                            break;
                        case 3:
                            log_car_event("ST", speed);
                            control_flag = SET_TIME_SCREEN;
                            reset_flag = RESET_SET_TIME;
                            break;
                        case 4:
                            log_car_event("CP", speed);
                            control_flag = CHANGE_PASSWRD_SCREEN;
                            reset_flag = RESET_PASSWORD;
                            break;
                    }
                } else if (control_flag == SET_TIME_SCREEN) {
                    control_flag = MAIN_MENU_SCREEN;
                    reset_flag = RESET_MAIN_MENU;
                    write_ds1307(SEC_ADDR, (updated_sec / 10 << 4) | (updated_sec % 10));
                    write_ds1307(MIN_ADDR, (updated_min / 10 << 4) | (updated_min % 10));
                    write_ds1307(HOUR_ADDR, (updated_hr / 10 << 4) | (updated_hr % 10));
                } else if (control_flag == VIEW_LOG_SCREEN || control_flag == CHANGE_PASSWRD_SCREEN) // When the Sw4 is long pressed if the present screen is either in view log or change Pw screen it should back to the main menu screen
                {
                    control_flag = MAIN_MENU_SCREEN;
                    reset_flag = RESET_MAIN_MENU;
                    clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                    __delay_us(100);
                }

                key_flag = press = 0;
            } else if (key_flag == SW5) {
                if (control_flag == MAIN_MENU_SCREEN || control_flag == VIEW_LOG_SCREEN || control_flag == SET_TIME_SCREEN || control_flag == CHANGE_PASSWRD_SCREEN) // Long press off SW5 will roll back to dashboard screen
                {
                    clear_screen();
                    previous_control_flag = MAIN_MENU_SCREEN;
                    control_flag = DASH_BOARD_SCREEN;
                    key_flag = press = 0;
                    clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                    __delay_us(100);
                    TMR2ON = 0;
                }
            }
        }

        if (previous_control_flag == MAIN_MENU_SCREEN) {
            key_flag = press = 0;
        }

        switch (control_flag) {
            case DASH_BOARD_SCREEN:
                display_dashboard(event, speed);
                break;
            case LOGIN_SCREEN:
                switch (login(key, reset_flag)) {
                    case RETURN_BACK: // RETURN TO DASHBOARD
                        control_flag = DASH_BOARD_SCREEN;
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        TMR2ON = 0;
                        break;
                    case LOGIN_SUCCESS: // TO MAIN MENU
                        control_flag = MAIN_MENU_SCREEN;
                        reset_flag = RESET_MAIN_MENU;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        TMR2ON = 0;
                        continue;
                }
                break;
            case MAIN_MENU_SCREEN:
                menu_pos = menu_screen(key, reset_flag);
                if (return_time == 0) {
                    clear_screen();
                    control_flag = DASH_BOARD_SCREEN;
                    TMR2ON = 0;
                }
                break;

            case VIEW_LOG_SCREEN: // HANDLE VIEW LOG
                return_val = view_log(key, reset_flag);
                if (return_val == TASK_FAIL) {
                    control_flag = MAIN_MENU_SCREEN;
                    reset_flag = RESET_MAIN_MENU;
                    continue;
                } else if (return_time == 0) {
                    clear_screen();
                    control_flag = DASH_BOARD_SCREEN;
                    TMR2ON = 0;
                }
                break;

            case CLEAR_LOG_SCREEN: // HANDLE CLEAR LOG
                clear_log(reset_flag);
                control_flag = MAIN_MENU_SCREEN;
                reset_flag = RESET_MAIN_MENU;
                continue;

                break;

            case DOWNLOAD_LOG_SCREEN: // HANDLE DOWNLOAD LOG
                download_log();
                control_flag = MAIN_MENU_SCREEN;
                reset_flag = RESET_MAIN_MENU;
                continue;

                break;

            case SET_TIME_SCREEN:
                set_time(key, reset_flag);
                if (return_time == 0) {
                    clear_screen();
                    control_flag = DASH_BOARD_SCREEN;
                    TMR2ON = 0;
                }

                break;

            case CHANGE_PASSWRD_SCREEN:
                return_val = change_password(key, reset_flag);
                if (return_val == TASK_SUCCESS) {
                    control_flag = MAIN_MENU_SCREEN;
                    reset_flag = RESET_MAIN_MENU;
                    clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                    __delay_us(100);
                    continue;

                } else if (return_time == 0) {
                    clear_screen();
                    control_flag = DASH_BOARD_SCREEN;
                    clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                    __delay_us(100);
                    TMR2ON = 0;
                }

                break;

        }

        reset_flag = RESET_NOTHING;
    }
    return;
}
