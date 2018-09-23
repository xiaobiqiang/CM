#ifndef MAIN_CM_NODE_H
#define MAIN_CM_NODE_H

#include "cm_defines.h"

#define CM_NODE_ID_NONE	0
#define CM_NODE_SUBDOMAIN_ID_NONE 0
#define CM_NODE_MAX_HOSTS_IN_SUBDOMAIN	0x20


typedef struct cm_node_subdomain_info_tt
{
    uint32 subdomain;
    uint32 size;
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
extern uint32 cm_node_get_local_nid();
extern sint32 cm_node_add(uint32 nid,);

#endif