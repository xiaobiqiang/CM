#include "cm_common.h"
#include "cm_log.h"
#include <sys/wait.h>

uint64 cm_get_time_now(uint64 *time)
{
    struct timeval tp;

    (void)gettimeofday(&tp, NULL);
    if(NULL != time)
    {
        *time = tp.tv_sec;
    }

    return tp.tv_sec;
}

sint32 cm_get_timestamp(sint8 *str_time, uint32 len)
{
    sint32 iRet;
    time_t ntime;
    struct tm *p_time = NULL;

    time(&ntime);
    p_time = localtime(&ntime);
    if(NULL == p_time)
    {
        return CM_FAIL;
    }

    //return wirte len
    iRet = strftime(str_time, len, "%Y%m%d%H%M%S", p_time);
    if(0 == iRet)
    {
        return CM_FAIL;
    }
    return CM_OK;
}

sint32 cm_exec_for_str_tmout
(sint8 *buf, uint32 size, const sint8 *cmd, uint32 tmout)
{
    sint32 iRet;
    sint32 fds[2];
    pid_t pid = 0;
    sint32 tmpfd = 0, status = 0;

    iRet = pipe(fds);
    if(CM_OK != iRet)
    {
        CM_LOG_ERR(CM_MOD_COMM, "pipe fail[%d]", iRet);
        return CM_FAIL;
    }

    if((pid = fork()) < 0)
    {
        CM_LOG_ERR(CM_MOD_COMM, "fork error[%d]", iRet);
        return CM_FAIL;
    }
    else if(pid > 0)
    {
        close(fds[1]);
        if((NULL != buf) && (size > 0))
        {
            (void)read(fds[0], buf, size);
        }
        (void)waitpid(pid, &status, 0);
        if(WIFEXITED(status))
        {
            return CM_OK;	//千万别用exit，那样主进程就退出了。
        }
        else
        {
            return CM_FAIL;
        }
    }
    close(fds[0]);
    (void)dup2(fds[1], STDOUT_FILENO);
    alarm(tmout);
    iRet = execl("/bin/sh", "sh", "-c", cmd, (char *)0);
    if(iRet == CM_FAIL)
    {
        CM_LOG_ERR(CM_MOD_COMM, "%s exec fail[%d]", cmd, iRet);
        close(fds[1]);
        _exit(CM_FAIL);
    }
    _exit(CM_OK);
}

sint32 cm_exec_tmout
(sint8 *buf, uint32 size, uint32 tmout, const sint8 *format, ...)
{
    va_list ap;
    sint8 cmdbuff[CM_STRING_1K] = {0};

    va_start(ap, format);
    CM_VSNPRINT_AP_ADD(cmdbuff, CM_STRING_1K, format, ap);
    return cm_exec_for_str_tmout(buf, size, cmdbuff, tmout);
}




