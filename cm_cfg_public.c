#include "cm_log.h"
#include "cm_cmt.h"
#include "cm_cmt_common.h"

const sint8 *g_cm_log_type_name_map[CM_LOG_TYPE_BUTT] =
{
    "ERR",
    "WARN",
    "DEBUG",
};

const cm_log_name_path_map_t g_cm_log_name_path_map[CM_MOD_BUTT] =
{
    {"Rpc", CM_LOG_DIR"ceres_cm.log"},
    {"Comm", CM_LOG_DIR"ceres_cm.log"}
};

const cm_cmt_msg_cfg_t g_cm_cmt_msg_cfgs[CM_CMT_MSG_TYPE_BUTT] =
{

};

