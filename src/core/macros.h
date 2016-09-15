#ifndef _MACROS_H
#define _MACROS_H

//#include <string.h>

#define STRINGIFY_HELPER(x)     #x
#define STRINGIFY(x)            STRINGIFY_HELPER(x)
#define GLUE_HELPER(x,y)        x##y
#define GLUE(x,y)               GLUE_HELPER(x,y)
#define GLUE3_HELPER(x,y,z)     x##y##z
#define GLUE3(x,y,z)            GLUE3_HELPER(x,y,z)

#define ZEROING(var) memset(&var, 0, sizeof(var))

#endif
