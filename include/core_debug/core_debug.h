/**
 * \package core_debug
 * \brief A very simple debug outputter.
 * 
 * <b>Usage with C/C++</b>
 *
 * To insert a debug message in a file include core_debug.h for C or core_debug.hpp for C++ and do the following at the start of the file
 * \code
 * _DEBUG_GROUP(DEBUG_GROUP)
 * \endcode
 * where DEBUG_GROUP is a string that identifies the debug group to assing all debugging message of this file to.
 * 
 * To insert a debug message in the code then do something like
 * \code
 * _DEBUG(DEBUG_LEVEL, "my debug message string=%s integer=%i", "this is a string", 10)
 * \endcode
 * where DEBUG_LEVEL is an integer that determines the level of the message. As shown the message can use the standard C-style printf format for inserting variable values.
 * 
 * To enable the debugging output build your code with the flag DEBUG=1.
 *
 * Once build with the messages enables, you can output messages by setting the group and level either programmatically
 * \code
 * _SET_DEBUG_GROUP(DEBUG_GROUP)
 * _SET_DEBUG_LEVEL(DEBUG_LEVEL)
 * \endcode
 * or at runtime by environment variables
 * \code
 * export DEBUG_GROUP=DEBUG_GROUP
 * export DEBUG_LEVEL=DEBUG_LEVEL
 * \endcode
 *
 * To output all messages set the group to 
 * \code
 * export DEBUG_GROUP=__ALL__
 * \endcode
 * and the level to the highest level used in the code.
 *
 * <b>Usage with Python</b>
 *
 * To insert a debug message from python do something similar to
 * \code
 * import arnovich.core as core
 * core.set_debug_level(core.DEBUG_ERROR)
 * core.debug(core.DEBUG_ERROR,"debug message")
 * \endcode
 *
 * <b>Files of interest</b>
 * - core_debug/core_debug.h
 * - core_debug/core_debug.hpp
 * - py/core.py
 * - pylib/core_main.c
 *
 */

/**
 * \file core_debug.h
 * \brief This is the C interface for the very simple core debugger.
 */

#ifndef CORE_DEBUG_H_
#define CORE_DEBUG_H_

/**
 * @brief Some standard predefined debug levels.
 *
 * You can define you own values or use these. Your choice...
 */
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
    debug(level, core_debug_group, "%s:%i   "exp"\n", __FILE__, __LINE__, ##__VA_ARGS__);

#else

/**
 * @brief Macro for outputting debug message
 */
#define _DEBUG(level, exp, ...) ;

/**
 * @brief Macro for setting the debug group to be used in a file
 */
#define _DEBUG_GROUP(group) ;

/**
 * @brief Macro for programatically setting the debug level
 */
#define _SET_DEBUG_LEVEL(level) ;

 /**
 * @brief Macro for programatically setting the debug group
 */
#define _SET_DEBUG_GROUP(debug) ;

/**
 * @brief Macro for programatically setting the debug output to be a file instead of std out
 */
#define _SET_DEBUG_FILEPATH(filepath) ;

 /**
 * @brief Macro for programatically unsetting the debug output to be a file and instead redirect to std out
 */
#define _UNSET_DEBUG_FILEPATH() ;

#endif

#endif /* CORE_DEBUG_H_ */
