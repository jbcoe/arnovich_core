/**
  * @file core_main.c
  */

#define DEBUG

#include <core_python/core_python_wrap.h>
#include <core_srv.h>

#include <core_debug/core_debug.h>
_DEBUG_GROUP(PYTHON)

static PyObject *error;

void srv_seterror(const char * msg)
{
    PyErr_SetString(error, msg);
}

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

variant do_debug(variant i, variant s)
{
    if(get_core_debug_level_env()>=variant_as_int(i) &&
       (is_core_debug_group(DEBUG_GROUP_ALL) ||
        is_core_debug_group(core_debug_group)))
    {
        printf("%s \n",variant_to_string(s));
    };
    return VARIANT_EMPTY;
}

DEFINE_PY_FUNCTION(_set_debug_level, set_level, 1, desc)
DEFINE_PY_FUNCTION(_set_debug_group, set_group, 1, desc)
DEFINE_PY_FUNCTION(_debug, do_debug, 2, desc)

START_PY_MODULE(core, Module for Arnovich Core)
    ADD_PY_OBJECT(Connection)
    ADD_PY_OBJECT(Tick)
    ADD_PY_FUNCTION(_set_debug_level)
    ADD_PY_FUNCTION(_set_debug_group)
    ADD_PY_FUNCTION(_debug)
    ADD_PY_EXCEPTION("core.error", error)
    ADD_PY_CONSTANT(DEBUG_OFF,DEBUG_OFF)
    ADD_PY_CONSTANT(DEBUG_ERROR,DEBUG_ERROR)
    ADD_PY_CONSTANT(DEBUG_LOW,DEBUG_LOW)
    ADD_PY_CONSTANT(DEBUG_MEDIUM,DEBUG_MEDIUM)
    ADD_PY_CONSTANT(DEBUG_HIGH,DEBUG_HIGH)
    ADD_PY_CONSTANT(DEBUG_PRETTYHIGH,DEBUG_PRETTYHIGH)
    ADD_PY_CONSTANT(DEBUG_HIGHEST,DEBUG_HIGHEST)
END_PY_MODULE

