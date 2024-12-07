/* 
 * File:   car_black_box.h
 * Author: Jana Ranjani
 *
 * Created on November 14, 2024, 12:13 PM
 */

#ifndef CAR_BLACK_BOX_H
#define	CAR_BLACK_BOX_H

int long_press_switch(unsigned char key);
// CAPTURE THE EVENTS
void log_car_event(char event[], unsigned char speed);
// DISPLAY THE DEFAULT MENU
void display_dashboard(char event[], unsigned char speed);
// CLEARS THE CLCD
void clear_screen(void);
// FOR SUCCESSFUL LOGIN
unsigned char login(unsigned char key, unsigned char reset_flag);
// FOR MAIN MENU OPTIONS
char menu_screen(unsigned char key, unsigned char reset_flag);
// VIEWING LOGS
char view_log(unsigned char key, unsigned char reset_flag);
// CLEARING LOGS
void clear_log(unsigned char reset_flag);
// DOWMLOAD LOGS TO CUTECOM USING UART
void download_log(void);

void send_log_via_uart(void);
// RESETTING TIME
void set_time(unsigned char key, unsigned char reset_flag);
// RESETTING NEW PASSWORD
char change_password(unsigned char key, unsigned char reset_flag);

#endif	/* CAR_BLACK_BOX_H */

