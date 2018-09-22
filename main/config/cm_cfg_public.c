#include "cm_log.h"

const sint8 *g_cm_log_type_name_map[CM_LOG_TYPE_BUTT] =
{
	"ERR",
	"WARN",
	"DEBUG",
};

const cm_log_name_path_map_t g_cm_log_name_path_map[CM_LOG_MOD_BUTT] =
{
	{"Rpc", CM_LOG_DIR"xbq_cm.log"},
	{"Comm", CM_LOG_DIR"xbq_cm.log"}
};

