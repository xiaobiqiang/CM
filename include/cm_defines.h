#ifndef INCLUDE_CM_DEFINES_H
#define INCLUDE_CM_DEFINES_H

#include "cm_sys.h"


#define CM_LOG_DIR "/var/cm/log/"

#define CM_OK 0
#define CM_FAIL 1

#define CM_LOG_DIR "/var/cm/log/"

#define CM_STRING_32 32
#define CM_STRING_64 64
#define CM_STRING_128 128
#define CM_STRING_256 256
#define CM_STRING_512 512
#define CM_STRING_1K 1024

#define CM_IP_LEN CM_STRING_32
#define CM_COMM_REQUEST_TMOUT 10
#define CM_NODE_HOST_NAME_LEN CM_STRING_64

/********* typedef for number *********/
typedef signed char sint8;
typedef unsigned char uint8;
typedef int16_t sint16;
typedef uint16_t uint16;
typedef int32_t sint32;
typedef uint32_t uint32;
typedef int64_t sint64;
typedef uint64_t uint64;
/********end typedef for number********/

typedef sint32 (*cm_main_init_func_t)();

typedef pthread_mutex_t cm_mutex_t;
typedef pthread_t cm_thread_t;
#define CM_MUTEX_INIT(pMutex) pthread_mutex_init(pMutex, NULL)
#define CM_MUTEX_LOCK(pMutex) pthread_mutex_lock(pMutex)
#define CM_MUTEX_UNLOCK(pMutex) pthread_mutex_unlock(pMutex)
#define CM_MUTEX_DESTROY(pMutex) pthread_mutex_destroy(pMutex)
#define CM_MUTEX_TRY_LOCK(pMutex) pthread_mutex_trylock(pMutex)

#define CM_SYSTEM(cmd) system(cmd)
#define CM_THREAD_CREATE(pTid, cbk, pArg) pthread_create(pTid, NULL, cbk, pArg)
#define CM_THREAD_DETACH(tid) pthread_detach(tid)

#define CM_MEM_ZERO(addr, len) memset(addr, 0, len)
#define CM_MEM_CPY(dst, src, len) memcpy(dst, src, len)
#define CM_MALLOC(len) malloc(len)
#define CM_FREE(p)	free(p)


#endif /* INCLUDE_CM_DEFINES_H */

