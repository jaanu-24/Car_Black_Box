/*
   Author        : JANA RANJANI S
   Date          : 14 / 11 / 2024
   Project Title : Car Black Box
   File Name     : Car Black Box definition
 */

#include "main.h"

unsigned char clock_reg[3];
char time[7]; // "HHMMSS"

char log[11]; // "hhmmssevsp"
char log_count = 0;

char s_time[7];
char s_speed[3];
char s_event[3];

char track_addr = -1;
unsigned char sec = 0;

extern char key_flag;

extern unsigned char return_time;
extern int press;
extern unsigned char updated_sec, updated_min, updated_hr;

char *menu[5] = {"VIEW LOG", "CLEAR LOG", "DOWNLOAD LOG", "SET TIME", "CHANGE PASSWRD"};
// 0            1             2              3             4

void clear_screen() {
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);
}

void get_time(void) {
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 

    // HH -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';

    // MM 
    time[2] = ((clock_reg[1] >> 4) & 0x03) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';

    // SS
    time[4] = ((clock_reg[2] >> 4) & 0x03) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';

    time[6] = '\0';

}

void display_time(void) {
    get_time();
    clcd_putch(time[0], LINE2(1));
    clcd_putch(time[1], LINE2(2));

    clcd_putch(':', LINE2(3));

    clcd_putch(time[2], LINE2(4));
    clcd_putch(time[3], LINE2(5));

    clcd_putch(':', LINE2(6));

    clcd_putch(time[4], LINE2(7));
    clcd_putch(time[5], LINE2(8));

}

void display_dashboard(char event[], unsigned char speed) {
    clcd_print("TIME     E  SP", LINE1(2));

    // TO DISPLAY TIME

    display_time();

    // TO DISPLAY EVENT

    clcd_print(event, LINE2(11));

    // TO DISPLAY SPEED

    clcd_putch(speed / 10 + '0', LINE2(14));
    clcd_putch(speed % 10 + '0', LINE2(15));

}

void record_log(void) {
    // EEPROM WRITE
    // WRITE LOG STRING TO MEMORY

    char addr;
    if (log_count < 10) {
        log_count++;
    }
    if (++track_addr == 10) {
        track_addr = 0;
    }

    addr = 0x05 + track_addr * 10;

    eeprom_at24c04_str_write(addr, log);

}

void log_car_event(char event[], unsigned char speed) {
    // TO STORE TIME

    get_time();
    strncpy(log, time, 6); // "hhmmss"

    // TO STORE EVENT

    strncpy(&log[6], event, 2); // "ev"    

    // TO STORE SPEED

    log[8] = speed / 10 + '0'; // sp
    log[9] = speed % 10 + '0';

    log[10] = '\0';

    record_log();

}

