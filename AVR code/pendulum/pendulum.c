/*
 * motor responce.c
 *
 * Created: 3/28/2014 3:29:43 PM
 *  Author: Nabeel
 */ 


#include <avr/io.h>
#define F_CPU 11059200
#include <util/delay.h>
// #include <stdio.h>
// #include <string.h>
// #include <avr/interrupt.h>
// #include <avr/sleep.h>
#include "w5100lib/types.h"
#include "w5100lib/spi.h"
#include "w5100lib/w5100.h"
#include "w5100lib/socket.h"
#include "w5100lib/dhcp.h"
#include "cat_lib.h"

// MATALB port definitions
#define MATLAB_CURRENT_PORT         12345
#define MATLAB_SPEED_SETPOINT       12345
#define MATLAB_VOLTAGE_PORT         12346
#define MATLAB_SPEED_PORT           12347
#define MATLAB_POSITION_PORT        12348
#define MATLAB_PENDULUM_PORT        12349
#define MATLAB_POSITION_SETPOINT    12350
#define MATLAB_PENDULUM_SETPOINT    12351


int main(void)
{
    _delay_ms(1000);
    IINCHIP_SpiInit_real();             // Initialize SPI
    sysinit(0b00000011, 0b00000011);    // Set buffer sizes
    
    uint8_t peer_ip[] = {192, 168, 0, 5};
    uint8_t broadcast_ip[] = {255, 255, 255, 255};
    // uint8_t peer_ip2[] = {192, 168, 0, 6};
    int16_t duty_cycle;
    uint8_t sender_ip[4];
    uint16_t sender_port;
    uint16_t var = 0;
    
    // Static IP
    uint8_t ip_var[] = {192, 168, 0, 1};
    //uint8_t ip_var[] = {172, 16, 64, 1};
    setGAR(ip_var);
    ip_var[0] = 255;
    ip_var[1] = 255;
    ip_var[2] = 255;
    ip_var[3] = 0;
    setSUBR(ip_var);
    uint8_t mac[] = {0x8A, 0x26, 0xE0, 0x1A, 0xDD, 0x84};
    setSHAR(mac);
    ip_var[0] = 192;
    ip_var[1] = 168;
    ip_var[2] = 0;
    ip_var[3] = 11;
    /*
    ip_var[0] = 172;
    ip_var[1] = 16;
    ip_var[2] = 64;
    ip_var[3] = 5;
    */
    setSIPR(ip_var);
    
    socket(0, Sn_MR_UDP, 43210, 0);     // Initialize sockets
    sendto(0, (uint8_t *)"hello\0", 6, broadcast_ip, 54321);
    //sendto(0, (uint8_t *)"hello\0", 6, broadcast_ip, 54321);
    
    int16_t prev_duty = 0;
    #define CW 0
    #define CCW 1
    #define STOPPED 2
    #define OC1A PD5
    #define OC1B PD4
    
    #define LOW1_Q10      OC1A
    #define LOW2_Q11      OC1B
    
    #define LOW1_Q10_PORT  PORTD
    #define LOW2_Q11_PORT  PORTD
    #define LOW1_Q10_DDR   DDRD
    #define LOW2_Q11_DDR   DDRD
    
    #define HIGH1_Q7      PA5
    #define HIGH2_Q9      PA1
    
    #define HIGH1_Q7_PORT PORTA
    #define HIGH2_Q9_PORT PORTA
    #define HIGH1_Q7_DDR  DDRA
    #define HIGH2_Q9_DDR  DDRA
    
    // DDR setup
    LOW1_Q10_DDR |= (1<<LOW1_Q10);
    LOW2_Q11_DDR |= (1<<LOW2_Q11);
    HIGH1_Q7_DDR |= (1<<HIGH1_Q7);
    HIGH1_Q7_DDR |= (1<<HIGH2_Q9);
    
    // Float
    HIGH1_Q7_PORT |= (1<<HIGH1_Q7);
    HIGH2_Q9_PORT |= (1<<HIGH2_Q9);
    LOW1_Q10_PORT &= ~(1<<LOW1_Q10);
    LOW2_Q11_PORT &= ~(1<<LOW2_Q11);
    
    setupPWM_T1();
    setupADC_10();
    
    // Start motion in CCW direction
    startPWM_T1B_LOW2_Q11();
    HIGH1_Q7_PORT &= ~(1<<HIGH1_Q7);
    pwm_duty_cycle_t1b_low2_q11(100);
    prev_duty=100;
    uint8_t prev_dir = CCW;
    
    while(1)
    {
        if (IINCHIP_READ(Sn_IR(0)) & Sn_IR_RECV)
        {
            IINCHIP_WRITE(Sn_IR(0), Sn_IR_RECV);    // Clearing flag
            recvfrom(0, (uint8_t *)&duty_cycle, 2, sender_ip, &sender_port);
            if (/*(sender_ip[3]==5) &&*/ (sender_port==MATLAB_PENDULUM_SETPOINT))
            {
                if (duty_cycle!=prev_duty)
                {
                    if (duty_cycle<0) // CW direction
                    {
                        if (prev_dir != CW)
                        {
                            // Apply brake by setting both ends to GND
                            stopPWM_T1B_LOW2_Q11();
                            HIGH1_Q7_PORT |= (1<<HIGH1_Q7);
                            HIGH2_Q9_PORT |= (1<<HIGH2_Q9);
                            LOW1_Q10_PORT |= (1<<LOW1_Q10);
                            LOW2_Q11_PORT |= (1<<LOW2_Q11);
                            _delay_ms(1);
                            
                            // Float
                            LOW1_Q10_PORT &= ~(1<<LOW1_Q10);
                            LOW2_Q11_PORT &= ~(1<<LOW2_Q11);
                            
                            // Reverse direction
                            HIGH2_Q9_PORT &= ~(1<<HIGH2_Q9);
                            startPWM_T1A_LOW1_Q10();         // OC1A ready to SWITCH
                            
                            prev_dir = CW;
                        }
                        
                        pwm_duty_cycle_t1a_low1_q10((uint8_t)(-duty_cycle));
                        prev_duty = duty_cycle;
                    }
                    
                    else if (duty_cycle>0)  // CCW direction
                    {
                        if (prev_dir != CCW)
                        {
                            // Apply brake by setting both ends to GND
                            stopPWM_T1A_LOW1_Q10();
                            HIGH1_Q7_PORT |= (1<<HIGH1_Q7);
                            HIGH2_Q9_PORT |= (1<<HIGH2_Q9);
                            LOW1_Q10_PORT |= (1<<LOW1_Q10);
                            LOW2_Q11_PORT |= (1<<LOW2_Q11);
                            _delay_ms(1);
                            
                            // Float
                            LOW1_Q10_PORT &= ~(1<<LOW1_Q10);
                            LOW2_Q11_PORT &= ~(1<<LOW2_Q11);
                            
                            // Reverse direction
                            HIGH1_Q7_PORT &= ~(1<<HIGH1_Q7); // Turning ON
                            startPWM_T1B_LOW2_Q11();         // OC1B ready to SWITCH
                            
                            prev_dir = CCW;
                        }

                        pwm_duty_cycle_t1b_low2_q11(duty_cycle);
                        prev_duty = duty_cycle;
                    }
                    
                    else if (prev_dir != STOPPED)   // Brake, duty = 0;
                    {
                        stopPWM_T1A_LOW1_Q10();
                        stopPWM_T1B_LOW2_Q11();
                        
                        // Apply brake by setting both ends to GND
                        stopPWM_T1A_LOW1_Q10();
                        HIGH1_Q7_PORT |= (1<<HIGH1_Q7);
                        HIGH2_Q9_PORT |= (1<<HIGH2_Q9);
                        LOW1_Q10_PORT |= (1<<LOW1_Q10);
                        LOW2_Q11_PORT |= (1<<LOW2_Q11);
                        
                        prev_duty = duty_cycle;
                        prev_dir = STOPPED;
                    }
                }
                var = getADCdata_10(0);
                sendto(0, (uint8_t *)&var, 2, sender_ip, MATLAB_PENDULUM_PORT);
            }
        }
    }
}