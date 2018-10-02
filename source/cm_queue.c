#include "cm_queue.h"

sint32 cm_queue_init(cm_queue_t **ppQueue)
{
    cm_queue_t *pQueue = NULL;
    if(NULL == ppQueue)
    {
        return CM_FAIL;
    }

    pQueue = CM_MALLOC(sizeof(cm_queue_t));
    if(NULL == pQueue)
    {
        return CM_FAIL;
    }

    pQueue->capacity = CM_QUEUE_CAPACITY;
    pQueue->size = 0;
    pQueue->wr_index = 0;
    pQueue->rd_index = 0;
    CM_MEM_ZERO(pQueue->elements,
                sizeof(cm_queue_ele_t) * pQueue->capacity);

    *ppQueue = pQueue;
    return CM_OK;
}

sint32 cm_queue_add(cm_queue_t *pQueue, void *pData, uint32 len)
{
    cm_queue_ele_t *pVal = NULL;
    if(NULL == pQueue || pQueue->size == pQueue->capacity)
    {
        return CM_FAIL;
    }

    pVal = &pQueue->elements[pQueue->wr_index];
    pVal->pData = pData;
    pVal->len = len;

    if(++pQueue->wr_index == pQueue->capacity)
    {
        pQueue->wr_index = 0;
    }
    pQueue->size++;
    return CM_OK;
}

sint32 cm_queue_get(cm_queue_t *pQueue, void **ppAckData, uint32 *pAckLen)
{
    cm_queue_ele_t *pVal = NULL;
    if(NULL == pQueue || pQueue->size == 0)
    {
        *ppAckData = NULL;
        *pAckLen = 0;
        return CM_FAIL;
    }

    pVal = &pQueue->elements[pQueue->rd_index];
    *ppAckData = pVal->pData;
    *pAckLen = pVal->len;
    if(++pQueue->rd_index == pQueue->capacity)
    {
        pQueue->rd_index = 0;
    }
    pQueue->size--;	//考虑到多线程添加，应该把这种影响较大的语句放在后面。
    //如果把ppAckData和pAckLen的赋值写在size--后面，就有可能返回的结果是刚刚add进来的数据。
    //试想一下，size=capacity时，size--之后把CPU执行权交给add函数，add函数执行完就有可能pVal指向的值。
    //这样ack拿到的就是刚刚插进来的值。没满的时候不会发生这种情况，因为操作的地址不同。
    return CM_OK;
}