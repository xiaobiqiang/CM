#include "cm_rpc.h"
#include "cm_log.h"
#include "cm_rpc_test.h"

static const cm_main_init_func_t init_cbks[] =
{
    cm_log_init,
    cm_rpc_init,
    cm_rpc_test_init,
    NULL,
};

static sint32 cm_main_iter_init_cbks()
{
    sint32 iRet;
    uint32 cnt = 0;
    cm_main_init_func_t *cbk = init_cbks;
    while(NULL != *cbk)
    {
        iRet = (*cbk)();
        if(CM_OK != iRet)
        {
            CM_LOG_ERR(CM_LOG_MOD_COMM, "index %u init fail[%d]", cnt, iRet);
            return CM_FAIL;
        }
        ++cnt;
        ++cbk;
    }
    return CM_OK;
}

sint32 main(sint32 argc, sint8 **argv)
{
    sint32 iRet;
    iRet = cm_main_iter_init_cbks();
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_LOG_MOD_COMM, "init function exec fail[%d]", iRet);
        return iRet;
    }
    while(1)
    {
        sleep(2);
    }
}
