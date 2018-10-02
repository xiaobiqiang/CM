#include "cm_rpc_common.h"
#include "cm_sys.h"
#include "cm_log.h"

sint32 cm_rpc_send_rpc_tmout(uint32 fd, void *data, uint32 len, uint32 tmout)
{
    sint32 iRet;
	struct timeval timeout = 
	{
		.tv_sec = tmout,
		.tv_usec = 0
	};
	iRet = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	if (iRet != CM_OK)
	{
		CM_LOG_ERR(CM_MOD_RPC, "set send timeout fail[%d]", iRet);
		return CM_FAIL;
	}
	iRet = send(fd, data, len, 0);
	if (iRet != len)
	{
		CM_LOG_ERR(CM_MOD_RPC, "data_len: %u, send_len: %d", len, iRet);
		return CM_FAIL;
	}
    return CM_OK;
}

//result默认为CM_OK
sint32 cm_rpc_new_rpc_msg
(uint32 fd, uint32 type, uint32 len, void *data, cm_rpc_msg_info_t **ppAck)
{
    sint32 iRet;
    cm_rpc_msg_info_t *pinfo = CM_MALLOC(sizeof(cm_rpc_msg_info_t) + len);
    if(NULL == pinfo)
    {
        CM_LOG_ERR(CM_MOD_RPC, "malloc fail");
        return CM_FAIL;
    }
    pinfo->tcp_fd = fd;
    pinfo->msg_type = type;
    pinfo->headlen = sizeof(cm_rpc_msg_info_t);
    pinfo->datalen = len;
    pinfo->result = CM_OK;
    CM_MEM_CPY(pinfo->data, data, len);
    *ppAck = pinfo;
    return CM_OK;
}

//发送数据段是rpc_data的cm_rpc_msg_info_t到套接字fd。
sint32 cm_rpc_send_tmout
(sint32 fd, uint32 type, void *rpc_data, uint32 len, uint32 tmout)
{
    sint32 iRet;
    sint32 snd_len = 0;
    cm_rpc_msg_info_t *pSnd = NULL;

    iRet = cm_rpc_new_rpc_msg(fd, type, len, rpc_data, &pSnd);
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_MOD_RPC, "create new rpc msg fail[%d]", iRet);
        return CM_FAIL;
    }
    iRet = cm_rpc_send_rpc_tmout(fd, pSnd, pSnd->headlen + pSnd->datalen, tmout);
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_MOD_RPC, "send less data");
        CM_FREE(pSnd);
        return CM_FAIL;
    }
    CM_FREE(pSnd);
    return CM_OK;
}

//接收socket为fd的客户端发送过来的消息，并把数据拷贝到*ppAck的地址上。
//不再变动
sint32 cm_rpc_recv_tmout(sint32 fd, uint32 tmout, void **ppAck, uint32 *pAckLen)
{
	sint32 iRet;
    sint32 recv_len = 0;
    uint32 ackLen = 0;
    void *pAckData = NULL;
    cm_rpc_msg_info_t *pRecv = NULL;
	struct timeval timeout =
	{
		.tv_sec = tmout,
		.tv_usec = 0
	};

    pRecv = CM_MALLOC(CM_RPC_MAX_LEN_MSG);
    if(NULL == pRecv)
    {
        CM_LOG_ERR(CM_MOD_RPC, "malloc fail");
        return CM_FAIL;
    }
	iRet = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    recv_len = recv(fd, pRecv, CM_RPC_MAX_LEN_MSG, 0);
    CM_LOG_DEBUG(CM_MOD_RPC, "recv_len:%d", recv_len);
    if(recv_len < sizeof(cm_rpc_msg_info_t))	//no data.
    {
        CM_LOG_ERR(CM_MOD_RPC, "recv_len:%d", recv_len);
        CM_FREE(pRecv);
        return CM_FAIL;
    }

    pAckData = CM_MALLOC(recv_len);
    if(NULL == pAckData)
    {
        CM_LOG_ERR(CM_MOD_RPC, "malloc fail");
        CM_FREE(pRecv);
        return CM_FAIL;
    }
    CM_MEM_CPY(pAckData, pRecv, recv_len);
    *ppAck = pAckData;
    *pAckLen = recv_len;
    CM_FREE(pRecv);
    return CM_OK;
}





