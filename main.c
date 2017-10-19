#include "util.h"

#define VIP_IDX_MAP_SIZE    VIP_ALIAS_MAX
#define VIP_ARP_COUNT           5
#define VIP_ARP_INTERVAL        400000  /* micro-seconds */

int tbcm_vip_init(char *device, char *vip_str, char *netmask_str, char *broadcast_str);
int tbcm_vip_alias(char *vip_str);
int tbcm_vip_release(char *vip_str, tb_bool_t use_retention);

int get_unique_interface(const char *device, char *unique_device,
                                char *physical_device, char *loop_back,
                                char* remove_addr, tb_bool_t chk_validity,
                                tb_bool_t logging);
int vip_add_internal(char *unique_device, char *vip_str, int idx);
int tbcm_vip_check_validity(char *vip_str, tb_bool_t logging);

void get_user_input(char *prompt_message);

tbcm_vip_t vip_info;

int main(int argc, char const *argv[])
{
    char device[IF_NAMESIZE];
    char vip[IP_LEN];
    char netmask[IP_LEN];
    char broadcast[IP_LEN];

    char    out_line[1024];
    char    desc_net_if_stmt[1024];
    char    assign_priv_ipaddr_stmt[1024];

    memset(device, 0, sizeof(device));
    memset(vip, 0, sizeof(vip));
    memset(netmask, 0, sizeof(netmask));
    memset(broadcast, 0, sizeof(broadcast));

    strcpy(device, "eth0");
    strcpy(vip, "192.1.13.193");
    // 192.1.13.193 ohhyun 
    // 192.1.13.214 juhyun
    strcpy(netmask, "255.255.0.0");
    strcpy(broadcast, "192.1.255.255");

    vip_info.flag_init = false;

    printf("vip test started.\n");

    get_user_input("type something.");

//    if(shell_command("aws ec2 assign-private-ip-addresses --network-interface-id eni-00175a3d --private-ip-addresses 172.31.26.207"))
//        printf("ERROR at shell_command");

    if(shell_command_as_pipe_get_singleline(out_line, "ec2-metadata -o|awk '{print $2;}'")){
        exit(1);
    }

    printf("Single line: %s", out_line);

    get_user_input("type anything to quit");

    sprintf(desc_net_if_stmt, "aws ec2 describe-network-interfaces"
            " --query 'NetworkInterfaces[?PrivateIpAddress=="
            "`%s`].NetworkInterfaceId'", out_line);

    printf("\n%s\n", desc_net_if_stmt);

    if(shell_command_as_pipe_get_singleline(out_line, desc_net_if_stmt)){
        exit(1);
    }

    printf("Single line: %s", out_line);

    sprintf(assign_priv_ipaddr_stmt, "aws ec2 assign-private-ip-addresses"
            " --network-interface-id %s"
            " --private-ip-addresses %s", out_line, "172.31.29.77");

    if(shell_command(assign_priv_ipaddr_stmt)){
        fprintf(stderr, "executing \"%s\" FAIL. errno=%d\n", errno);
        exit(1);
    }



    /*tbcm_vip_init(device, vip, netmask, broadcast);
    print_vip_info(&vip_info);

    get_user_input("type anything to alias ip");

    tbcm_vip_alias(vip);

    get_user_input("type anything to release ip");

    tbcm_vip_release(vip, false);

    get_user_input("type anything to end this test");*/

    printf("\nvip test ended\n");
    return 0;
}

