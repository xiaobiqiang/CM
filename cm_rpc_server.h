#ifndef BASE_CM_RPC_SERVER_H
#define BASE_CM_RPC_SERVER_H

#include "cm_rpc_common.h"

extern sint32 cm_rpc_server_init();

extern sint32 cm_rpc_server_reg(uint32 type, cm_rpc_server_cbk_func_t cbk);

#endif