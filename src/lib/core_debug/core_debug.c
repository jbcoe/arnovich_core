/*
 * @file core_debug.c
 */

#ifdef DEBUG

#include <core_debug/core_debug.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define DEBUG_GROUP_ALL       "__ALL__"

#define DEBUG_GROUP_SIZE      30
#define DEBUG_GROUP_SET_SIZE  5

#define CORE_DEBUG_LEVEL_ENV_VAR "DEBUG_LEVEL"

#define CORE_DEBUG_GROUP_ENV_VAR "DEBUG_GROUP"

static int                       core_debug_level_env = DEBUG_UNDEFINED;
static char                      core_debug_group_env[DEBUG_GROUP_SET_SIZE][DEBUG_GROUP_SIZE] = {"", "", "", "", ""};
static int                       core_debug_group_set = 0;
static FILE*                     core_debug_file = NULL;

//static int get_core_debug_level_env()
int get_core_debug_level_env()
{
	if(core_debug_level_env==DEBUG_UNDEFINED)
	{
		char* debug_level_env = getenv(CORE_DEBUG_LEVEL_ENV_VAR);
		if(debug_level_env)
		{
			core_debug_level_env = atoi(debug_level_env);
		}
	}
	return core_debug_level_env;
}

void set_core_debug_level_env(int level)
{
	core_debug_level_env = level;
}

void set_core_debug_group_env(char* group)
{
    core_debug_group_set = 0;
    char* tok = strtok(group, "|");
    while(tok && (core_debug_group_set < DEBUG_GROUP_SET_SIZE))
    {
        strncpy(core_debug_group_env[core_debug_group_set], tok, DEBUG_GROUP_SIZE);
        core_debug_group_set++;
        tok = strtok(NULL, "|");
    }
}

//static int is_core_debug_group(char* group)
int is_core_debug_group(char* group)
{
    if(!core_debug_group_set)
    {
        char* debug_group_env = getenv(CORE_DEBUG_GROUP_ENV_VAR);
        if(debug_group_env)
        {
            set_core_debug_group_env(debug_group_env);
        }
        if(!core_debug_group_set)
        {
            return 1;
        }
    }
    int i=0;
    for(;i<core_debug_group_set;++i)
    {
        if(!strncmp(core_debug_group_env[i], group, DEBUG_GROUP_SIZE))
        {
            return 1;
        }
    }
	return 0;
}

void set_core_debug_file(char* file)
{
	core_debug_file = fopen(file,"a");
}

void unset_core_debug_file()
{
    if(core_debug_file) 
    { 
        fclose(core_debug_file); 
        set_core_debug_file(0); 
    }
}

void debug(int level, char* core_debug_group, char* format, char* file, int line,...)
{
    if(get_core_debug_level_env()>=level && 
       (is_core_debug_group(DEBUG_GROUP_ALL) || 
        is_core_debug_group(core_debug_group)))
    {
        va_list vl;
        va_start(vl, line);
        if(core_debug_file) 
        { 
            fprintf (core_debug_file, format, file, line, vl); 
        } 
        else 
        { 
            printf(format, file, line, vl); 
        } 
        va_end(vl);
    };
}

#endif
