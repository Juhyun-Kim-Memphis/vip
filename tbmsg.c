//
// Created by juhyun on 17. 10. 12.
//

#include <assert.h>
#include "tbmsg.h"

void tbmsg_struct_vip_list_set(tbmsg_struct_vip_list_t *tbmsg, int vip)
{
    assert(tbmsg != NULL);

    tbmsg->vip = vip;
}


void
tbmsg_tbcm_vip_info_set(tbmsg_tbcm_vip_info_t *tbmsg,
                        int flag_init,
                        char *dev_pub,
                        char *dev_ini,
                        char *dev_lo,
                        int netmask,
                        int broadcast,
                        int vip_list_array_cnt,
                        tbmsg_struct_vip_list_t *vip_list)
{
    assert(tbmsg != NULL);

    tbmsg->base.type = TBMSG_TBCM_VIP_INFO;
    tbmsg->base.body_size = 0;
    tbmsg->base.str_auto_free = false;
    tbmsg->base.tsn = 0;

    tbmsg->flag_init = flag_init;
    if (dev_pub == NULL) {
        tbmsg->dev_pub.len = 0;
        tbmsg->dev_pub.data = NULL;
    }
    else {
        tbmsg->dev_pub.len = (int)strlen(dev_pub);
        tbmsg->dev_pub.data = dev_pub;
    }
    if (dev_ini == NULL) {
        tbmsg->dev_ini.len = 0;
        tbmsg->dev_ini.data = NULL;
    }
    else {
        tbmsg->dev_ini.len = (int)strlen(dev_ini);
        tbmsg->dev_ini.data = dev_ini;
    }
    if (dev_lo == NULL) {
        tbmsg->dev_lo.len = 0;
        tbmsg->dev_lo.data = NULL;
    }
    else {
        tbmsg->dev_lo.len = (int)strlen(dev_lo);
        tbmsg->dev_lo.data = dev_lo;
    }
    tbmsg->netmask = netmask;
    tbmsg->broadcast = broadcast;
    tbmsg->vip_list.array_cnt = vip_list_array_cnt;
    tbmsg->vip_list.data = vip_list;
}