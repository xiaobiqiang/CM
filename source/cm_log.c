#include "cm_log.h"
#include "cm_common.h"

#define CM_LOG_FILE_MODE 	O_WRONLY|O_CREAT|O_APPEND
#define CM_LOG_FILE_ACCESS 	S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH

extern const cm_log_name_path_map_t g_cm_log_name_path_map[CM_MOD_BUTT];
extern const sint8 *g_cm_log_type_name_map[CM_LOG_TYPE_BUTT];

sint32 cm_log_init()
{
    sint32 iRet;
	iRet = CM_SYSTEM("mkdir -p "CM_LOG_DIR);
	if(iRet != CM_OK)
	{
		return CM_FAIL;
	}
	//没有就创建，有就打开
	iRet = open(CM_LOG_DIR"ceres_cm.log", CM_LOG_FILE_MODE, CM_LOG_FILE_ACCESS);
	if (iRet > 0)	//打开了需要关闭
	{
		close(iRet);
	}
	else
	{
		return CM_FAIL;
	}
    return CM_OK;
}

sint32 cm_log_print
(
    uint32 type, uint32 mod, uint32 line,
    const sint8 *func, const sint8 *format, ...
)
{
    sint32 fd = -1;
    sint32 iRet = CM_FAIL;
    uint32 len_str = 0;
    sint8 str_log[CM_LOG_LEN_MSG] = {0};
    va_list ap;

    if((type >= CM_LOG_TYPE_BUTT) ||
            (mod >= CM_MOD_BUTT))
    {
        return CM_FAIL;
    }

    //get time stamp string
    iRet = cm_get_timestamp(str_log, CM_LOG_LEN_MSG);
    if(CM_OK != iRet)
    {
        return CM_FAIL;
    }

    //get log mod name and log type string.like Rpc.ERR
    CM_SNPRINT_ADD(str_log, CM_LOG_LEN_MSG, ":[");
    CM_SNPRINT_ADD(str_log, CM_LOG_LEN_MSG, g_cm_log_name_path_map[mod].name);
    CM_SNPRINT_ADD(str_log, CM_LOG_LEN_MSG, ".");
    CM_SNPRINT_ADD(str_log, CM_LOG_LEN_MSG, g_cm_log_type_name_map[type]);
    CM_SNPRINT_ADD(str_log, CM_LOG_LEN_MSG, "]:");

    va_start(ap, format);
    CM_VSNPRINT_AP_ADD(str_log, CM_LOG_LEN_MSG, format, ap);
    va_end(ap);

    CM_SNPRINT_ADD(str_log, CM_LOG_LEN_MSG, ":[");
    CM_SNPRINT_ADD(str_log, CM_LOG_LEN_MSG, "%u, %s", line, func);
    CM_SNPRINT_ADD(str_log, CM_LOG_LEN_MSG, "]");

    CM_SNPRINT_ADD(str_log, CM_LOG_LEN_MSG, "\n");

    fd = open(g_cm_log_name_path_map[mod].path,
              CM_LOG_FILE_MODE, CM_LOG_FILE_ACCESS);
    if(-1 == fd)
    {
        return CM_FAIL;
    }

    len_str = strlen(str_log);
    iRet = write(fd, str_log, len_str);
    if((iRet < 0) || (len_str > iRet))
    {
        close(fd);
        return CM_FAIL;
    }
    close(fd);
    return CM_OK;
}
