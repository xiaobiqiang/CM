#include "cm_rpc_server.h"
#include "cm_rpc_common.h"
#include <sys/socket.h>
#include "cm_log.h"
#include "cm_queue.h"

#define CM_RPC_SERVER_NUM_THREAD 2
#define CM_RPC_SERVER_NUM_REQ	64

/* every type has its own server. */
typedef struct
{
    uint32 msg_type;
    cm_mutex_t lock;
    cm_rpc_server_cbk_func_t cbk;
    cm_queue_t *wait_responce;
} cm_rpc_server_cfg_t;

static sint32 g_cm_rpc_server_fd = -1;
static cm_mutex_t g_cm_rpc_server_mutex;
static cm_rpc_server_cfg_t g_cm_rpc_server_cfg[CM_RPC_MSG_BUTT];

static sint32 cm_rpc_send_fail_rpc_msg
(uint32 fd, uint32 type, sint32 iRet, uint32 tmout, void *pdata, uint32 len);

//建立连接后2秒钟之内收不到数据就不接收了。
static void * cm_rpc_server_cbk_accept_thread(void *pArg)
{
    sint32 iRet;
    sint32 ser_fd = *(sint32 *)pArg;
    sint32 cli_fd = -1;
    uint32 recv_len = 0;
    cm_rpc_msg_info_t *pMsg = NULL;
    cm_rpc_server_cfg_t *pCfg = NULL;

    while(1)
    {
        cli_fd = accept(ser_fd, NULL, NULL);
        if(-1 == cli_fd)
        {
            CM_LOG_ERR(CM_MOD_RPC, "accept fail[%d]", cli_fd);
            continue;
        }
		//两秒钟之内收不到数据就不收了。
        iRet = cm_rpc_recv_tmout(cli_fd, 2, &pMsg, &recv_len);
        if(CM_OK != iRet)
        {
            CM_LOG_ERR(CM_MOD_RPC, "recv_snd_len: %u", recv_len);
			//两秒钟之内发不出去就不发了。
            cm_rpc_send_fail_rpc_msg(cli_fd, 0, CM_FAIL, 2, NULL, 0);
            continue;
        }
        //在上层cmt等初始化就会注册处理函数，一般很难为NULL，所以不加全局锁来判断了。
        pCfg = &g_cm_rpc_server_cfg[pMsg->msg_type];
        if(NULL == pCfg->wait_responce)
        {
            CM_LOG_ERR(CM_MOD_RPC, "type of %u not register yet", pMsg->msg_type);
            (void)cm_rpc_send_fail_rpc_msg(cli_fd, pMsg->msg_type, CM_FAIL, 2, NULL, 0);
            CM_FREE(pMsg);
            continue;
        }

        pMsg->tcp_fd = cli_fd;
        iRet = cm_queue_add(pCfg->wait_responce, pMsg, recv_len);
        if(CM_OK != iRet)
        {
            CM_LOG_ERR(CM_MOD_RPC, "queue add fail[%d]", iRet);
            cm_rpc_send_fail_rpc_msg(cli_fd, pMsg->msg_type, CM_FAIL, 2, NULL, 0);
            CM_FREE(pMsg);
        }
    }
    close(ser_fd);
    return NULL;
}

static sint32 cm_rpc_server_bind_retry
(sint32 fd, const struct sockaddr *addr, uint32 len, uint32 tmout)
{
	sint32 iRet;

	for (int numsec = 1; numsec <= tmout; numsec <<= tmout)
	{
		iRet = bind(fd, addr, len);
		if (CM_OK == iRet)
		{
			return CM_OK;
		}
		if (numsec <= tmout / 2)
		{
			sleep(numsec);
		}
	}
	return CM_FAIL;
}

