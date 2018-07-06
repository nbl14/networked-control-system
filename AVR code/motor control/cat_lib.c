/*
 * cat_lib.c
 *
 * Created: 4/6/2014 3:01:32 PM
 *  Author: Nabeel Ahmad
 */


#include <avr/io.h>
#define F_CPU 11059200
#include <util/delay.h>
// #include <stdio.h>
// #include <string.h>
#include <avr/interrupt.h>
// #include <avr/sleep.h>
#include "w5100lib/types.h"
#include "w5100lib/spi.h"
#include "w5100lib/w5100.h"
#include "w5100lib/socket.h"
#include "w5100lib/dhcp.h"

void setupADC_8(void)
{
    // Vref=2.56. Left adjusted result.
    ADMUX |= (1<<REFS1)|(1<<REFS0)|(1<<ADLAR);
    //_delay_us(5);
    // ADC enable. f = Xtal/64.
    ADCSRA |= (1<<ADEN)/*|(1<<ADPS0)*/|(1<<ADPS1)|(1<<ADPS2);
    //_delay_us(5);
}

uint8_t getADCdata_8(uint8_t channel)
{   
    ADMUX  |= (channel & 0b00000111);    // Selecting channel
    //_delay_us(10);
    
    ADCSRA |= (1<<ADSC);
    
    /*
    // Going to sleep to reduce noise
    set_sleep_mode(SLEEP_MODE_ADC);
    sleep_mode();
    */
    
    while(ADCSRA & (1<<ADSC));
    
    uint8_t var = ADCH;
    ADMUX &= ~(channel & 0b00000111);    // De-selecting channel
    return var;
}

void setupADC_10(void)
{
    // Vref=2.56.
    ADMUX |= (1<<REFS1)|(1<<REFS0);
    //_delay_us(5);
    // ADC enable. f = Xtal/64.
    ADCSRA |= (1<<ADEN)/*|(1<<ADPS0)*/|(1<<ADPS1)|(1<<ADPS2);
    //_delay_us(5);
}

uint16_t getADCdata_10(uint8_t channel)
{
    ADMUX  |= (channel & 0b00000111);    // Selecting channel
    //_delay_us(10);
    
    ADCSRA |= (1<<ADSC);
    while(ADCSRA & (1<<ADSC));
    
    // For 16 bit result (Remove left adjustment in setup)
    uint16_t result = 0;
    result = (uint16_t)ADCL;
    result |= (((uint16_t)ADCH)<<8);
    
    ADMUX &= ~(channel & 0b00000111);    // De-selecting channel
    return result;
}

void setupCounterT0(void)
{
    PORTB  |= (1<<PB0);             // Clock pin pull-up enable
    //TCCR0  |= (1<<CS02)|(1<<CS01);  // Clock: T0 pin, falling edge
}

void setupCounterT0_nonstop(void)
{
    PORTB  |= (1<<PB0);             // Clock pin pull-up enable
    TCCR0  |= (1<<CS02)|(1<<CS01);  // Clock: T0 pin, falling edge
}

uint8_t count_for_100ms_T0_nonstop(void)
{
    TCNT0 = 0;
    //TCCR0  |= (1<<CS02)|(1<<CS01);
    _delay_ms(100);
    uint8_t var = TCNT0;
    //TCCR0  = 0;
    return var;
}

uint8_t count_for_100ms_T0(void)
{
    TCNT0 = 0;
    TCCR0  |= (1<<CS02)|(1<<CS01);
    _delay_ms(100);
    uint8_t var = TCNT0;
    TCCR0  = 0;
    return var;
}

uint8_t count_for_95ms_T0_nonstop(void)
{
    TCNT0 = 0;
    _delay_ms(95);
    return TCNT0;
}

uint8_t count_for_var_T0_nonstop(void)
{
    TCNT0 = 0;
    _delay_ms(2000);
    return TCNT0;
}

uint8_t count_for_50ms_T0_nonstop(void)
{
    TCNT0 = 0;
    TCCR0  |= (1<<CS02)|(1<<CS01);
    _delay_ms(50);
    uint8_t var = TCNT0;
    TCCR0  = 0;
    return var;
}

uint8_t count_for_50ms_T0(void)
{
    TCNT0 = 0;
    TCCR0  |= (1<<CS02)|(1<<CS01);
    _delay_ms(50);
    uint8_t var = TCNT0;
    TCCR0  = 0;
    return var;
}

void setupCounterT0_asynch()
{
    setupCounterT0_nonstop();
    TCNT1 = 0;
    
    // TCCR1B
    // Bit 2:0 – CS12:0: Clock Select
    // clk I/O /1024
    TCCR1B |= (1<<CS12) | (1<<CS10);
    
    OCR1A = 10800;  // For a delay of 100 ms
    
    // Bit 4 – OCIE1A: Timer/Counter1, Output Compare A Match Interrupt Enable
    TIMSK |= (1<<OCIE1A);
}

void setupPWM_T1(void)
{
    DDRD |= (1<<PD5) | (1<<PD4);
    
    // TCCR1A
    // Bit 1:0 – WGM11:0: Waveform Generation Mode
    // PWM, Phase Correct, 8-bit 
    TCCR1A |= (1<<WGM10);
    
    // TCCR1B
    // Bit 2:0 – CS12:0: Clock Select
    // clk I/O /1024 (From prescaler)
    // Around 21k Hz at 11 MHz crystal
    TCCR1B |= (1<<CS10) ;
}

void startPWM_T1A_LOW1_Q10(void)
{
    // TCCR1A
    // Bit 7:6 – COM1A1:0: Compare Output Mode for Channel A
    // Compare Output Mode, Phase Correct PWM
    TCCR1A |= (1<<COM1A1);
}

void stopPWM_T1A_LOW1_Q10(void)
{
    // TCCR1A
    // Bit 7:6 – COM1A1:0: Compare Output Mode for Channel A
    // Compare Output Mode, Phase Correct PWM
    TCCR1A &= ~(1<<COM1A1);
}

void startPWM_T1B_LOW2_Q11(void)
{
    // TCCR1A
    // Bit 7:6 – COM1A1:0: Compare Output Mode for Channel A
    // Compare Output Mode, Phase Correct PWM
    TCCR1A |= (1<<COM1B1);
}

void stopPWM_T1B_LOW2_Q11(void)
{
    // TCCR1A
    // Bit 7:6 – COM1A1:0: Compare Output Mode for Channel A
    // Compare Output Mode, Phase Correct PWM
    TCCR1A &= ~(1<<COM1B1);
}

void setupPWM_T2(void)
{
    DDRD |= (1<<PD7);
    
    // TCCR2
    // Bit 3, 6 – WGM21:0: Waveform Generation Mode
    // PWM, Phase Correct
    /*TCCR2 |= (1<<WGM20);*/
    // Fast PWM
    TCCR2 |= (1<<WGM21) | (1<<WGM20);
    // Bit 5:4 – COM21:0: Compare Match Output Mode
    TCCR2 |= (1<<COM21);
    // Bit 2:0 – CS22:0: Clock Select
    // clk I/O / 256 (From prescaler)
    //x 84.7 Hz at 11.0592 MHz crystal
    
    TCCR2 |= (1<<CS21) | (1<<CS20);
}