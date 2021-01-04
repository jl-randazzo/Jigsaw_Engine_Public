#ifndef _J_DEBUG_H_
#define _J_DEBUG_H_
#include "j_log.h"
#include <cassert>
#ifdef __JIGSAW_DEBUG_ENABLED
#define DEBUG_ASSERT(expr, err) if(!( expr)){\
	LOG_ERR(err);\
	assert(false);\
}


#define J_D_ASSERT_LOG_ERROR(expr, classname, ...)\
if (!(expr)) { \
		J_LOG_ERROR(classname, __VA_ARGS__); \
		assert(false); \
}

#else
#define DEBUG_ASSERT(expr, err) 
#endif
#endif