int
tbcm_vip_init( char *device, char *vip_str,
               char *netmask_str, char *broadcast_str)
{
    uint32_t vip,   netmask,  broadcast;    /* host byte order */
    uint32_t vip_n, netmask_n;              /* network byte order */
    int idx;

    if (vip_info.flag_init == true) {
        printf("VIP was already initialized.");
        return FAILURE;
    }

    if (!(getuid() == 0 && geteuid() == 0)) {
        printf("Failed to initializ vip. "
                       "UID or EUID must be 0 (root)");
        return FAILURE;
    }

    printf("initializing VIP info : [%s:%s:%s:%s]",
           device, vip_str, netmask_str, broadcast_str );

    /* network mask와 broadcast address가 주어지지 않으면 default로
     * IP ver4 address의 class 에 따른 값을 정한다.
     */
    inet_pton(AF_INET, vip_str, &vip);
    if (!netmask_str || !netmask_str[0]) {
        vip_n = htonl(vip);
        if (IN_CLASSA(vip_n))
            netmask_n = IN_CLASSA_NET;
        else if (IN_CLASSB(vip_n))
            netmask_n = IN_CLASSB_NET;
        else if (IN_CLASSC(vip_n))
            netmask_n = IN_CLASSC_NET;
        else {
            printf("IP %s invalid", vip_str);
            return FAILURE;
        }
        netmask = ntohl(netmask_n);
    } else {
        inet_pton(AF_INET, netmask_str, &netmask);
    }

    if (!broadcast_str || !broadcast_str[0])
        broadcast = (vip & netmask) | (0xffffffff & ~netmask);
    else
        inet_pton(AF_INET, broadcast_str, &broadcast );

    vip_info.netmask    = netmask;
    vip_info.broadcast  = broadcast;


    /* device로부터 aliasing을 위한 unique name, physical name,
     * loop back device정보를 얻는다. */
    idx = get_unique_interface( device, vip_info.dev_ini,
                                vip_info.dev_pub, vip_info.dev_lo,
                                vip_str, false, true );
    if (idx < 0) {
        printf("can't get interface info from %s", device);
        return FAILURE;
    }

    vip_info.flag_init = true;
    vip_info.count     = 0;

    printf("initializing VIP success : [%s:%s:%s:%x:%x]",
           vip_info.dev_pub, vip_info.dev_ini, vip_info.dev_lo,
           vip_info.netmask, vip_info.broadcast );

    return SUCCESS;
}

int  tbcm_vip_alias(char *vip_str)
{
    int rc;
    int idx;
    char unique_device[IF_NAMESIZE];
    uint32_t vip;

    if( vip_info.flag_init != true ) {
        printf("VIP was not initialized yet.\n");
        return FAILURE;
    }

    /* unique device name을 얻는다. */
    if ( (idx = get_unique_interface(vip_info.dev_pub,
                                     unique_device, NULL, NULL,
                                     vip_str, false, true)) < 0) {
        printf("can't find unique device name for %s\n", vip_info.dev_pub);
        return FAILURE;
    }

    inet_pton(AF_INET, vip_str, &vip);

    /* vip_info에 먼저 세팅해 준 후에 CM guard에게 메시지를 날려준다. */
    vip_info.vips[idx]  = vip;
    vip_info.count     += 1;

     /* CM guard에게 먼저 VIP 정보를 보내준 후에 실제 vip alias 시도 */
     rc = vip_add_internal(unique_device, vip_str, idx);
     if ( rc == SUCCESS)
         printf("VIP %s alias success.\n", vip_str);
     else {
         printf("VIP %s alias failure.\n", vip_str);
         vip_info.vips[idx] = 0;
         vip_info.count    -= 1;
     }

    return rc;
}

int
tbcm_vip_release(char *vip_str, tb_bool_t use_retention)
{
    int  i;
    uint32_t vip = 0;
    char unique_device[IF_NAMESIZE];

    if (vip_info.flag_init != true ) {
        printf("VIP was not initialized yet.\n");
        return FAILURE;
    }

    if (vip_info.count <= 0 ) {
        printf("no VIP aliased.\n" );
        return FAILURE;
    }

    inet_pton(AF_INET, vip_str, &vip);  /* FIXME....no error handling? */

    /* Step I)  VIP를 alias한적이 있는지 확인하고, index를 찾는다. */
    for (i = 0; i < VIP_ALIAS_MAX; i++) {
        if (vip_info.vips[i] && vip_info.vips[i] == vip) {
            sprintf(unique_device, "%s:%d", vip_info.dev_pub, i);
            break;
        }
    }

    if (i == VIP_ALIAS_MAX) {
        printf("VIP %s was not found.", vip_str);
        return FAILURE;
    }

    /* Step II) VIP를 NIC에서 제거한다. */
    if (tbcm_vip_check_validity(vip_str, false) == SUCCESS) {
        if (remove_ip_from_nic(unique_device, vip) < 0) {
            printf("VIP %s release failure.\n", vip_str);
            return FAILURE;
        }
    }
    else {
        if (remove_ip_from_nic(unique_device, vip) < 0)
            printf("VIP %s was removed already.\n", vip_str);
    }

    vip_info.vips[i] = 0;
    vip_info.count  -= 1;

    printf("VIP %s release success.\n", vip_str);

    return SUCCESS;
}