unsigned char login(unsigned char key, unsigned char reset_flag) {
    static char user_passwrd[4];
    char store_passwrd[4];
    static unsigned char i, attempt_left; // after i = 4, compare password
    if (reset_flag == RESET_PASSWORD) {
        key = ALL_RELEASED;
        attempt_left = 3;
        user_passwrd[0] = '\0';
        user_passwrd[1] = '\0';
        user_passwrd[2] = '\0';
        user_passwrd[3] = '\0';
        i = 0;
        return_time = 5;
    }

    if (return_time == 0) {
        // CHANGE SCREEN
        return RETURN_BACK;
    }

    if (key == ALL_RELEASED) {
        if (key_flag == SW4 && i < 4) // '1'
        {
            user_passwrd[i] = '1';
            clcd_putch('*', LINE2(4 + i));
            i++;
            return_time = 5;
            key_flag = press = 0;

        } else if (key_flag == SW5 && i < 4) // '0'
        {
            user_passwrd[i] = '0';
            clcd_putch('*', LINE2(4 + i));
            i++;
            return_time = 5;
            key_flag = press = 0;
        }
    }

    if (i == 4) // COMPARE PASSWORD
    {
        for (unsigned char j = 0; j < 4; j++) {
            store_passwrd[j] = eeprom_at24c04_random_read(j);
        }
        if (strncmp(user_passwrd, store_passwrd, 4) == 0) {
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            clcd_print("LOGIN SUCCESS", LINE1(1));
            __delay_ms(2000);
            return LOGIN_SUCCESS;

        } else {
            clear_screen();
            attempt_left--;
            if (attempt_left == 0) // IF 0
            {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("YOU ARE BLOCKED", LINE1(0));
                clcd_print("WAIT FOR", LINE2(1));
                clcd_print("SECS", LINE2(13));
                //__delay_ms(3000); // CONVERT TO 15 MINS
                // WAIT FOR 1 MIN
                sec = 60;
                while (sec) {
                    clcd_putch((sec / 10) + '0', LINE2(10));
                    clcd_putch((sec % 10) + '0', LINE2(11));
                }
                attempt_left = 3;
            } else // NOT 0
            {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("WRONG PASSWORD", LINE1(1));
                clcd_putch(attempt_left + '0', LINE2(1));
                clcd_print("ATTEMPTS LEFT", LINE2(3));
                __delay_ms(3000);
            }

            clear_screen();
            clcd_print("ENTER PASSWORD", LINE1(1));
            clcd_write(LINE2(4), INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            i = 0;
            return_time = 5;
        }
    }
}

char menu_screen(unsigned char key, unsigned char reset_flag) {
    static unsigned char menu_pos;

    if (reset_flag == RESET_MAIN_MENU) {
        clear_screen();
        menu_pos = 0;
        return_time = 5;
        key_flag = press = 0;
    }

    if (key == ALL_RELEASED && press < 13) {
        if (key_flag == SW5) {
            if (menu_pos < 4) {
                menu_pos++;
                return_time = 5;
                clear_screen();
            }
            key_flag = press = 0;
        } else if (key_flag == SW4) {
            if (menu_pos > 0) {
                menu_pos--;
                return_time = 5;
                clear_screen();
            }
            key_flag = press = 0;
        }
    }

    if (menu_pos == 4) {
        clcd_putch('*', LINE2(0));
        clcd_print(menu[menu_pos - 1], LINE1(2));
        clcd_print(menu[menu_pos], LINE2(2));
    } else {
        clcd_putch('*', LINE1(0));
        clcd_print(menu[menu_pos], LINE1(2));
        clcd_print(menu[menu_pos + 1], LINE2(2));

    }

    return menu_pos;
}

void get_saved_log(char log_pos) {
    unsigned char addr, i;

    addr = (log_pos - 1) * 10 + 5;

    for (i = 0; i < 6; i++) {
        s_time[i] = eeprom_at24c04_random_read(addr + i); // Get the log from the memory eeprom and store it in array -> HHSSMM
    }
    s_time[i] = '\0';

    s_event[0] = eeprom_at24c04_random_read(addr + 6); // Get the log from the memory eeprom and store it in array -> EV 
    s_event[1] = eeprom_at24c04_random_read(addr + 7);
    s_event[2] = '\0';

    s_speed[0] = eeprom_at24c04_random_read(addr + 8); // Get the log from the memory eeprom and store it in array -> speed 0 to 99 
    s_speed[1] = eeprom_at24c04_random_read(addr + 9);
    s_speed[2] = '\0';

}

void print_log(char log_pos) {

    clcd_putch(log_pos - 1 + '0', LINE2(0)); // In line 1 0th position display the line number of log 

    clcd_putch(s_time[0], LINE2(2)); // Hour
    clcd_putch(s_time[1], LINE2(3)); // Hour

    clcd_putch(':', LINE2(4));

    clcd_putch(s_time[2], LINE2(5)); // Minute
    clcd_putch(s_time[3], LINE2(6)); // Minute

    clcd_putch(':', LINE2(7));

    clcd_putch(s_time[4], LINE2(8)); // Second
    clcd_putch(s_time[5], LINE2(9)); // Second

    clcd_putch(s_event[0], LINE2(11)); // Event 
    clcd_putch(s_event[1], LINE2(12));

    clcd_putch(s_speed[0], LINE2(14)); // Speed
    clcd_putch(s_speed[1], LINE2(15));

}

char view_log(unsigned char key, unsigned char reset_flag) {

    static char log_pos;

    if (reset_flag == RESET_VIEW_LOG) {
        clear_screen();
        return_time = 5;

        if (log_count > 0) // If there is a log count obviously logs are avaliable to display
        {
            clcd_print("LOGS", LINE1(5)); // If the logs are avaliable then display this TITLE
            log_pos = 1;

            get_saved_log(log_pos);
            print_log(log_pos);
        }
    }

    if (log_count > 0) {
        if (key == ALL_RELEASED && press < 13) // LOng press
        {
            if (key_flag == SW4) // When SW4 is pressed log pos should increase from 0 to max count after it reaches the max value for ex: 9(max) it should roll back to zero
            {
                if (++log_pos > log_count) {
                    log_pos = 1;
                }
                get_saved_log(log_pos);
                print_log(log_pos);

                key_flag = press = 0;
                return_time = 5;
            } else if (key_flag == SW5) // When SW5 is pressed log pos should decrease from max count to 0 after it reaches the 0  it should roll back to max count
            {
                if (--log_pos < 1) {
                    log_pos = log_count;
                }
                get_saved_log(log_pos);
                print_log(log_pos);

                key_flag = press = 0;
                return_time = 5;
            }
        }
    } else /* If the log count are less than or equal to zero there is no logs available*/ {
        clcd_print("  NO  LOGS  ARE", LINE1(0));
        clcd_print("    AVAILABLE ", LINE2(0));
        __delay_ms(3000);

        return TASK_FAIL;
    }

    __delay_ms(80);

    return TASK_SUCCESS;
}

void clear_log(unsigned char reset_flag) {

    // when the clear log is selected, after the logs are cleared successfully it will back to main menu screen if there is inactivity it should back to dashboard screen
    if (reset_flag == RESET_LOG_MEMORY) {

        log_count = 0; // By the making the log count as zero means no logs available
        track_addr = -1; // Make the track address as -1

        clcd_print("LOGS ARE CLEARED", LINE1(0));
        clcd_print("  SUCCESSFULLY  ", LINE2(0));

        __delay_ms(3000);
    }
}

void display_uart(char log_pos) {
    //For ex: 0 12:00:09 ON 99
    //        p HH:MM:SS EV SP

    putchar(log_pos - 1 + '0');
    putchar(' ');

    putchar(s_time[0]); // Hour
    putchar(s_time[1]); // Hour
    putchar(':');

    putchar(s_time[2]); // Hour
    putchar(s_time[3]); // Hour
    putchar(':');

    putchar(s_time[4]); // Second
    putchar(s_time[5]); // Second
    putchar(' ');

    putchar(s_event[0]); // event
    putchar(s_event[1]); // event
    putchar(' ');

    putchar(s_speed[0]); // speed
    putchar(s_speed[1]); // speed
    putchar('\n');
}

void download_log(void) {
    char i = 1;

    if (log_count > 0) {
        while (i <= log_count) {
            get_saved_log(i);
            display_uart(i);
            i++;
        }

        clcd_print(" LOGS DISPLAYED ", LINE1(0));
        clcd_print("    IN UART    ", LINE2(0));
    } else {
        clcd_print("     NO LOGS    ", LINE1(0));
        clcd_print("  ARE AVAILABLE ", LINE2(0));
    }

    __delay_ms(3000);
}

void set_time(unsigned char key, unsigned char reset_flag) {


    static char updated_time[7];

    static int i, j, delay, blink;

    if (reset_flag == RESET_SET_TIME) {
        clear_screen();
        return_time = 5;

        delay = blink = 0;

        clcd_print("TIME <HH:MM:SS>", LINE1(0));
        get_time();

        strcpy(updated_time, time); // copy the time to the updated time

        // converts str to int 
        updated_hr = (updated_time[0] - '0') * 10 + updated_time[1] - '0'; // for ex : Hr is 14    (1 - '0') * 10  where 1 is char  (49 - '0') * 10 =  10
        //                      (4 - '0')       where it gives   (53 - '0')      =   4
        //                                                                         ------
        //                                                                           14 (int)     
        //                                                                         ------
        updated_min = (updated_time[2] - '0') * 10 + updated_time[3] - '0';
        updated_sec = (updated_time[4] - '0') * 10 + updated_time[5] - '0';

        i = 2;
        j = 10;

    }

    if (key == ALL_RELEASED && press < 13) {
        if (key_flag == SW4) {
            if (i == 2) {
                if (++updated_sec > 59) // Increase the sec upto 59 and roll back to zero
                {
                    updated_sec = 0;
                }
            } else if (i == 1) {
                if (++updated_min > 59) // Increase the min upto 59 and roll back to zero
                {
                    updated_min = 0;
                }
            } else {
                if (++updated_hr > 23) // Increase the hr upto 23 and roll back to zero
                {
                    updated_hr = 0;
                }
            }

            return_time = 5;
            key_flag = press = 0;

        } else if (key_flag == SW5) {
            if (--i < 0) {
                i = 2;
            }

            if (i == 2) {
                j = 10;
            } else if (i == 1) {
                j = 7;
            } else if (i == 0) {
                j = 4;
            }

            return_time = 5;
            key_flag = press = 0;
        }
    }

    if (++delay > 10) {
        blink = !blink;
        delay = 0;
    }

    if (!blink) {
        clcd_putch(updated_hr / 10 + '0', LINE2(4)); //Hour
        clcd_putch(updated_hr % 10 + '0', LINE2(5)); //Hour

        clcd_putch(':', LINE2(6));

        clcd_putch(updated_min / 10 + '0', LINE2(7)); //Minute
        clcd_putch(updated_min % 10 + '0', LINE2(8)); //Minute

        clcd_putch(':', LINE2(9));

        clcd_putch(updated_sec / 10 + '0', LINE2(10)); //Second
        clcd_putch(updated_sec % 10 + '0', LINE2(11)); //Second
    } else {
        clcd_putch(0xFF, LINE2(j));
        clcd_putch(0xFF, LINE2(j + 1));
    }
}

char change_password(unsigned char key, unsigned char reset_flag) {

    char saved_passwd[4];
    static char new_passwd_1[4], new_passwd_2[4];
    static int i, j;

    if (reset_flag == RESET_PASSWORD) {
        clear_screen();

        clcd_print("ENTER NEW PASSWRD", LINE1(0));
        clcd_write(LINE2(4), INST_MODE);
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
        __delay_us(100);

        i = j = 0;

        /* Enter 4 digit new password */
        new_passwd_1[0] = '\0';
        new_passwd_1[1] = '\0';
        new_passwd_1[2] = '\0';
        new_passwd_1[3] = '\0';

        /* Re enter the 4 digit new password*/
        new_passwd_2[0] = '\0';
        new_passwd_2[1] = '\0';
        new_passwd_2[2] = '\0';
        new_passwd_2[3] = '\0';

        return_time = 5;
    }

    if (key == ALL_RELEASED && press < 13) {
        if (key_flag == SW4) {
            if (i < 4) {
                new_passwd_1[i] = '1';
                clcd_putch('*', LINE2(i + 4));
                i++;
                key_flag = press = 0;
            } else {
                new_passwd_2[j] = '1';
                clcd_putch('*', LINE2(j + 4));
                j++;
            }
            return_time = 5;
            key_flag = press = 0;
        } else if (key_flag == SW5) {
            if (i < 4) {
                new_passwd_1[i] = '0';
                clcd_putch('*', LINE2(i + 4));
                i++;
                key_flag = press = 0;
            } else {
                new_passwd_2[j] = '0';
                clcd_putch('*', LINE2(j + 4));
                j++;
            }
            return_time = 5;
            key_flag = press = 0;
        }
    }

    if (i == 4) {
        i++;
        clear_screen();
        clcd_print("RENTER PASSWRD", LINE1(0));
        clcd_write(LINE2(4), INST_MODE);
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
        __delay_us(100);
    }
    else if (j == 4) {
        if (strncmp(new_passwd_1, new_passwd_2, 4) == 0) {
            clear_screen();
            for (int i = 0; i < 4; i++) {
                eeprom_at24c04_byte_write(i, new_passwd_1[i]);
            }

            clcd_print("PASSWORD CHANGED", LINE1(0));
            clcd_print("  SUCCESSFULLY  ", LINE2(0));
            __delay_ms(3000);

            return TASK_SUCCESS;

        } else {

            clcd_print("PASSOWRD DOESN'T", LINE1(0));
            clcd_print("MATCH, Try again", LINE2(0));
            __delay_ms(3000);

            clear_screen();

            clcd_print("ENTER PASSWORD", LINE1(1));
            clcd_write(LINE2(4), INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);

            /*enter new password*/
            new_passwd_1[0] = '\0';
            new_passwd_1[1] = '\0';
            new_passwd_1[2] = '\0';
            new_passwd_1[3] = '\0';

            /*re enter the password*/
            new_passwd_2[0] = '\0';
            new_passwd_2[1] = '\0';
            new_passwd_2[2] = '\0';
            new_passwd_2[3] = '\0';

            return_time = 5;

            i = j = 0;
        }
    }
    __delay_ms(81);
}
