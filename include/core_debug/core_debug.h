/*
 * @file core_debug.h
 */

#ifndef CORE_DEBUG_H_
#define CORE_DEBUG_H_

typedef enum
{
    DEBUG_UNDEFINED     = -1,
    DEBUG_OFF           = 0,
    DEBUG_ERROR         = 5,
    DEBUG_LOW           = 10,
    DEBUG_MEDIUM        = 20,
    DEBUG_HIGH          = 30,
    DEBUG_PRETTYHIGH    = 40,
    DEBUG_HIGHEST       = 50
} DEBUG_LEVEL;

#ifdef DEBUG

#ifndef CASTOP
#define CASTOP(op) op
#endif

void  set_core_debug_level_env(int);
void  set_core_debug_group_env(char*);
void  set_core_debug_file(char*);
void  debug(int,char*,char*,char*,int,...);

#define _SET_DEBUG_LEVEL(level) \
	set_core_debug_level_env(level);

#define _SET_DEBUG_GROUP(debug) \
	set_core_debug_group_env(debug);

#define _SET_DEBUG_FILEPATH(filepath) \
	set_core_debug_file(filepath);

#define _UNSET_DEBUG_FILEPATH() \
	unset_core_debug_file();

#define _DEBUG_GROUP(group) \
	static char* core_debug_group = CASTOP(#group);

#include <stdio.h>
#define DEBUG_GROUP_ALL       "__ALL__"
int is_core_debug_group(char* group);
int get_core_debug_level_env();

#define _DEBUG(level, exp, ...) \
{ \
    if(get_core_debug_level_env()>=level && \
       (is_core_debug_group(DEBUG_GROUP_ALL) || \
        is_core_debug_group(core_debug_group))) \
    { \
        printf("%s:%i   "exp"\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    }; \
}

 
#define _DEBUG2(level, exp, ...) \
    debug2(level, core_debug_group, "%s:%i   "exp"\n", __FILE__, __LINE__, ##__VA_ARGS__);

#else

#define _DEBUG(level, exp, ...) ;
#define _DEBUG_GROUP(group) ;
#define _SET_DEBUG_LEVEL(level) ;
#define _SET_DEBUG_GROUP(debug) ;
#define _SET_DEBUG_FILEPATH(filepath) ;
#define _UNSET_DEBUG_FILEPATH() ;

#endif

#endif /* CORE_DEBUG_H_ */
