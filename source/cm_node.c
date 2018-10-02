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

sint32 cm_node_new_node()
{
    return CM_OK;
}

uint32 cm_node_get_local_nid()
{
    return CM_NODE_ID_NONE;
}

uint32 cm_node_get_subdomain_master()
{
    return g_cm_node_subdomain_id;
}

uint32 cm_node_get_subdomain_id()
{
    return g_cm_node_subdomain_id;
}

uint32 cm_node_get_submaster_info(cm_node_info_t *pNode)
{
    return cm_node_getinfo_by_nid
           (cm_node_get_subdomain_master(), pNode);
}

//如果节点nid位于当前节点的子域内才能获取到节点信息。
//换句话说，只能获取到当前子域的节点信息，并不能获取到其他子域的节点信息
sint32 cm_node_getinfo_by_nid(uint32 nid, cm_node_info_t *pNode)
{
    return CM_OK;
}

//子域主通过任意节点ID获取该节点所在的子域ID
//子域内的普通节点不能获得nid的子域ID、子域主、子域主节点信息
uint32 cm_node_get_subdomain_by_nid(uint32 nid)
{
    return CM_NODE_SUBDOMAIN_ID_NONE;
}

uint32 cm_node_get_submaster_by_nid(uint32 nid)
{
    return CM_NODE_ID_NONE;
}

sint32 cm_node_get_submaster_info_by_subdomainId
(uint32 subdomainId, cm_node_info_t *pinfo)
{
    return CM_OK;
}

sint32 cm_node_check_nid_in_subdomain(uint32 nid)
{
    return CM_OK;
}