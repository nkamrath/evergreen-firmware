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

bool NetworkManager_Create(void);
bool NetworkManager_Beacon(void);

#endif