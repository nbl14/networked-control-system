/*
 * dhcp.c
 *
 *     Created: 3/5/2014
 *      Author: Nabeel Ahmad (nbl14@hotmail.com)
 * Description: Definition of a function that implements an incomplete version
 *              of the DHCP client state-machine.
 */ 

#include <avr/io.h>
#include <string.h>
#include "types.h"
#include "w5100.h"
#include "socket.h"
#include "dhcp.h"
#include "../lcd_farhan.h"


uint8_t runDHCP(void)
{
    #define HOST_NAME	"Khand\0"		    /**< Host Name */
    u_char SRC_MAC_ADDR[6];				/**< Local MAC address */
    u_char GET_SN_MASK[4];				/**< Subnet mask received from the DHCP server */
    u_char GET_GW_IP[4];				/**< Gateway ip address received from the DHCP server */
    u_char GET_SIP[4] = {0, 0, 0, 0};	/**< Local ip address received from the DHCP server */

    static u_char DHCP_SIP[4] = {0, 0, 0, 0};				/**< DHCP server ip address is discovered */
    
    static u_long DHCP_XID = 0x75269875;
    static RIP_MSG* pRIPMSG;				/**< Pointer for the DHCP message */
    RIP_MSG RIPMSG;                         /**< by nbl*/
    
    uint16_t recv_msg_size;
    uint8_t *recv_msg_end;
    uint8_t *current_option;
    uint8_t  option_length;
    
    u_char ip[4];
    u_int i=0;
    uint16_t port;
    
    // MAC address
    SRC_MAC_ADDR[0] = 0x8A;
    SRC_MAC_ADDR[1] = 0x26;
    SRC_MAC_ADDR[2] = 0xE0;
    SRC_MAC_ADDR[3] = 0x1A;
    SRC_MAC_ADDR[4] = 0xDD;
    SRC_MAC_ADDR[5] = 0x83;
    setSHAR(SRC_MAC_ADDR);
    
    // Set up socket
    socket(0, Sn_MR_UDP, DHCP_CLIENT_PORT, 0);     // Initialize sockets
    
    
    /************************************************************************/
    /*                           Send DHCPDISCOVER                          */
    /************************************************************************/
    
	pRIPMSG = &RIPMSG;

	// *((u_long*)DHCP_SIP)=0;
	// *((u_long*)DHCP_REAL_SIP)=0;
	
	memset((void*)pRIPMSG,0,sizeof(RIP_MSG));

	pRIPMSG->op = DHCP_BOOTREQUEST;
	pRIPMSG->htype = DHCP_HTYPE10MB;
	pRIPMSG->hlen = DHCP_HLENETHERNET;
	pRIPMSG->hops = DHCP_HOPS;
	pRIPMSG->xid = DHCP_XID;
	pRIPMSG->secs = DHCP_SECS;
	pRIPMSG->flags = DHCP_FLAGSBROADCAST;
	pRIPMSG->chaddr[0] = SRC_MAC_ADDR[0];
	pRIPMSG->chaddr[1] = SRC_MAC_ADDR[1];
	pRIPMSG->chaddr[2] = SRC_MAC_ADDR[2];
	pRIPMSG->chaddr[3] = SRC_MAC_ADDR[3];
	pRIPMSG->chaddr[4] = SRC_MAC_ADDR[4];
	pRIPMSG->chaddr[5] = SRC_MAC_ADDR[5];

	/* MAGIC_COOKIE */
	pRIPMSG->OPT[i++] = (char)((MAGIC_COOKIE >> 24)& 0xFF);
	pRIPMSG->OPT[i++] = (char)((MAGIC_COOKIE >> 16)& 0xFF);
	pRIPMSG->OPT[i++] = (char)((MAGIC_COOKIE >> 8)& 0xFF);
	pRIPMSG->OPT[i++] = (char)(MAGIC_COOKIE& 0xFF);

	/* Option Request Param. */
	pRIPMSG->OPT[i++] = dhcpMessageType;
	pRIPMSG->OPT[i++] = 0x01;
	pRIPMSG->OPT[i++] = DHCP_DISCOVER;

	// Client identifier
	pRIPMSG->OPT[i++] = dhcpClientIdentifier;
	pRIPMSG->OPT[i++] = 0x07;
	pRIPMSG->OPT[i++] = 0x01;
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[0];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[1];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[2];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[3];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[4];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[5];
	
	// host name
	pRIPMSG->OPT[i++] = hostName;
	pRIPMSG->OPT[i++] = strlen(HOST_NAME); // length of hostname
	strcpy((char*)&(pRIPMSG->OPT[i]), HOST_NAME);
	i+=strlen(HOST_NAME);

	// IP Address Lease Time request
    pRIPMSG->OPT[i++] = dhcpIPaddrLeaseTime;
    pRIPMSG->OPT[i++] = 4;
    pRIPMSG->OPT[i++] = 0xFF;
    pRIPMSG->OPT[i++] = 0xFF;
    pRIPMSG->OPT[i++] = 0xFF;
    pRIPMSG->OPT[i++] = 0xFF;
    
    // Parameter Request List
	pRIPMSG->OPT[i++] = dhcpParamRequest;
	pRIPMSG->OPT[i++] = 0x02;
	pRIPMSG->OPT[i++] = subnetMask;
	pRIPMSG->OPT[i++] = routersOnSubnet;
	// pRIPMSG->OPT[i++] = dns;
	// pRIPMSG->OPT[i++] = domainName;
	// pRIPMSG->OPT[i++] = dhcpT1value;
	// pRIPMSG->OPT[i++] = dhcpT2value;
	pRIPMSG->OPT[i++] = endOption;

	/* send broadcasting packet */
	ip[0] = 255;
	ip[1] = 255;
	ip[2] = 255;
	ip[3] = 255;
    
	sendto(0, (u_char *)pRIPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
	
    
    /************************************************************************/
    /*                           Receive DHCPOFFER                          */
    /************************************************************************/
    
    while (1)
    {
        if (IINCHIP_READ(Sn_IR(0)) & Sn_IR_RECV)
        {
            IINCHIP_WRITE(Sn_IR(0), Sn_IR_RECV);    // Clearing flag
            recv_msg_size = recvfrom(0, (uint8_t *)pRIPMSG, sizeof(RIPMSG), ip, &port);
            
            if (port==DHCP_SERVER_PORT                      &&
                !memcmp(RIPMSG.chaddr, SRC_MAC_ADDR, 6)     &&
                RIPMSG.xid == DHCP_XID                      )
            {
                // Check options
                recv_msg_end = (uint8_t *)((uint16_t)&(RIPMSG.op) + recv_msg_size);
                current_option = (uint8_t *)((uint16_t)&(RIPMSG.op) + 240);
                while(current_option < recv_msg_end)
                {
                    switch (*(current_option++))
                    {
                        case padOption:
                            break;
                        
                        case endOption:
                            break;
                        
                        case dhcpMessageType:
                            current_option++;
                            if ((*current_option++) != DHCP_OFFER)
                            {
                                // lcd_msg_delayed("wrong reply")
                                return 0;
                            }
                            else break;
                            
                        case subnetMask:
                            option_length = *current_option++;
                            memcpy(GET_SN_MASK, current_option, 4);
                            current_option += option_length;
                            break;
                            
                        case routersOnSubnet:
                            option_length = *current_option++;
                            memcpy(GET_GW_IP, current_option, 4);
                            current_option += option_length;
                            break;
                            
                        case dhcpServerIdentifier:
                            current_option++;
                            memcpy(DHCP_SIP, current_option, 4);
                            current_option += 4;
                            break;
                            
                        default:
                            option_length = *current_option++;
                            current_option += option_length;
                            break;
                    }
                }
                
                memcpy(GET_SIP, (RIPMSG.yiaddr), 4);
                break;
            }
            else
            {
                // lcd_msg_delayed("unknown reply")
                return 0;
            }
        }
    }
    
    /************************************************************************/
    /*                            Send DHCPREQUEST                          */
    /************************************************************************/
    
    i=0;
        
    memset((void*)pRIPMSG, 0, sizeof(RIP_MSG));

    pRIPMSG->op = DHCP_BOOTREQUEST;
    pRIPMSG->htype = DHCP_HTYPE10MB;
    pRIPMSG->hlen = DHCP_HLENETHERNET;
    pRIPMSG->hops = DHCP_HOPS;
    pRIPMSG->xid = DHCP_XID;
    pRIPMSG->secs = DHCP_SECS;
    pRIPMSG->flags = DHCP_FLAGSBROADCAST;
    pRIPMSG->chaddr[0] = SRC_MAC_ADDR[0];
    pRIPMSG->chaddr[1] = SRC_MAC_ADDR[1];
    pRIPMSG->chaddr[2] = SRC_MAC_ADDR[2];
    pRIPMSG->chaddr[3] = SRC_MAC_ADDR[3];
    pRIPMSG->chaddr[4] = SRC_MAC_ADDR[4];
    pRIPMSG->chaddr[5] = SRC_MAC_ADDR[5];

    /* MAGIC_COOKIE */
    pRIPMSG->OPT[i++] = (char)((MAGIC_COOKIE >> 24)& 0xFF);
    pRIPMSG->OPT[i++] = (char)((MAGIC_COOKIE >> 16)& 0xFF);
    pRIPMSG->OPT[i++] = (char)((MAGIC_COOKIE >> 8)& 0xFF);
    pRIPMSG->OPT[i++] = (char)(MAGIC_COOKIE& 0xFF);

    /* Option Request Param. */
    pRIPMSG->OPT[i++] = dhcpMessageType;
    pRIPMSG->OPT[i++] = 0x01;
    pRIPMSG->OPT[i++] = DHCP_REQUEST;

    // Client identifier
    pRIPMSG->OPT[i++] = dhcpClientIdentifier;
    pRIPMSG->OPT[i++] = 0x07;
    pRIPMSG->OPT[i++] = 0x01;
    pRIPMSG->OPT[i++] = SRC_MAC_ADDR[0];
    pRIPMSG->OPT[i++] = SRC_MAC_ADDR[1];
    pRIPMSG->OPT[i++] = SRC_MAC_ADDR[2];
    pRIPMSG->OPT[i++] = SRC_MAC_ADDR[3];
    pRIPMSG->OPT[i++] = SRC_MAC_ADDR[4];
    pRIPMSG->OPT[i++] = SRC_MAC_ADDR[5];
    
    // Request IP Address
    pRIPMSG->OPT[i++] = dhcpRequestedIPaddr;
    pRIPMSG->OPT[i++] = 0x04;
    pRIPMSG->OPT[i++] = GET_SIP[0];
    pRIPMSG->OPT[i++] = GET_SIP[1];
    pRIPMSG->OPT[i++] = GET_SIP[2];
    pRIPMSG->OPT[i++] = GET_SIP[3];
    
    // Server Identifier
    pRIPMSG->OPT[i++] = dhcpServerIdentifier;
    pRIPMSG->OPT[i++] = 0x04;
    pRIPMSG->OPT[i++] = DHCP_SIP[0];
    pRIPMSG->OPT[i++] = DHCP_SIP[1];
    pRIPMSG->OPT[i++] = DHCP_SIP[2];
    pRIPMSG->OPT[i++] = DHCP_SIP[3];
    
    // host name
    pRIPMSG->OPT[i++] = hostName;
    pRIPMSG->OPT[i++] = strlen(HOST_NAME); // length of hostname
    strcpy((char*)&(pRIPMSG->OPT[i]), HOST_NAME);
    i+=strlen(HOST_NAME);

    /* IP Address Lease Time request
    pRIPMSG->OPT[i++] = dhcpIPaddrLeaseTime;
    pRIPMSG->OPT[i++] = 4;
    pRIPMSG->OPT[i++] = 0xFF;
    pRIPMSG->OPT[i++] = 0xFF;
    pRIPMSG->OPT[i++] = 0xFF;
    pRIPMSG->OPT[i++] = 0xFF;   */
    
    // Parameter Request List
    pRIPMSG->OPT[i++] = dhcpParamRequest;
    pRIPMSG->OPT[i++] = 0x02;
    pRIPMSG->OPT[i++] = subnetMask;
    pRIPMSG->OPT[i++] = routersOnSubnet;
    // pRIPMSG->OPT[i++] = dns;
    // pRIPMSG->OPT[i++] = domainName;
    // pRIPMSG->OPT[i++] = dhcpT1value;
    // pRIPMSG->OPT[i++] = dhcpT2value;
    pRIPMSG->OPT[i++] = endOption;

    /* send broadcasting packet */
    ip[0] = 255;
    ip[1] = 255;
    ip[2] = 255;
    ip[3] = 255;
    
    sendto(0, (u_char *)pRIPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
    
    
    /************************************************************************/
    /*                           Receive DHCPACK                            */
    /************************************************************************/
    
    while (1)
    {
        if (IINCHIP_READ(Sn_IR(0)) & Sn_IR_RECV)
        {
            IINCHIP_WRITE(Sn_IR(0), Sn_IR_RECV);    // Clearing flag
            recv_msg_size = recvfrom(0, (uint8_t *)pRIPMSG, sizeof(RIPMSG), ip, &port);
            
            if (port==DHCP_SERVER_PORT                      &&
                !memcmp(RIPMSG.chaddr, SRC_MAC_ADDR, 6)     &&
                RIPMSG.xid == DHCP_XID                      )
            {
                // Check options
                recv_msg_end = (uint8_t *)((uint16_t)&(RIPMSG.op) + recv_msg_size);
                current_option = (uint8_t *)((uint16_t)&(RIPMSG.op) + 240);
                while(current_option < recv_msg_end)
                {
                    switch (*(current_option++))
                    {
                        case padOption:
                        break;
                        
                        case endOption:
                        break;
                        
                        case dhcpMessageType:
                        current_option++;
                        if ((*current_option++) != DHCP_ACK)
                        {
                            // lcd_msg_delayed("wrong reply")
                            return 0;
                        }
                        else break;
                        
                        case subnetMask:
                        option_length = *current_option++;
                        memcpy(GET_SN_MASK, current_option, 4);
                        current_option += option_length;
                        break;
                        
                        case routersOnSubnet:
                        option_length = *current_option++;
                        memcpy(GET_GW_IP, current_option, 4);
                        current_option += option_length;
                        break;
                        
                        default:
                        option_length = *current_option++;
                        current_option += option_length;
                        break;
                    }
                }
                
                memcpy(GET_SIP, (RIPMSG.yiaddr), 4);
                memcpy(DHCP_SIP, (RIPMSG.siaddr), 4);
                break;
            }
            else
            {
                return 0;
            }
        }
    }
    
    // Set network parameters
    setSUBR(GET_SN_MASK);
    setGAR(GET_GW_IP);
    setSIPR(RIPMSG.yiaddr);
    
    // Close socket
    close(0);
    
    return(1);
}