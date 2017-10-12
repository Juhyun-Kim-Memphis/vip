#include "util.h"
#include "tbmsg.h"

static void
tbcm_sendmsg_vip_info( tbcm_node_t *node, tbcm_vip_t *vip_info_ptr) {
    tbmsg_tbcm_vip_info_t tbmsg_tbcm_vip_info;
    tbmsg_struct_vip_list_t *tbmsg_struct_vip_list;
    tbmsg_t *msg;
    unsigned int size_raw;
    char buff_raw[TBCM_MAX_SIZE_BUFF]= {0,};
    int i;

    if( node->flag_socket != true )
        return;

    tbmsg_struct_vip_list = (tbmsg_struct_vip_list_t *)
            malloc( sizeof(tbmsg_struct_vip_list_t) * VIP_ALIAS_MAX );

    for( i = 0; i < VIP_ALIAS_MAX; i ++ ) {
        tbmsg_struct_vip_list_set( &tbmsg_struct_vip_list[i],
                                   (int)vip_info_ptr->vips[i] );
    }

    tbmsg_tbcm_vip_info_set( &tbmsg_tbcm_vip_info, vip_info_ptr->flag_init,
                             vip_info_ptr->dev_pub, vip_info_ptr->dev_ini,
                             vip_info_ptr->dev_lo, vip_info_ptr->netmask,
                             vip_info_ptr->broadcast, VIP_ALIAS_MAX,
                             tbmsg_struct_vip_list );

    msg = (tbmsg_t*)(&tbmsg_tbcm_vip_info);

//    size_raw = tbmsg_get_msg_raw_size( msg );
//    tbmsg_serialize( SYSTEM_ALLOC, msg, buff_raw );
//    tbcm_set_write( node, buff_raw, size_raw );

//    /* 바로 메시지를 보내주기 위해... */
//    tbcm_reset_fence();
//
//    tb_free( SYSTEM_ALLOC, tbmsg_struct_vip_list );
}

/* NOTE:
 * add_ip_to_nic() 와 remove_ip_from_nic() 를 위해
 * 일단은 'ifconfig'를 system으로 호출하는 방식을 사용한다.
 * TODO: 추후, net-tool을 통합?
 */
int add_ip_to_nic(char *device, uint32_t vip,
              uint32_t netmask, uint32_t broadcast)
{
    /* device는 public NIC에 unique identifier를 붙여서 넘어와야 한다. */
    if (shell_command("%s %s %s netmask %s broadcast %s",
                      IFCONFIG, device, iptos(vip),
                      iptos(netmask), iptos(broadcast)))
        return FAILURE;

    printf("VIP %s added at %s (netmask:%s, broadcast:%s)",
              iptos(vip), device, iptos(netmask), iptos(broadcast));
    return SUCCESS;
}

int
remove_ip_from_nic(char *device, uint32_t vip)
{
    /* device는 public NIC에 unique identifier를 붙여서 넘어와야 한다. */
    /* NOTE:
     * Virtual IP가 public IP가 같은 subnet에 할당된 IP range를 사용하거나
     * 최소한 하나의 VIP를 유지하는 경우 (tibero cluster에 해당) routing
     * table을 수정할 필요는 없어 보인다. 더욱이 대부분(?) 해당 IP가 없어지면
     * rtentry도 같이 삭제되는듯 하다.
     */

    /* shell_command("%s -n del -host %s", ROUTE, iptos(vip)); */
    if (shell_command("%s %s down", IFCONFIG, device))
        return FAILURE;

    printf("VIP %s removed from %s", iptos(vip), device);

    return 0;
}

/*
 * When the cluster needs to move the IP to a different NIC,
 * be it on the same machine or a different one, it reconfigures the NICs
 * appropriately then broadcasts a gratuitous ARP reply to inform
 * the neighboring machines about the change in MAC for the IP.  Machines
 * receiving the ARP packet then update their ARP tables with the new MAC.
 */