static sint32 cm_rpc_server_bind(sint32 fd, uint32 tmout)
{
    sint32 iRet;
    sint8 ipaddr[CM_IP_LEN] = {0};
    uint32 bin_addr = 0;
    uint32 reuseaddr = 1;
    struct sockaddr_in addr_in;

    iRet = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(uint32));
    if(iRet != CM_OK)
    {
        CM_LOG_ERR(CM_MOD_RPC, "set reuse addr fail");
        return CM_FAIL;
    }

    iRet = cm_exec_for_str_tmout(ipaddr, CM_IP_LEN,
                                 "ifconfig ens33 | grep -w inet | awk '{printf $2}'",
                                 1);
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_MOD_RPC, "get rpc server ip fail[%d]", iRet);
    }

    addr_in.sin_family = AF_INET;
    addr_in.sin_port = CM_RPC_SERVER_PORT;

    (void)inet_pton(AF_INET, ipaddr, &bin_addr);
    addr_in.sin_addr.s_addr = bin_addr;

    return cm_rpc_server_bind_retry(g_cm_rpc_server_fd,
                                    (struct sockaddr *)&addr_in, sizeof(addr_in), tmout);
}

// 创建socket_fd, bind, listen, 初始化互斥锁, 开启线程接收各个RPC_TYPE发送的消息。
// 线程回调函数: cm_rpc_server_cbk_accept_thread
sint32 cm_rpc_server_init()
{
    sint32 iRet;
    uint32 cnt = 0;
    cm_thread_t tid;

    g_cm_rpc_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(g_cm_rpc_server_fd < 0)
    {
        CM_LOG_ERR(CM_MOD_RPC,
                   "socket fd fail[%d]", g_cm_rpc_server_fd);
        return CM_FAIL;
    }

    iRet = cm_rpc_server_bind(g_cm_rpc_server_fd, CM_RPC_RETRY_TMOUT);
    if(iRet != CM_OK)
    {
        CM_LOG_ERR(CM_MOD_RPC,
                   "bind %d fail", g_cm_rpc_server_fd);
        return CM_FAIL;
    }

    iRet = listen(g_cm_rpc_server_fd, CM_RPC_SERVER_NUM_REQ);
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_MOD_RPC,
                   "listen %d fail[%d]", g_cm_rpc_server_fd, iRet);
        return CM_FAIL;
    }

    CM_MUTEX_INIT(&g_cm_rpc_server_mutex);
    CM_MEM_ZERO(g_cm_rpc_server_cfg,
                sizeof(cm_rpc_server_cfg_t) * CM_RPC_MSG_BUTT);
    //所有线程创建失败直接返回CM_FAIL。
    for(sint32 i = 0; i < CM_RPC_SERVER_NUM_THREAD; i++)
    {
        iRet = CM_THREAD_CREATE(&tid,
                                cm_rpc_server_cbk_accept_thread, &g_cm_rpc_server_fd);
        if(CM_OK != iRet)
        {
            CM_LOG_ERR(CM_MOD_RPC,
                       "create accept thread fail[%d]", iRet);
            if(++cnt == CM_RPC_SERVER_NUM_THREAD)
            {
				CM_MUTEX_DESTROY(&g_cm_rpc_server_mutex);
				close(g_cm_rpc_server_fd);
                return CM_FAIL;
            }
        }
        else
        {
            (void)CM_THREAD_DETACH(tid);
        }
    }
    return CM_OK;
}

//将cm_rpc_msg_info_t的result置为CM_FAIL，之后无论发送成功失败都会关闭fd.
static sint32 cm_rpc_send_fail_rpc_msg
(uint32 fd, uint32 type, sint32 result, uint32 tmout, void *pdata, uint32 len)
{
    sint32 iRet;
    cm_rpc_msg_info_t *pSnd = NULL;

    iRet = cm_rpc_new_rpc_msg(fd, type, len, pdata, &pSnd);
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_MOD_RPC, "create new rpc msg fail");
		close(fd);
        return CM_FAIL;
    }
    pSnd->result = result;
    iRet = cm_rpc_send_rpc_tmout(fd, pSnd, pSnd->headlen+len, tmout);
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_MOD_RPC, "snd_fail_msg_fail[%d]", iRet);
        CM_FREE(pSnd);
        close(fd);
        return CM_FAIL;
    }
    CM_FREE(pSnd);
    close(fd);
    return CM_OK;
}

