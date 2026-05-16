#ifndef CORE_POOLS_H
#define CORE_POOLS_H

#include "CoreTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Generic fixed pool pattern
*/

#define CORE_POOL_DEFINE(type, name, max) \
typedef struct { \
    type items[max]; \
    core_u32 count; \
} name;



#ifdef __cplusplus
}
#endif

#endif