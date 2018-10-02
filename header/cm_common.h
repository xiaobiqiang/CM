#ifndef INCLUDE_CM_COMMON_H
#define INCLUDE_CM_COMMON_H

#include "cm_sys.h"
#include "cm_defines.h"

#define CM_SNPRINT_ADD(buf, size, format, ...) 	\
{	\
	uint32 len = strlen(buf);	\
	snprintf((buf)+len, ((size)-len), (format), ## __VA_ARGS__);	\
}

#define CM_VSNPRINT_ADD(buf, size, format, ...) 	\
{	\
	va_list ap;	\
	uint32 len = strlen(buf);	\
	va_start(ap, (format));	\
	vsnprintf(((buf)+len), ((size)-len), (format), ap);	\
	va_end(ap);	\
}

#define CM_VSNPRINT_AP_ADD(buf, size, format, ap)	\
{	\
	uint32 len = strlen(buf);	\
	vsnprintf((buf)+len, (size)-len, (format), ap);	\
}

extern uint64 cm_get_time_now(uint64 * time);

extern sint32 cm_get_timestamp(sint8 * str_time, uint32 len);

extern sint32 cm_exec_for_str_tmout
(sint8 *buf, uint32 size, const sint8 *cmd, uint32 tmout);

extern sint32 cm_exec_tmout
(sint8 *buf, uint32 size, uint32 tmout, const sint8 *format, ...);


#endif /* INCLUDE_CM_COMMON_H */