int get_unique_interface(const char *device, char *unique_device,
                         char *physical_device, char *loop_back,
                         char* remove_addr, tb_bool_t chk_validity,
                         tb_bool_t logging)
{
    struct ifaddrs *ifaddr = NULL;
    struct ifaddrs *ifa;
    int family, s;
    char addr_tmp[NI_MAXHOST];
    uint32_t addr_tmp_n;
    char org_dev[IF_NAMESIZE];
    char dev_name_tmp[IF_NAMESIZE];
    char *cptr;
    int idx;
    char idx_map[VIP_IDX_MAP_SIZE];
    tb_bool_t found = false;
    int retval = FAILURE;

    memset(idx_map, 0, VIP_IDX_MAP_SIZE);
    idx_map[0] = 'O';

    strncpy(org_dev, device, IF_NAMESIZE);
    if (physical_device)
        strncpy(physical_device, org_dev, IF_NAMESIZE);

    if (getifaddrs(&ifaddr) == -1) {
        if (logging)
            printf("getifaddrs() failed: %s",
                   strerror(errno));
        goto release_out;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        if (family != AF_INET)
            continue;       /* IPv4 address만 처리 */

        if (strncmp(ifa->ifa_name, org_dev, strlen(org_dev)) != 0)
            continue;

        if (ifa->ifa_flags & IFF_LOOPBACK) {
            if (logging)
                printf(
                        "Loopback device %s can't be used for VIP aliasing",
                        ifa->ifa_name);
            goto release_out;
        }

        found = true;

        s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), addr_tmp,
                        NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        strcpy(dev_name_tmp, ifa->ifa_name);

        if (s != 0){
            if (s == EAI_FAMILY)
                continue;

            if (logging)
                printf("getnameinfo() failed: %s",
                       gai_strerror(s));
            goto release_out;
        }

        if (remove_addr != NULL &&
            strcmp(remove_addr, addr_tmp) == 0) {
            if (logging)
                printf("%s has aliased IP %s",
                       dev_name_tmp, addr_tmp );

            if (chk_validity == true) {
                retval = SUCCESS;
                goto release_out;
            }

            if (!vip_info.flag_init) {
                if (logging)
                    printf("Duplicated VIP %s", remove_addr);
                printf("Duplicated VIP %s. Check TIP file.\n", remove_addr);
                freeifaddrs(ifaddr);
                exit(-1);
            }

            inet_pton(AF_INET, addr_tmp, &addr_tmp_n);
            if (remove_ip_from_nic(dev_name_tmp, addr_tmp_n) != FAILURE)
                continue;
        }

        if ((cptr = strchr(ifa->ifa_name, ':')) == NULL)
            continue;

        idx = (int)strtol((const char *)cptr + 1, (char **)NULL, 10);

        if (idx > 0 && idx < VIP_IDX_MAP_SIZE)
            idx_map[idx] = 'O';
    }

    if (chk_validity)
        goto release_out;

    if (found) {
        for (idx = 0; idx < VIP_IDX_MAP_SIZE; ++idx) {
            if (!idx_map[idx]) {
                sprintf(unique_device, "%s:%d", org_dev, idx);
                retval = idx;
                goto release_out;
            }
        }
    }

    release_out:
    if (ifaddr != NULL)
        freeifaddrs(ifaddr);
    return retval;
}

int vip_add_internal(char *unique_device, char *vip_str, int idx)
{
    uint32_t vip;

    inet_pton(AF_INET, vip_str, &vip);

    /* Step II) VIP를 NIC에 aliasing한다. */
    if (add_ip_to_nic(unique_device, vip,
                      vip_info.netmask, vip_info.broadcast) != SUCCESS ) {
        printf("IP %s aliasing failure\n", vip_str);
        return FAILURE;
    }

    /* Step III) Gratuitous ARP를 날린다. */
    if( send_gratuitous_arp(vip_info.dev_pub, vip,
                            VIP_ARP_COUNT, VIP_ARP_INTERVAL) != SUCCESS ) {
        remove_ip_from_nic(unique_device, vip);
        return FAILURE;
    }

    return SUCCESS;
}

int tbcm_vip_check_validity(char *vip_str, tb_bool_t logging)
{
    int idx;

    if (vip_info.flag_init != true ) {
        if (logging)
            printf("VIP was not initialized yet.\n");
        return FAILURE;
    }

    if ( (idx = get_unique_interface(vip_info.dev_pub, NULL, NULL, NULL,
                                     vip_str, true, logging)) < 0 ) {
        if (logging)
            printf("can't find VIP %s at %s\n",
                      vip_str, vip_info.dev_pub);
        return FAILURE;
    }

    return SUCCESS;
}