#ifndef BASE_CM_RPC_CLIENT_H
#define BASE_CM_RPC_SERVER_H

#include "cm_rpc_common.h"

extern sint32 cm_rpc_client_init();

extern sint32 cm_rpc_client_request(
    uint32 type, const sint8 *ipaddr, uint32 tmout,
    void *pData, uint32 len,
    void **ppAck, uint32 *pAckLen);

#endif