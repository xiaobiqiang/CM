#ifndef BASE_CM_RPC_COMMON_H
#define BASE_CM_RPC_COMMON_H

#include "cm_defines.h"

#define CM_RPC_RETRY_TMOUT	2   //有一次重试的机会。
#define CM_RPC_MAX_LEN_MSG	CM_STRING_1K
#define CM_RPC_SERVER_PORT 8800

typedef sint32(*cm_rpc_server_cbk_func_t)
(void *pData, uint32 len, void **ppAck, uint32 *pAckLen);

typedef enum
{
	CM_RPC_MSG_TEST = 0,
    CM_RPC_MSG_CMT,
    CM_RPC_MSG_BUTT
} cm_rpc_msg_type_e;

typedef struct
{
    uint32 tcp_fd;
    uint32 msg_type;
    uint32 headlen;
    uint32 datalen;
    sint32 result;  /* filled by rpc server */
    uint8 data[];
} cm_rpc_msg_info_t;

extern sint32 cm_rpc_send_rpc_tmout(uint32 fd, void *data, uint32 len, uint32 tmout);

extern sint32 cm_rpc_new_rpc_msg
(uint32 fd, uint32 type, uint32 len, void *data, cm_rpc_msg_info_t **ppAck);

extern sint32 cm_rpc_send_tmout
(sint32 fd, uint32 type, void *rpc_data, uint32 len, uint32 tmout);

extern sint32 cm_rpc_recv_tmout(sint32 fd, uint32 tmout, void **ppAck, uint32 *pAckLen);

#endif