/*
 * spi.c
 *
 * Created: 2/17/2014 5:33:55 PM
 *  Author: Nabeel
 */ 


#include <avr/io.h>
#define F_CPU 11059200
#include <util/delay.h>
// #include <avr/interrupt.h>
#include "spi.h"

void IINCHIP_SpiInit_real(void)
{
    DDR_SS_PORT |= (1<<SS_PIN);                        // SS: Slave Select
    DDR_SPI_PORT |= (1<<MOSI_PIN) | (1<<SCK_PIN);      // MOSI and SCK
    
    SPI_PORT |= (1<<MISO_PIN);                         // Enabling pull-up register
    SS_PORT |= (1<<SS_PIN);                            // Disabling slave
    
    SPCR |= (1<<SPE) | (1<<MSTR) | (1<<SPR0);          // Enable, Master. Xtal/16. MSB first. Mode 0.
}

/*
void IINCHIP_SpiSendData(uint8_t x)
{
    SPDR = x;
    while(!(SPSR & (1<<SPIF)));
    //_delay_us(3);
}*/


void IINCHIP_INTInit_real(void)
{
    // SREG |= (1<<I);
    // MCUCR, MCUCSR can be changed if needed
    
    // Enable specific interrupts
    // IINCHIP_WRITE(IMR, IR_CONFLICT | IR_UNREACH | IR_SOCK(0))
}

/*
void IINCHIP_WRITE(uint16_t addr, char data)
{
    uint8_t addr0 = (uint8_t)(addr&0x0F);        // Divide address into two bytes
    uint8_t addr1 = (uint8_t)((addr&0xF0)>>8);
    
    SS_PORT &= ~(1<<SS_PIN);    // Select slave
    
    SPDR = 0xF0;         // 'Write' operation
    while(!(SPSR & (1<<SPIF)));
    
    SPDR = addr1;        // Send address
    while(!(SPSR & (1<<SPIF)));
    SPDR = addr0;
    while(!(SPSR & (1<<SPIF)));
    
    SPDR = data;        // Send data
    while(!(SPSR & (1<<SPIF)));
    
    SS_PORT |= (1<<SS_PIN);
}

uint8_t IINCHIP_READ(uint16_t addr)
{
    uint8_t addr0 = (uint8_t)(addr&0x0F);        // Divide address into two bytes
    uint8_t addr1 = (uint8_t)((addr&0xF0)>>8);
    
    SS_PORT &= ~(1<<SS_PIN);      // Select slave
    
    SPDR = 0x0F;            // 'Read' operation
    while(!(SPSR & (1<<SPIF)));
    
    SPDR = addr1;        // Send address
    while(!(SPSR & (1<<SPIF)));
    SPDR = addr0;
    while(!(SPSR & (1<<SPIF)));
    
    // _delay_us(1);
    SPDR = 0xFF;        // Read data
    while(!(SPSR & (1<<SPIF)));
    SS_PORT |= (1<<SS_PIN);
    return SPDR;
}
*/