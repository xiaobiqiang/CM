#ifndef MAIN_CM_NODE_H
#define MAIN_CM_NODE_H

#include "cm_defines.h"

#define CM_NODE_ID_NONE	0
#define CM_NODE_SUBDOMAIN_ID_NONE 0
#define CM_NODE_MAX_HOSTS_IN_SUBDOMAIN	0x20


typedef struct cm_node_subdomain_info_tt
{
    uint32 subdomain;
    uint32 submaster;
    uint32 size;
    uint32 nodes[CM_NODE_MAX_HOSTS_IN_SUBDOMAIN];
} cm_node_subdomain_info_t;

typedef struct
{
    uint32 nid;
    uint32 subdomain;
    sint8 ipaddr[CM_IP_LEN];
} cm_node_info_t;

typedef struct cm_node_tree_tt
{
    struct cm_node_tree_tt *pSib;
    struct cm_node_tree_tt *pChild;
    uint32 datalen;
    uint8 data[];
} cm_node_tree_t;


extern sint32 cm_node_init();
extern sint32 cm_node_new_node();

//获取该节点所在子域内节点ID为nid的节点信息
extern sint32 cm_node_getinfo_by_nid(uint32 nid, cm_node_info_t *pNode);

extern uint32 cm_node_get_local_nid();
extern uint32 cm_node_get_subdomain_id();
extern uint32 cm_node_get_subdomain_master();
extern uint32 cm_node_get_submaster_info(cm_node_info_t *pinfo);

//只有子域主才能获取到nid所在的子域ID、子域主、子域主节点信息
extern uint32 cm_node_get_subdomain_by_nid(uint32 nid);
extern uint32 cm_node_get_submaster_by_nid(uint32 nid);

extern sint32 cm_node_get_submaster_info_by_subdomainId
(uint32 subdomainId, cm_node_info_t *pinfo);

//检查nid是否在当前子域内
extern sint32 cm_node_check_nid_in_subdomain(uint32 nid);

#endif