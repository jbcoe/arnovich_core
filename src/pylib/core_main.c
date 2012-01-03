/*
 * @file core_main.c
 */

#define DEBUG

#include <core_python/core_python_wrap.h>

#include <core_debug/core_debug.h>

variant set_level(variant i)
{
    if(!variant_is_int(i))
    {
        return variant_from_error("Invalid debug level");
    }
    _SET_DEBUG_LEVEL(variant_as_int(i));
    return VARIANT_EMPTY;
}

variant set_group(variant s)
{
    if(variant_is_string(s))
    {
        return variant_from_error("Invalid debug group");
    }
    _SET_DEBUG_GROUP(variant_as_string(s));
    return VARIANT_EMPTY;
}

DEFINE_PY_FUNCTION(set_debug_level, set_level, 1, desc)
DEFINE_PY_FUNCTION(set_debug_group, set_group, 1, desc)

START_PY_MODULE(core, Module for Arnovich Core)
    ADD_PY_FUNCTION(set_debug_level)
    ADD_PY_FUNCTION(set_debug_group)
    ADD_PY_EXCEPTION(CoreError)
END_PY_MODULE

