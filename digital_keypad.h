/* 
 * File:   digital_keypad.h
 * Author: Adil
 *
 * Created on 15 April, 2020, 8:22 PM
 */

#ifndef DIGITAL_KEYPAD_H
#define	DIGITAL_KEYPAD_H

#define LEVEL_DETECTION         0
#define LEVEL                   0

#define STATE_DETECTION         1
#define STATE                   1

#define KEYPAD_PORT             PORTB
#define KEYPAD_PORT_DDR         TRISB

#define INPUT_LINES             0x3F  // 0011 1111

#define SW1                     0x3E  // 0011 1110
#define SW2                     0x3D  // 0011 1101
#define SW3                     0x3B  // 0011 1011
#define SW4                     0x37  // 0011 0111
#define SW5                     0x2F  // 0010 1111
#define SW6                     0x1F  // 0001 1111

#define ALL_RELEASED            INPUT_LINES

unsigned char read_digital_keypad(unsigned char mode);
void init_digital_keypad(void);

#endif	/* DIGITAL_KEYPAD_H */

