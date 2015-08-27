#include "CommunicationProtocol.h"
#include "nRF24L01.h"

rt_err_t getProtocalData(CommunicationProtocol *cp ,rt_uint8_t * data)
{
	
	rt_memcpy(cp,data,RX_PLOAD_WIDTH);		
	return RT_EOK;
}

