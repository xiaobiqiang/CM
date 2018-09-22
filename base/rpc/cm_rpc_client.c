#include "cm_rpc_client.h"
#include "cm_rpc_common.h"

sint32 cm_rpc_client_init()
{
	return CM_OK;
}

//return fd when successful, otherwise return CM_FAIL
static sint32 cm_rpc_client_connect_retry
(sint32 domain, sint32 type, const sint8 *ipaddr, uint32 ser_port, uint32 tmout)
{
	sint32 iRet;
	sint32 fd = -1;
	uint32 bin_addr = 0;
	struct sockaddr_in addr_in;

	iRet = inet_pton(domain, ipaddr, &bin_addr);
	if(iRet != 1)
	{
		CM_LOG_ERR(CM_LOG_MOD_RPC,
			"%s cvt to binary fail[%d]", ipaddr, iRet);
		return CM_FAIL;
	}
	addr_in.sin_family = domain;
	addr_in.sin_port = ser_port;
	addr_in.sin_addr.s_addr = bin_addr;

	for(sint32 numsec=1; numsec<=tmout; numsec<<1)
	{
		fd = socket(domain, type, 0);
		iRet = connect(fd, (struct sockaddr *)&addr_in, sizeof(addr_in));
		if(CM_OK == iRet)
		{
			return fd;
		}
		if(numsec <= tmout/2)
		{
			sleep(numsec);
		}
		close(fd);
	}
	return CM_FAIL;
}

//CM_FAIL:连接服务器失败，发送数据失败，当接受到的数据小于等于sizeof(cm_rpc_msg_info_t),
//分配空间失败等等
//CM_OK:成功接受到大于sizeof(cm_rpc_msg_info_t)的数据。
//*ppAck = pRecv->data，说明pRecv并没有释放掉，由上层释放。!!!!!!!!!!!
sint32 cm_rpc_client_request(
	uint32 type, const sint8 *ipaddr,
	void *pData, uint32 len,
	void **ppAck, uint32 *pAckLen
	)
{
	sint32 iRet;
	sint32 tcp_fd;
	uint32 recv_len = 0;
	cm_rpc_msg_info_t *pRecv = NULL;

	tcp_fd = cm_rpc_client_connect_retry(AF_INET, SOCK_STREAM,
		ipaddr, CM_RPC_SERVER_PORT, CM_RPC_RETRY_TMOUT);
	if(CM_FAIL == tcp_fd)
	{
		CM_LOG_ERR(CM_LOG_MOD_RPC, "connect %s tmout", ipaddr);
		return CM_FAIL;
	}

	iRet = cm_rpc_client_send_retry(tcp_fd, type, pData, len, CM_RPC_RETRY_TMOUT);
	if(CM_OK != iRet)
	{
		CM_LOG_ERR(CM_LOG_MOD_RPC, "send fail[%d]", iRet);
		return CM_FAIL;
	}

	iRet = cm_rpc_recv_try(tcp_fd, &pRecv, &recv_len);
	if(CM_OK != iRet)
	{
		CM_LOG_ERR(CM_LOG_MOD_RPC, "recv data fail[%d]", iRet);
		return CM_FAIL;
	}

	*ppAck = pRecv->data;
	*pAckLen = recv_len - sizeof(cm_rpc_msg_info_t);
	return CM_OK;
}

