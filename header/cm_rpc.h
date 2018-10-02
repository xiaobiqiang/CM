#ifndef BASE_CM_RPC_H
#define BASE_CM_RPC_H

#include "cm_rpc_common.h"

extern sint32 cm_rpc_init();

extern sint32 cm_rpc_request(
    uint32 type, const sint8 *ipaddr,
    void *pdata, uint32 len,
    void **ppAck, uint32 *pAckLen);

extern sint32 cm_rpc_register
(uint32 type, cm_rpc_server_cbk_func_t cbk);

#endif