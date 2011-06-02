/*
 * @file core_debug.c
 */

#ifdef DEBUG

#include <core_debug/core_debug.h>

#include <stdlib.h>

static int      core_debug_level_env = DEBUG_UNDEFINED;
static int      core_debug_group_env = DEBUG_GROUP_UNDEFINED;
static FILE*    core_debug_file = 0;

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

int get_core_debug_group_env()
{
	if(core_debug_group_env==DEBUG_GROUP_UNDEFINED)
	{
		char* debug_group_env = getenv(CORE_DEBUG_GROUP_ENV_VAR);
		if(debug_group_env)
		{
			core_debug_group_env = atoi(debug_group_env);
		}
	}
	return core_debug_group_env;
}

void set_core_debug_group_env(int group)
{
	core_debug_group_env = group;
}

FILE* get_core_debug_file()
{
	return core_debug_file;
}

void set_core_debug_file(FILE* file)
{
	core_debug_file = file;
}

#endif
