#ifndef MAIN_CM_QUEUE_H
#define MAIN_CM_QUEUE_H

#include "cm_defines.h"

#define CM_QUEUE_CAPACITY	16

typedef struct cm_queue_ele_tt cm_queue_ele_t;

typedef struct
{
    uint32 capacity;
    uint32 size;
    uint32 rd_index;
    uint32 wr_index;
    cm_queue_ele_t elements[CM_QUEUE_CAPACITY];
} cm_queue_t;

extern sint32 cm_queue_init(cm_queue_t **ppQueue);

extern sint32 cm_queue_add(cm_queue_t *pQueue, void *pData, uint32 len);

extern sint32 cm_queue_get(cm_queue_t *pQueue, void **ppAckData, uint32 *pAckLen);

//don't provide the interface to destroy the queue,because i don't know the pdata
//is whether or not allocated by malloc. another is destroy the queue, all data
//in the queue will be free together, so all data can't be used anymore, that's
//easy to generate problem.but we provide cm_queue_delete

#endif