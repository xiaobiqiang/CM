#ifndef BASE_CM_CMT_COMMON_H
#define BASE_CM_CMT_COMMON_H

#include "cm_defines.h"

#define CM_CMT_FREE_RPC_MSG(pCmt) \
    (CM_FREE((pCmt)-sizeof(cm_rpc_msg_info_t)))

typedef sint32(*cm_cmt_cbk_func_t)(void *pdata, uint32 len, void **ppAck, uint32 *pAckLen);

typedef enum
{
    CM_CMT_MSG_TYPE_BUTT
} cm_cmt_msg_type_e;

typedef struct
{
    uint32 type;
    cm_cmt_cbk_func_t cbk;
} cm_cmt_msg_cfg_t;

typedef struct
{
    uint32 from;
    uint32 to;
    uint32 type;
    uint32 result;  /* filled by cmt server */
    uint32 headlen;
    uint32 datalen;
    uint8 data[];
} cm_cmt_msg_info_t;

extern sint32 cm_cmt_new_cmt_msg
(uint32 type, uint32 to, void *pData, uint32 len, cm_cmt_msg_info_t **ppAck);

extern sint32 cm_cmt_request
(uint32 nid, uint32 type, void *pData, uint32 len, void **ppAck, uint32 *pAckLen);

#endif /* BASE_CM_CMT_COMMON_H */