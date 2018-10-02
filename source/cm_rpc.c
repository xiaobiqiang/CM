#include "cm_rpc_client.h"
#include "cm_rpc_server.h"
#include "cm_log.h"

#define CM_RPC_SERVER_ON 
#undef CM_RPC_SERVER_ON


sint32 cm_rpc_init()
{
    sint32 iRet;
#ifdef CM_RPC_SERVER_ON
    iRet = cm_rpc_server_init();
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_MOD_RPC, "rpc server init fail[%d]", iRet);
        return CM_FAIL;
    }
#endif

    iRet = cm_rpc_client_init();
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_MOD_RPC, "rpc client init fail[%d]", iRet);
        return CM_FAIL;
    }

    return CM_OK;
}

sint32 cm_rpc_request(
    uint32 type, const sint8 *ipaddr, uint32 tmout,
    void *pdata, uint32 len,
    void **ppAck, uint32 *pAckLen)
{
    sint32 iRet;
    iRet = cm_rpc_client_request
           (type, ipaddr, tmout, pdata, len, ppAck, pAckLen);
    if(iRet != CM_OK)
    {
        CM_LOG_ERR(CM_MOD_RPC, "request %s fail[%d]", ipaddr, iRet);
        return iRet;
    }
    return CM_OK;
}

sint32 cm_rpc_register
(uint32 type, cm_rpc_server_cbk_func_t cbk)
{
    sint32 iRet;
    iRet = cm_rpc_server_reg(type, cbk);
    if(iRet != CM_OK)
    {
        CM_LOG_ERR(CM_MOD_RPC, "register config fail");
        return CM_FAIL;
    }
    return CM_OK;
}



