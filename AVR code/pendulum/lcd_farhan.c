/*
 * lcd_farhan.c
 *
 * Created: 2/19/2014 9:49:22 AM
 *  Author: Nabeel
 */ 

#include "lcd_farhan.h"

void wrLCDnibble(unsigned char x)
{
    unsigned char y=0;
    y = lcd & 0x0F;
    x &= 0xF0;
    lcd = x | y;
    PORTD |= 1<<PD7; //en=1;
    _delay_us(15);
    PORTD &= ~(1<<PD7); //en=0;
    _delay_us(15);
}

void wrLCDcmd(unsigned char x)
{
    _delay_ms(1);
    PORTD &= ~(1<<PD6); //regs=0;
    wrLCDnibble(x);
    x = x << 4;
    wrLCDnibble(x);
}

void wrLCDchr(unsigned char x)
{
    _delay_ms(2);
    PORTD |= (1<<PD6); //regs=1;
    wrLCDnibble(x);
    x = x << 4;
    wrLCDnibble(x);
}

void LCD_msg(unsigned char *p)
{
    while(*p!=0)
    {
        wrLCDchr(*p);
        p++;
    }
}

void Init_LCD(void)
{
    lcd=0x0F;
    DDRC=0xF0;
    DDRD|=1<<PD6;
    DDRD|=1<<PD7;
    PORTD &= ~(1<<PD6); //regs=0;
    PORTD &= ~(1<<PD7); //en=0;
    _delay_ms(50);
    wrLCDnibble(0x30);
    _delay_ms(50);
    wrLCDnibble(0x30);
    _delay_ms(50);
    wrLCDnibble(0x30);
    _delay_ms(50);
    wrLCDnibble(0x20);
    _delay_ms(50);
    wrLCDcmd(0x28);
    _delay_ms(50);
    wrLCDcmd(0x01);
    _delay_ms(50);
    wrLCDcmd(0x0C);
    _delay_ms(50);
    wrLCDcmd(0x06);
    _delay_ms(50);
}

void UpdateDigits(unsigned int Number)
{
    unsigned char dig1, dig2, dig3, dig4, dig5;
    dig1	=(Number%10)+0x30;
    Number/=10;
    dig2	=(Number%10)+0x30;
    Number/=10;
    dig3	=(Number%10)+0x30;
    Number/=10;
    dig4	=(Number%10)+0x30;
    Number/=10;
    dig5	=(Number%10)+0x30;
    UDR=dig5;
    wrLCDchr(dig5);
    UDR=dig4;
    wrLCDchr(dig4);
    UDR=dig3;
    wrLCDchr(dig3);
    UDR=dig2;
    wrLCDchr(dig2);
    UDR=dig1;
    wrLCDchr(dig1);
}

void lcd_msg_delayed(char *message)
{
    Init_LCD();
    // wrLCDcmd(0x80);
    // _delay_ms(2);       // added
    LCD_msg((unsigned char *)message);
    _delay_ms(400);
}