/*
 * lcd_farhan.h
 *
 * Created: 2/18/2014 7:36:26 PM
 *  Author: Nabeel
 */ 


#ifndef LCD_FARHAN_H_
#define LCD_FARHAN_H_


#include <avr/io.h>
#define F_CPU 11059200
#include <util/delay.h>

// #define regs PORTD.6
// #define en PORTD.7

#define lcd PORTC

void wrLCDnibble(unsigned char x);
void wrLCDcmd(unsigned char x);
void wrLCDchr(unsigned char x);
void LCD_msg(unsigned char *p);
void Init_LCD(void);
void UpdateDigits(unsigned int Number);
void lcd_msg_delayed(char *message);


#endif /* LCD_FARHAN_H_ */