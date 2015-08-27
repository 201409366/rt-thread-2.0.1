#ifndef __COMMUNICATIONPROTOCAL_H__
#define __COMMUNICATIONPROTOCAL_H__

#include <rtthread.h>

typedef struct
{    
	rt_uint16_t authentication;
	rt_uint8_t	type;
	rt_uint8_t	accex;
	rt_uint8_t	accey;
	rt_uint8_t	accez;
	rt_uint8_t	gyrox;
	rt_uint8_t	gyroy;
	rt_uint8_t	gyroz;
	rt_uint8_t	m1pwm;
	rt_uint8_t	m2pwm;
	rt_uint8_t	m3pwm;
	rt_uint8_t	m4pwm;
	rt_uint8_t	LEDS;
	rt_uint16_t	crc;	
} CommunicationProtocol;

rt_err_t getProtocalData(CommunicationProtocol *cp ,rt_uint8_t * data);

#endif /* __COMMUNICATIONPROTOCAL_H__ */

