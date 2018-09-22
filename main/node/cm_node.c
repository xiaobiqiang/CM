#include "cm_node.h"

#define CM_NODE_ID_CFG_PATH "/etc/cm/node/cm_node_id.cfg"

static cm_node_tree_t *gp_cm_node_root_tree = NULL;
static cm_node_tree_t *gp_cm_node_subdomain_tree = NULL;
static cm_node_tree_t *gp_cm_node_node_tree = NULL;

static uint32 g_cm_node_local_nid = CM_NODE_ID_NONE;
static uint32 g_cm_node_master_nid = CM_NODE_ID_NONE;
static uint32 g_cm_node_subdomain_id = CM_NODE_SUBDOMAIN_ID_NONE;

sint32 cm_node_init()
{
	sint32 iRet;
	sint8 myIp[CM_IP_LEN];
	sint8 myHost[CM_NODE_HOST_NAME_LEN];

	iRet = cm_exec_for_str_tmout(myHost, CM_NODE_HOST_NAME_LEN,
		"hostname", CM_COMM_REQUEST_TMOUT);
	iRet = cm_exec_tmout(myIp, CM_IP_LEN, CM_COMM_REQUEST_TMOUT,
		"cat %s | awk '$2==%s{printf $1}'", CM_NODE_ID_CFG_PATH, myHost);
}