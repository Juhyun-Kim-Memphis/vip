//
// Created by juhyun on 17. 10. 12.
//

#ifndef VIP_TBMSG_HPP
#define VIP_TBMSG_HPP

#include "util.h"

#define SIZEOF_VOID_P 8

enum tbmsg_type_e {
    TBMSG_TBCM_VIP_INFO
};

#ifndef _TBMSG_T
#define _TBMSG_T
typedef struct tbmsg_s tbmsg_t;
#endif  /* _TBMSG_T */
struct tbmsg_s {
    char *raw;
    int str_auto_free;
#if SIZEOF_VOID_P == 8
    /* 64bit 기계의 경우 dummy 변수가 없으면 아래 body_size 필드와 tsn 필드
     * 사이에 padding이 끼어들어 아랫쪽에 있는 TBMSG_HDR_SIZE 매크로 구한 헤더의
     * 크기가 실제 헤더의 크기와 다르게 된다.
     * XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     * XXX WARNING: 추후에 이 구조체에 필드가 추가되면 헤더에 들어갈 필드들
     * XXX 사이에 padding이 끼어들지 않도록 적절히 이 dummy를 넣고 빼주는 작업을
     * XXX 해주는 것을 잊지 말아야 한다.
     * XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     */
    int dummy;
#endif

    /* 메시지 헤더에 포함될 필드들 */
    /* WARNING: filed추가하거나 변경시 serialize/deserialize함수도 고쳐야함 */
    int type;
    unsigned int body_size;
    uint64_t tsn;
};

#define TBMSG_VIP_LIST__VIP (TBMSG_HDR_SIZE + 0)
#define TBMSG_VIP_LIST_SZ (TBMSG_HDR_SIZE + 4)

typedef struct tbmsg_struct_vip_list_s tbmsg_struct_vip_list_t;
struct tbmsg_struct_vip_list_s {
    int vip;
};

void tbmsg_struct_vip_list_set(tbmsg_struct_vip_list_t *tbmsg, int vip);


#define TBMSG_TBCM_VIP_INFO__FLAG_INIT (TBMSG_HDR_SIZE + 0)
#define TBMSG_TBCM_VIP_INFO__DEV_PUB (TBMSG_HDR_SIZE + 4)

typedef struct tbmsg_tbcm_vip_info_s tbmsg_tbcm_vip_info_t;
struct tbmsg_tbcm_vip_info_s {
    tbmsg_t base;

    int flag_init;
    struct {
        int len;
        char *data;
    } dev_pub;
    struct {
        int len;
        char *data;
    } dev_ini;
    struct {
        int len;
        char *data;
    } dev_lo;
    int netmask;
    int broadcast;
    struct {
        int array_cnt;
        tbmsg_struct_vip_list_t *data;
    } vip_list;
};

void tbmsg_tbcm_vip_info_set(tbmsg_tbcm_vip_info_t *tbmsg,
                             int flag_init,
                             char *dev_pub,
                             char *dev_ini,
                             char *dev_lo,
                             int netmask,
                             int broadcast,
                             int vip_list_array_cnt,
                             tbmsg_struct_vip_list_t *vip_list);
#endif //VIP_TBMSG_HPP
