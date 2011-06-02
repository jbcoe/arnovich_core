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

typedef enum
{
	DEBUG_GROUP_UNDEFINED       = -1,
	DEBUG_GROUP_ZERO            = 0,
	DEBUG_GROUP_MT_RNDS         = 100,
	DEBUG_GROUP_MC_CALL         = 200,
	DEBUG_GROUP_MC_PTRB         = 300,
	DEBUG_GROUP_MC_PYOF         = 400,
	DEBUG_GROUP_MC_PATH         = 500,
	DEBUG_GROUP_MC_ASTS         = 600,
	DEBUG_GROUP_MC_PROC         = 700,
	DEBUG_GROUP_BS_BSMC         = 800,
	DEBUG_GROUP_MC_PRMS         = 900,
	DEBUG_GROUP_MC_CLDR         = 1000,
	DEBUG_GROUP_MC_MATH         = 1100,
	DEBUG_GROUP_MC_FUNC         = 1200,
	DEBUG_GROUP_MT_ROOT         = 1300,
	DEBUG_GROUP_MT_OPTM         = 1400,
	DEBUG_GROUP_AT_BCST         = 1500,
	DEBUG_GROUP_LCL_FUNC_LANG   = 1600,
	DEBUG_GROUP_LCL_FUNC_SEMN   = 1700,
	DEBUG_GROUP_LCL_PARSE       = 1800,
	DEBUG_GROUP_LCL_PARSE_LANG  = 1900,
	DEBUG_GROUP_AT_SVCS         = 2000,
} DEBUG_GROUP;

#ifdef DEBUG

#define CORE_DEBUG_LEVEL_ENV_VAR "DEBUG_LEVEL"

#define CORE_DEBUG_GROUP_ENV_VAR "DEBUG_GROUP"

#include <stdio.h>

int   get_core_debug_level_env(void);
void  set_core_debug_level_env(int);
int   get_core_debug_group_env(void);
void  set_core_debug_group_env(int);
FILE* get_core_debug_file(void);
void  set_core_debug_file(FILE*);


#define _SET_DEBUG_LEVEL(level) \
	set_core_debug_level_env(level);

#define _SET_DEBUG_GROUP(debug) \
	set_core_debug_group_env(debug);

#define _SET_DEBUG_FILEPATH(filepath) \
	set_core_debug_file(fopen(filepath,"a"));

#define _UNSET_DEBUG_FILEPATH() \
	if(get_core_debug_file()) \
	{ \
		fclose(get_core_debug_file()); \
		set_core_debug_file(0); \
	}

#define _DEBUG_GROUP(group) \
	static int core_debug_group = group;

#define _DEBUG(level, exp, ...) \
	{\
		if(get_core_debug_level_env()>=level && (get_core_debug_group_env()==DEBUG_GROUP_UNDEFINED || get_core_debug_group_env()==DEBUG_GROUP_ZERO || get_core_debug_group_env()==core_debug_group)) \
		{\
			if(get_core_debug_file()) \
			{ \
				fprintf (get_core_debug_file(), "%s:%i   "exp"\n", __FILE__, __LINE__, ##__VA_ARGS__); \
			} \
			else \
			{ \
				printf("%s:%i   "exp"\n", __FILE__, __LINE__, ##__VA_ARGS__); \
			} \
		};\
	};

#else

#define _DEBUG(level, exp, ...) ;
#define _DEBUG_GROUP(group) ;
#define _SET_DEBUG_LEVEL(level) ;
#define _SET_DEBUG_GROUP(debug) ;
#define _SET_DEBUG_FILEPATH(filepath) ;
#define _UNSET_DEBUG_FILEPATH() ;

#endif

#endif /* CORE_DEBUG_H_ */
