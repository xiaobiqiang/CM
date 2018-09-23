#include "cm_cmt_common.h"
#include "cm_cmt_server.h"
#include "cm_rpc.h"
#include "cm_log.h"
/*****************************************************************************************************
 *子域内的节点可以直接互相通信
 *子域主之间可以直接互相通信
 *子域主与其他子域内不是子域主的节点通信需要经过子域主节点转发。(一次转发)
 *子域内不是子域主的节点于另一个子域内不是子域主的节点通信需要经过两个子域主的转发。(二次转发)
 *******************************************************************************************************/
sint32 cm_cmt_cbk_rpc_reg(void *pData, uint32 len,
                          void **ppAck, uint32 *pAckLen)
{
    sint32 iRet;
    cm_cmt_msg_cfg_t *pCfg = NULL;
    cm_cmt_msg_info_t *pinfo = pData;
    uint32 myid = cm_node_get_local_nid();

    if((NULL == pData) || (len <= sizeof(cm_cmt_msg_info_t)))
    {
        CM_LOG_ERR(CM_LOG_MOD_CMT, "data_len:%u", len);
        *ppAck = NULL;
        *pAckLen = 0;
        return CM_FAIL;
    }

    //发送至的节点就是自身节点，那么直接处理
    if(pinfo->to == myid)
    {
        pCfg = g_cm_cmt_msg_cfgs[pinfo->type];
        iRet = pCfg->cbk(pinfo->data, pinfo->datalen, ppAck, pAckLen);
        if(CM_OK != iRet)
        {
            CM_LOG_ERR(CM_LOG_MOD_CMT, "cbk exec fail[%d]", iRet);
            *ppAck = NULL;
            *pAckLen = 0;
            rerturn CM_FAIL;
        }
        return CM_OK;
    }

    //不是自身节点，自身是子域主，那么转发到指定的节点
    if(myid == cm_node_get_subdomain_master())
    {
        pinfo->from = cm_node_get_local_nid();
        return cm_cmt_request(pinfo->to, pinfo->type, pinfo, len, ppAck, pAckLen);
    }

    //不是子域主，也不是目的节点，只能返回失败,很难出现
    CM_LOG_ERR(CM_LOG_MOD_CMT, "local id:%u, submaster:%u",
               myid, cm_node_get_subdomain_master());
    *ppAck = NULL;
    *pAckLen = 0;
    return CM_FAIL;
}


sint32 cm_cmt_server_init()
{
    sint32 iRet;
    iRet = cm_rpc_register(CM_RPC_MSG_TYPE_CMT, cm_cmt_cbk_rpc_reg);
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_LOG_MOD_CMT, "register cmt rpc callback fail[%d]", iRet);
        return CM_FAIL;
    }
}

