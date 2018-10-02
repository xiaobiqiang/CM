#ifndef MAIN_CM_LOG_H
#define MAIN_CM_LOG_H

#include "cm_defines.h"
#include "cm_sys.h"

#define CM_LOG_LEN_MSG (CM_STRING_256+1)

typedef struct
{
    sint8 *name;
    sint8 *path;
} cm_log_name_path_map_t;

typedef enum
{
    CM_LOG_MOD_RPC = 0,
    CM_LOG_MOD_COMM,
    CM_LOG_MOD_CMT,
    CM_LOG_MOD_BUTT
} cm_log_mod_e;

typedef enum
{
    CM_LOG_TYPE_ERR = 0,
    CM_LOG_TYPE_WARN,
    CM_LOG_TYPE_DEBUG,
    CM_LOG_TYPE_BUTT
} cm_log_type_e;

extern sint32 cm_log_init();

extern sint32 cm_log_print(uint32 type, uint32 mod, uint32 line,
                           const sint8 *func, const sint8 *format, ...);

#define CM_LOG_ERR(MOD, FORMAT, ...) 	\
	cm_log_print(CM_LOG_TYPE_ERR, (MOD), __LINE__, __func__, (FORMAT), ## __VA_ARGS__)
#define CM_LOG_WARN(MOD, FORMAT, ...) 	\
	cm_log_print(CM_LOG_TYPE_WARN, (MOD), __LINE__, __func__, (FORMAT), ## __VA_ARGS__)
#define CM_LOG_DEBUG(MOD, FORMAT, ...)	\
	cm_log_print(CM_LOG_TYPE_DEBUG, (MOD), __LINE__, __func__, (FORMAT), ## __VA_ARGS__)

#endif /* MAIN_CM_LOG_H */
