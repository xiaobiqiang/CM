#ifndef BASE_CM_RPC_TEST_H
#define BASE_CM_RPC_TEST_H

#include "cm_defines.h"
#include "cm_rpc.h"

extern sint32 cm_rpc_test_cbk_rpc_reg
(void *pData, uint32 len, void **ppAck, uint32 *pAckLen);


extern sint32 cm_rpc_test_init();

#endif
