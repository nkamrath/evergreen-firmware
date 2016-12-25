#include "networking.h"
#include "light_controller.h"

#define BROADCAST_GROUP_ADDR 	"224.1.1.1"
#define BROADCAST_PORT			50008

int multicast_socket = 0;
struct sockaddr_in multicast_addr;
struct sockaddr_in local_addr;
struct sockaddr_in remote_addr;

#define _RX_BUFFER_LENGTH	128
uint8_t _rx_buffer[_RX_BUFFER_LENGTH];

void udp_multicast_init()
{
	memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = inet_addr(BROADCAST_GROUP_ADDR);
    multicast_addr.sin_port=htons(BROADCAST_PORT);

    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port=htons(BROADCAST_PORT);    

    //setup the broadcast udp socket
    if((multicast_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP) < 0))
    {
    	Pin_SetOutput(DEBUG_PIN3_MASK);
    }

    printf("multicast socket: %d\r\n", multicast_socket);

    if(bind(multicast_socket, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0)
    {
    	Pin_SetOutput(DEBUG_PIN3_MASK);
    }

    //set non blocking mode so reads can happen because ioctl doesn't seem to work for detecting new data avaialable
    uint32_t ioctl_mode = 1; //non blocking
    ioctl(multicast_socket, FIONBIO, &ioctl_mode);
}

void udp_broadcast(uint16_t port, uint8_t* buffer, uint16_t buffer_length)
{
	multicast_addr.sin_port=htons(port);
    if(sendto(multicast_socket, buffer, buffer_length, 0, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr)) < 0)
    {
    	Pin_SetOutput(DEBUG_PIN3_MASK);
    }
}

void udp_service_socket(void)
{
	int count = 0;
	ioctl(multicast_socket, FIONREAD, &count);
	//if(count > 0)
	//{
		socklen_t temp_len;
		int rcv_len = recvfrom(multicast_socket, _rx_buffer, _RX_BUFFER_LENGTH, 0, (struct sockaddr*)&remote_addr, &temp_len);
		if(rcv_len > 0)
		{
			printf("Got udp packet from: %s:%d\r\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
			printf("data: %s\r\n", _rx_buffer);

			if(memcmp("light on", _rx_buffer, 8) == 0)
			{
				LightController_MotionTriggerOff();
				LightController_On();
			}
			else if(memcmp("light off", _rx_buffer, 9) == 0)
			{
				LightController_MotionTriggerOff();
				LightController_Off();
			}
			else if(memcmp("light auto-time", _rx_buffer, 15) == 0)
			{
				uint32_t new_time = 0;
				if(rcv_len - 15 == 4)
				{
				 	new_time += (_rx_buffer[16]- 48) *100;
				 	new_time += (_rx_buffer[17]- 48) *10;
					new_time += (_rx_buffer[18]- 48) *1;
				}
				else if(rcv_len - 15 == 3)
				{
					new_time += (_rx_buffer[16]- 48) *10;
					new_time += (_rx_buffer[17]- 48) *1;
				}
				else
				{
					new_time += (_rx_buffer[16]- 48) *1;
				}
				
				printf("setting new motion on time: %d\r\n", new_time);
				LightController_SetMotionOnTime(new_time);
			}
			else if(memcmp("light auto", _rx_buffer, 10) == 0)
			{
				LightController_MotionTriggerOn();
			}
		}
	//}
	//else if(count < 0)
	//{
	//	printf ("ioctl error!\r\n");
	//}
}


//======================================================
//BELOW IS LOWER LAYER UDP/MULTICAST STUFF
// struct udp_pcb* pcb;
// ip_addr_t multicast_addr;
// struct pbuf packet_buffer;

//void udp_multicast_init()
//{
// 	pcb = udp_new();
// 	multicast_addr.u_addr.ip4.addr = inet_addr(BROADCAST_GROUP_ADDR);
// 	udp_connect(pcb, &multicast_addr, BROADCAST_PORT);
//}

//void udp_broadcast(uint16_t port, uint8_t* buffer, uint16_t buffer_length)
//{
    // packet_buffer.next = NULL;
    // packet_buffer.payload = buffer;
    // packet_buffer.len = buffer_length;
    // packet_buffer.tot_len = buffer_length;
    // packet_buffer.flags = 0;
    // packet_buffer.ref = 1;
    // packet_buffer.type = PBUF_RAM;
    // err_t res = udp_send(pcb, &packet_buffer);
    // if(res < 0)
    // {
    // 	Pin_SetOutput(DEBUG_PIN3_MASK);
    // }
//}