static int
send_gratuitous_arp(char *device, uint32_t vip,
                    u_int count, u_long interval)
{
    int         rc;
    u_int       i;
    libnet_t   *libnet_context = NULL;
    struct libnet_ether_addr *e;
    char        macaddr_str[MACADDR_STRLEN];
    char        errbuf[LIBNET_ERRBUF_SIZE + 1];

    libnet_context = libnet_init(LIBNET_LINK_ADV,   /* injection type */
                                 device,            /* network interface */
                                 errbuf);           /* errbuf */
//    if (!libnet_context) {
//        TB_CM_LOG( LOG_ESSENTIAL,"libnet_init(): %s", errbuf);
//        return FAILURE;
//    }
//
//    /* get HW address */
//    if (!(e = libnet_get_hwaddr(libnet_context))) {
//        TB_CM_LOG( LOG_ESSENTIAL,
//                   "libnet_get_hwaddr(): %s", libnet_geterror(libnet_context));
//        return FAILURE;
//    }
//
//    TB_CM_LOG(LOG_DEFAULT, "sending gratuitous arp packets via %s (MAC:%s)",
//              device, macaddr2str(e->ether_addr_octet, macaddr_str));
//
//    /* NOTE:
//     * We need to send both a broadcast ARP request as well as the ARP response
//     * we were already sending.  All the interesting research work for this fix
//     * was done by Masaki Hasegawa <masaki-h@pp.iij4u.or.jp> and his colleagues.
//     */
//    /* Note that some devices will respond to the gratuitous request and some
//     * will respond to the gratuitous reply. If one is trying to write software
//     * for moving IP addresses around that works with all routers, switches and
//     * IP stacks, it is best to send both the request and the reply
//     */
//    for (i = 0; i < count; ++i) {
//        rc = send_arp(libnet_context, vip, e->ether_addr_octet, ARPOP_REQUEST);
//        if (rc < 0)
//            break;
//        TB_CM_LOG(LOG_OPTIONAL, "An arp request sent (%d bytes)", rc);
//
//        USLEEP(interval);
//
//        rc = send_arp(libnet_context, vip, e->ether_addr_octet, ARPOP_REPLY);
//        if (rc < 0)
//            break;
//        TB_CM_LOG(LOG_OPTIONAL, "An arp reply sent (%d bytes)", rc);
//    }
//
//    libnet_destroy(libnet_context);
//    return SUCCESS;
}

int
shell_command(const char *fmt, ...)
{
    int     rc;
    int     exit_status;
    va_list ap;
    char    buf[1024];

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    printf("start exec %s", buf);

    rc = system(buf);
    if (rc == -1) {
        printf("exec %s failed (%d)",  buf, errno);
    }
    else {
        exit_status = 0;
        if (WIFEXITED(rc)) {
            exit_status = WEXITSTATUS(rc);
        }
        printf("exec %s success. exit status %d",
               buf, exit_status);
        rc = exit_status;
    }

    va_end(ap);
    return rc;
}

/* from tcptraceroute */
#define IPTOSBUFFERS    12

char *iptos(uint32_t in)
{
    static char output[IPTOSBUFFERS][IP_LEN];
    static short which;
    u_char *p;

    p = (u_char *)&in;
    which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
    sprintf(output[which], "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    return output[which];
}

void print_vip_info(tbcm_vip_t *v) {
    printf("\n\nprint_vip_info result!!\n");
    printf("flag_init: %d\n", v->flag_init);
    printf("dev_pub: %s\n", v->dev_pub);
    printf("dev_ini: %s\n", v->dev_ini);
    printf("dev_lo: %s\n", v->dev_lo);
    printf("netmask: %d\n", v->netmask);
    printf("broadcast: %d\n", v->broadcast);

    printf("vips: ");
    for (int i = 0; i < VIP_ALIAS_MAX; ++i)
    {
        printf("%d, ", v->vips[i]);
    }
    printf("\n");

    printf("count: %d\n", v->count);
}

