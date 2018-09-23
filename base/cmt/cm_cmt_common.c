#include "cm_cmt_common.h"
#include "cm_node.h"

static sint32 cm_cmt_request_comm
(const sint8 *ipaddr, cm_cmt_msg_info_t *pinfo, void **ppAck, uint32 *pAckLen);

static sint32 cm_cmt_request_nid
(uint32 nid, cm_cmt_msg_info_t *pinfo, void **ppAck, uint32 *pAckLen);

static sint32 cm_cmt_request_ano_submaster
(uint32 subdomain, cm_cmt_msg_info_t *pinfo, void **ppAck, uint32 *pAckLen);

static sint32 cm_cmt_request_submaster
(cm_cmt_msg_info_t *pinfo, void **ppAck, uint32 *pAckLen);



//result默认为CM_OK
sint32 cm_cmt_new_cmt_msg
(uint32 type, uint32 to, void *pData, uint32 len, cm_cmt_msg_info_t **ppAck)
{
    cm_cmt_msg_info_t *pinfo = NULL;
    pinfo = CM_MALLOC(sizeof(cm_cmt_msg_info_t) + len);
    if(NULL == pinfo)
    {
        CM_LOG_ERR(CM_LOG_MOD_CMT, "malloc fail");
        return CM_FAIL;
    }

    pinfo->from = cm_node_get_local_nid();
    pinfo->to = to;
    pinfo->result = CM_OK;
    pinfo->type = type;
    pinfo->headlen = sizeof(cm_cmt_msg_info_t);
    pinfo->datalen = len;
    if(NULL != pData)
    {
        CM_MEM_CPY(pinfo + pinfo->headlen, pData, len);
    }
    *ppAck = pinfo;
    return CM_OK;
}

sint32 cm_cmt_request(uint32 nid, uint32 type,
                      void *pData, uint32 len,
                      void **ppAck, uint32 *pAckLen)
{
    sint32 iRet;
    cm_cmt_msg_cfg_t *pCfg = NULL;
    cm_cmt_msg_info_t *pinfo = NULL;
    uint32 myid = cm_node_get_local_nid();
    //nid的子域ID
    uint32 ano_subdomain = CM_NODE_SUBDOMAIN_ID_NONE;

    if(type >= CM_CMT_MSG_TYPE_BUTT)
    {
        CM_LOG_ERR(CM_LOG_MOD_CMT, "not supported msg type[%u]", type);
        return CM_FAIL;
    }

    //目的节点就是本节点，直接处理cbk
    if(nid == myid)
    {
        pCfg = g_cm_cmt_msg_cfgs[type];
        iRet = pCfg->cbk(pData, len, ppAck, pAckLen);
        if(CM_OK != iRet)
        {
            CM_LOG_ERR(CM_LOG_MOD_CMT, "cbk exec fail[%d]", iRet);
            return iRet;
        }
        return CM_OK;
    }

    iRet = cm_cmt_new_cmt_msg(type, nid, pData, len, &pinfo);
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_LOG_MOD_CMT, "get new cmt msg fail[%d]", iRet);
        return CM_FAIL;
    }

    iRet = cm_node_check_nid_in_subdomain(nid);
    if(CM_OK == iRet)
    {
        //如果nid和当前节点就在一个子域内，那么直接发送到nid节点即可
        return cm_cmt_request_nid(nid, pinfo, ppAck, pAckLen);
    }

    //位于不同子域,需要经过子域主转发
    //如果自身是子域主，那么转发到另外一个子域主
    if(myid == cm_node_get_subdomain_master())
    {
        ano_subdomain = cm_node_get_submaster_by_nid(nid)
                        return cm_cmt_request_ano_submaster
                               (ano_subdomain, pinfo, ppAck, pAckLen);
    }

    //如果自身节点还不是子域主，那么就发送给子域主
    return cm_cmt_request_submaster(pinfo, ppAck, pAckLen);
}

static sint32 cm_cmt_request_comm
(const sint8 *ipaddr, cm_cmt_msg_info_t *pinfo, void **ppAck, uint32 *pAckLen)
{
    sint32 iRet;
    void *pAck = NULL;
    uint32 acklen = 0;
    uint32 rpcAcklen = 0;
    cm_cmt_msg_info_t *pRpcAck = NULL;

    iRet = cm_rpc_request(CM_RPC_MSG_TYPE_CMT, ipaddr,
                          pinfo, pinfo->headlen + pinfo->datalen,
                          &pRpcAck, &rpcAcklen);
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_LOG_MOD_CMT, "rpc request %u fail[%d]", nid, iRet);
        return CM_FAIL;
    }

    pAck = CM_MALLOC(pRpcAck->datalen);
    if(NULL == pAck)
    {
        CM_LOG_ERR(CM_LOG_MOD_CMT, "malloc fail");
        CM_CMT_FREE_RPC_MSG(pRpcAck);
        return CM_FAIL;
    }

    CM_MEM_CPY(pAck, pRpcAck->data, pRpcAck->datalen);
    *ppAck = pAck;
    *pAckLen = pRpcAck->datalen;
    CM_CMT_FREE_RPC_MSG(pRpcAck);
    return CM_OK;
}

//不会做nid是否是本子域内节点检查
static sint32 cm_cmt_request_nid
(uint32 nid, cm_cmt_msg_info_t *pinfo, void **ppAck, uint32 *pAckLen)
{
    sint32 iRet;
    cm_node_info_t nodeinfo;

    iRet = cm_node_getinfo_by_nid(nid, &nodeinfo);
    if(CM_OK != = iRet)
    {
        CM_LOG_ERR(CM_LOG_MOD_CMT, "subdomainId:%u, ano_subdomainId:%u",
                   cm_node_get_subdomain_id(), cm_node_get_subdomain_by_nid(nid));
        return CM_FAIL;
    }

    return cm_cmt_request_comm
           (nodeinfo.ipaddr, pinfo, ppAck, pAckLen);
}

//不会做该节点是否是本子域内子域主检查
static sint32 cm_cmt_request_ano_submaster
(uint32 subdomain, cm_cmt_msg_info_t *pinfo, void **ppAck, uint32 *pAckLen)
{
    sint32 iRet;
    cm_node_info_t nodeinfo;

    iRet = cm_node_get_submaster_info_by_subdomainId(subdomain, &nodeinfo);
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_LOG_MOD_CMT, "get ano_submaster info fail[%d]", iRet);
        return CM_FAIL;
    }

    return cm_cmt_request_comm
           (nodeinfo.ipaddr, pinfo, ppAck, pAckLen);
}

static sint32 cm_cmt_request_submaster
(cm_cmt_msg_info_t *pinfo, void **ppAck, uint32 *pAckLen)
{
    sint32 iRet;
    cm_node_info_t nodeinfo;

    iRet = cm_node_get_submaster_info(&nodeinfo);
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_LOG_MOD_CMT, "get submaster info fail[%d]", iRet);
        return CM_FAIL;
    }

    return cm_cmt_request_comm
           (nodeinfo.ipaddr, pinfo, ppAck, pAckLen);


}

