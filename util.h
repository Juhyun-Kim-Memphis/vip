//
// Created by juhyun on 17. 10. 12.
//

#ifndef VIP_UTIL_H
#define VIP_UTIL_H

#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <stdarg.h>
#include "libnet.h"

#define SUCCESS         0
#define FAILURE         -1
#define true 1
#define false 0

#ifndef IF_NAMESIZE
#define IF_NAMESIZE     16    /* refer to net/if.h */
#endif

#define MACADDR_STRLEN      (2 * 6 + 5 + 1)     /* XX:XX:XX:XX:XX:XX */
#define IP_LEN  (3*4+3+1)

#define VIP_ALIAS_MAX           256     /* max. # of IPs to take over */
#define IFCONFIG    "ifconfig"

#define TBCM_MAX_SIZE_BUFF    (32*1024)

typedef int tb_bool_t;

typedef struct tbcm_vip_s tbcm_vip_t;
struct tbcm_vip_s {
    int         flag_init;
    /* NOTE:  VIP들은 모두 public device하나에 alias되고,
     * netmask, broadcast address는 모두 같다고 가정한다.
     */

    // doesn't need to be initialize before tbcm_vip_init;
    char        dev_pub[IF_NAMESIZE];   /* public device */
    char        dev_ini[IF_NAMESIZE];   /* 1st aliased NIC name */
    char        dev_lo[IF_NAMESIZE];    /* loop back device */
    uint32_t    netmask;
    uint32_t    broadcast;
    uint32_t    vips[VIP_ALIAS_MAX];    /* max VIPs bound to public device */
    int         count;
};

typedef struct tbcm_node_s {
    int flag_socket;
} tbcm_node_t;

int add_ip_to_nic(char *device, uint32_t vip, uint32_t netmask, uint32_t broadcast);
int remove_ip_from_nic(char *device, uint32_t vip);
char *iptos(uint32_t in);
int shell_command(const char *fmt, ...);
void print_vip_info(tbcm_vip_t *vip_info);

#endif //VIP_UTIL_H
