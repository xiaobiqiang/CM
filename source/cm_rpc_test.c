#include "cm_rpc_test.h"
#include "cm_log.h"

#define CM_RPC_TEST_SERVER_ON
#define CM_RPC_TEST_CLIENT_ON

//#undef CM_RPC_TEST_SERVER_ON
#undef CM_RPC_TEST_CLIENT_ON

sint32 cm_rpc_test_init()
{
	char *pAck = NULL;
	uint32 acklen = 0;
#ifdef CM_RPC_TEST_SERVER_ON
	(void)cm_rpc_register(CM_RPC_MSG_TYPE_TEST, cm_rpc_test_cbk_rpc_reg);
#endif
#ifdef CM_RPC_TEST_CLIENT_ON
	(void)cm_rpc_request(CM_RPC_MSG_TYPE_TEST, "192.168.127.134", "hello "
	"world", strlen("hello world")+1, (void **)&pAck, &acklen);
	CM_LOG_DEBUG(CM_LOG_MOD_RPC, "ackdata: %s, acklen: %u", pAck, acklen);
#endif
    return CM_OK;
}

sint32 cm_rpc_test_cbk_rpc_reg
(void *pData, uint32 len, void **ppAck, uint32 *pAckLen)
{
	void *pAck = CM_MALLOC(len);
	CM_MEM_CPY(pAck, pData, len);
	*pAckLen = len;
	*ppAck = pAck;
	return CM_OK;
}

