#ifndef NETWORKING_UTILS_H
#define NETWORKING_UTILS_H

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/udp.h"

#include "pin.h"

#include <stdint.h>
#include <string.h>

void udp_multicast_init(void);
void udp_broadcast(uint16_t port, uint8_t* buffer, uint16_t buffer_length);
void udp_service_socket(void);

#endif