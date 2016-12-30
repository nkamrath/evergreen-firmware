#include "networking.h"
#include "light_controller.h"
#include "firmware_version.h"

#include "lwip/udp.h"

#define _NETWORK_MANAGER_PACKET_MARKER "NMAN"

#define _BUFFER_LENGTH   1024
#define BROADCAST_GROUP_ADDR 	"224.1.1.1"
#define UNICAST_PORT			50008
#define BROADCAST_PORT			50009

/***********************
 * Private Types
 ***********************/
typedef enum
{
	PACKET_TYPE_ANNOUNCE,
	PACKET_TYPE_ANNOUNCE_RESPONSE,
	PACKET_TYPE_BEACON,
	PACKET_TYPE_COMMAND
}packet_type_t;

typedef struct
{
	uint32_t marker;							/**< Current state of the light (on/off) **/
	uint32_t sequence_number;						/**< If motion trigger is set to on 1 else 0 **/
	uint32_t packet_type;		/**< Current motion trigger on time  in seconds **/
	uint32_t payload_length;					/**< Firmware version.  MSB is not currently used (0x00, MAJOR, MINOR, PATCH) **/
	uint8_t* payload;

}__attribute__((packed)) packet_header_t;

typedef struct
{
	uint8_t light_state;						/**< Current state of the light (on/off) **/
	uint8_t motion_trigger;						/**< If motion trigger is set to on 1 else 0 **/
	uint32_t motion_trigger_time_seconds;		/**< Current motion trigger on time  in seconds **/
	uint32_t firmware_version;					/**< Firmware version.  MSB is not currently used (0x00, MAJOR, MINOR, PATCH) **/

}__attribute__((packed)) network_manager_beacon_t;

/***********************
 * Private Variables
 ***********************/
//UDP socket variables
static struct udp_pcb* pcb;
static ip_addr_t network_manager_addr;
static ip_addr_t multicast_addr;
static struct pbuf packet_buffer;
static uint8_t _buffer[_BUFFER_LENGTH];

//server network variables
static bool found_server = false;
static ip_addr_t server_addr;
static uint16_t server_port = 0;
//static uint32_t last_sequence_number;

void NetworkManager_RxCallback(void* arg, struct udp_pcb* upcb, struct pbuf* p, const ip_addr_t* remote_addr, u16_t port)
{
	bool ok = true;

	//get header from packet
	packet_header_t* header = (packet_header_t*)p->payload;

	if(header->packet_type == PACKET_TYPE_COMMAND)
	{
		uint8_t* buffer = (uint8_t*)&header->payload;
		uint32_t rcv_len = header->payload_length;
		if(memcmp("light on", buffer, 8) == 0)
		{
			LightController_MotionTriggerOff();
			LightController_On();
		}
		else if(memcmp("light off", buffer, 9) == 0)
		{
			LightController_MotionTriggerOff();
			LightController_Off();
		}
		else if(memcmp("light auto-time", buffer, 15) == 0)
		{
			uint32_t new_time = 0;
			if(rcv_len - 15 == 4)
			{
			 	new_time += (buffer[16]- 48) *100;
			 	new_time += (buffer[17]- 48) *10;
				new_time += (buffer[18]- 48) *1;
			}
			else if(rcv_len - 15 == 3)
			{
				new_time += (buffer[16]- 48) *10;
				new_time += (buffer[17]- 48) *1;
			}
			else
			{
				new_time += (buffer[16]- 48) *1;
			}
			
			printf("setting new motion on time: %d\r\n", new_time);
			LightController_SetMotionOnTime(new_time);
		}
		else if(memcmp("light auto", buffer, 10) == 0)
		{
			LightController_MotionTriggerOn();
		}
		else
		{
			ok = false;
		}
	}
	else if(header->packet_type == PACKET_TYPE_ANNOUNCE_RESPONSE)
	{
		//this is from server, copy address and port, update internal variables as necessary
		server_addr = *remote_addr;
		server_port = UNICAST_PORT;
		found_server = true;
		ok = false; //don't send a response packet, don't need because no retries or notify necessary
		printf("\r\nGot server announce response\r\n");
	}

	if(ok)
	{
		//send an op packet
	}

	pbuf_free(p);
}

bool NetworkManager_Create(void)
{
	pcb = udp_new();

	multicast_addr.u_addr.ip4.addr = inet_addr(BROADCAST_GROUP_ADDR);

    network_manager_addr.u_addr.ip4.addr = htonl(INADDR_ANY);

    udp_bind(pcb, &network_manager_addr, UNICAST_PORT);

    udp_recv(pcb, NetworkManager_RxCallback, NULL);

    return true;
}

bool NetworkManager_Beacon(void)
{
	packet_header_t* header = (packet_header_t*)_buffer;
	memcpy(&header->marker, _NETWORK_MANAGER_PACKET_MARKER, 4);

	if(found_server)
	{
		header->packet_type = PACKET_TYPE_BEACON;
		network_manager_beacon_t* beacon = (network_manager_beacon_t*)&header->payload;
		beacon->light_state = LightController_GetState();
		beacon->motion_trigger = LightController_GetMotionTriggerState();
		beacon->motion_trigger_time_seconds = LightController_GetMotionOnTime();
		beacon->firmware_version = FIRMWARE_VERSION;

	    packet_buffer.payload = _buffer;
	    packet_buffer.len = sizeof(network_manager_beacon_t) + sizeof(packet_header_t);
	    packet_buffer.tot_len = sizeof(network_manager_beacon_t) + sizeof(packet_header_t);
	    packet_buffer.type = PBUF_RAM;
	    packet_buffer.ref = 1;
	    header->payload_length = sizeof(network_manager_beacon_t);
	    udp_sendto(pcb, &packet_buffer, &server_addr, server_port);
	}
	else
	{
		//need to announce and try to find server
		header->packet_type = PACKET_TYPE_ANNOUNCE;
		header->payload_length = 0; //no payload for announce

	    packet_buffer.payload = _buffer;
	    packet_buffer.len = sizeof(packet_header_t);
	    packet_buffer.tot_len = sizeof(packet_header_t);
	    packet_buffer.type = PBUF_RAM;
	    packet_buffer.ref = 1;
	    udp_sendto(pcb, &packet_buffer, &multicast_addr, BROADCAST_PORT);
	}

	return true;
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