/*
 * spi.h
 *
 * Created: 2/17/2014 5:34:04 PM
 *  Author: Nabeel
 */ 


#ifndef SPI_H_
#define SPI_H_


/************************************************************************/
/*                                SPI                                   */
/************************************************************************/

#define DDR_SS_PORT     DDRB
#define SS_PORT         PORTB
#define SS_PIN          PB4
#define DDR_SPI_PORT    DDRB
#define SPI_PORT        PORTB
#define MISO_PIN        PB6
#define MOSI_PIN        PB5
#define SCK_PIN         PB7

#define IINCHIP_CSoff()   SS_PORT &= ~(1<<SS_PIN)
#define IINCHIP_CSon()    SS_PORT |= (1<<SS_PIN)

#define IINCHIP_SpiRecvData()     SPDR
#define IINCHIP_SpiSendData(a)    SPDR = a;     while(!(SPSR & (1<<SPIF)))

void    IINCHIP_SpiInit_real(void);
// void    IINCHIP_SpiRecvData(void);
// void    IINCHIP_SpiSendData(uint8_t x);

// void    IINCHIP_WRITE(uint16_t addr, char data);
// uint8_t IINCHIP_READ(uint16_t addr);


/************************************************************************/
/*                             Interrupts                               */
/************************************************************************/

#define W_INT_PORT          PORTD
#define W_INT_DDR_PORT      DDRD
#define W_INT_PIN           PD3
#define W_INT               INT1
#define W_INT_FLG           INTF1
#define W_INT_VECT          INT1_vect

void IINCHIP_INTInit_real(void);


/************************************************************************/
/*                         Global variables                             */
/************************************************************************/

// Debug
char lcd_msg_var[16];
uint8_t test;

// Flag for receive reply interrupt
uint8_t flag_var;


#endif /* SPI_H_ */