/* 
 * File:   External_EEPROM.h
 * Author: Jana Ranjani
 *
 * Created on November 15, 2024, 11:59 AM
 */

#ifndef EXTERNAL_EEPROM_H
#define	EXTERNAL_EEPROM_H

#define SLAVE_WRITE_EE            0b10100000 // 0xA0
#define SLAVE_READ_EE             0b10100001 // 0xA1

#define SEC_ADDR                0x00
#define MIN_ADDR                0x01
#define HOUR_ADDR               0x02

unsigned char eeprom_at24c04_random_read(unsigned char memory_loc);
void eeprom_at24c04_byte_write(unsigned char memory_loc, unsigned char data);
void eeprom_at24c04_str_write(unsigned char memory_loc, char *data);

#endif	/* EXTERNAL_EEPROM_H */