// 注册RPC_TYPE的线程回调函数，用于在wait_responce中取数据处理。
static void * cm_rpc_server_cbk_reg_thread(void *arg)
{
    sint32 iRet;
    uint32 msgLen = 0;
    cm_rpc_msg_info_t *pMsg = NULL;
    cm_rpc_server_cfg_t *pCfg = arg;
    void *pAckData = NULL;
    uint32 ackLen = 0;

    while(1)
    {
        iRet = cm_queue_get(pCfg->wait_responce, (void **)&pMsg, &msgLen);
        if(CM_OK != iRet)
        {
            // 没有就休眠200毫秒
            usleep(200000);
            continue;
        }

        iRet = pCfg->cbk(pMsg->data, pMsg->datalen, &pAckData, &ackLen);
        CM_LOG_DEBUG(CM_MOD_RPC, "acklen :%d", ackLen);
        if(CM_OK != iRet)
        {
            CM_LOG_ERR(CM_MOD_RPC, "process fail[%d]", iRet);
			//两秒钟之内发不出去就不发了。
            (void)cm_rpc_send_fail_rpc_msg(pMsg->tcp_fd, pMsg->msg_type, iRet, 2, pAckData, ackLen);
            if(NULL != pAckData)
            {
                CM_FREE(pAckData);
            }
            CM_FREE(pMsg);
            continue;
        }
		//两秒钟之内发不出去就不发了。
        iRet = cm_rpc_send_tmout(pMsg->tcp_fd, pMsg->msg_type,
                                 pAckData, ackLen, 2);
        if(CM_OK != iRet)
        {
            CM_LOG_ERR(CM_MOD_RPC, "send ack data fail[%d]", iRet);
        }
        close(pMsg->tcp_fd);
        CM_FREE(pMsg);
        CM_FREE(pAckData);
    }
    return NULL;
}

static sint32 cm_rpc_server_init_cfg
(uint32 type, cm_rpc_server_cbk_func_t cbk, cm_rpc_server_cfg_t *pCfg)
{
    sint32 iRet;
    //一台主机只有一个方法初始化，因此不用考虑加锁
    if(NULL != pCfg->cbk)
    {
        //registered.
        CM_LOG_ERR(CM_MOD_RPC, "this type has already registered.");
        return CM_FAIL;
    }

    iRet = cm_queue_init(&pCfg->wait_responce);
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_MOD_RPC, "init queue fail[%d]", iRet);
        return CM_FAIL;
    }

    CM_MUTEX_INIT(&pCfg->lock);
    pCfg->msg_type = type;
    pCfg->cbk = cbk;
    return CM_OK;
}

//初始化type的cm_rpc_server_cfg_t,失败会清空cm_rpc_server_cfg_t。
//同时开启线程去wait_responce队列取数据进行处理。
//线程回调函数：cm_rpc_server_cbk_reg_thread
sint32 cm_rpc_server_reg(uint32 type, cm_rpc_server_cbk_func_t cbk)
{
    sint32 iRet;
    cm_thread_t tid;
    cm_rpc_server_cfg_t *pCfg = NULL;

    if(type >= CM_RPC_MSG_BUTT)
    {
        CM_LOG_ERR(CM_MOD_RPC, "not supported type[%d]", type);
        return CM_FAIL;
    }

    pCfg = &g_cm_rpc_server_cfg[type];
    iRet = cm_rpc_server_init_cfg(type, cbk, pCfg);
    if(iRet != CM_OK)
    {
        CM_LOG_ERR(CM_MOD_RPC, "init cfg fail");
        if(NULL != pCfg->wait_responce)
        {
			CM_FREE(pCfg->wait_responce);
        }
        return CM_FAIL;
    }

    iRet = CM_THREAD_CREATE(&tid, cm_rpc_server_cbk_reg_thread, pCfg);
    if(CM_OK != iRet)
    {
		CM_LOG_ERR(CM_MOD_RPC, "create thread fail[%d]", iRet);
        //失败的话就需要释放掉wait_responce这个队列。
        //这个时候就不考虑wait_responce还有元素这个问题，几乎不可能出现.
        CM_FREE(pCfg->wait_responce);
		CM_MUTEX_DESTROY(&pCfg->lock);
        CM_MEM_ZERO(pCfg, sizeof(cm_rpc_server_cfg_t));
        return CM_FAIL;
    }
    CM_THREAD_DETACH(tid);
    return CM_OK;
}
