
/*
 * @file core_python.c
 */

#include <core_python/core_python.h>

variant core_python_py_to_variant(PyObject* o)
{
    if(PyInt_Check(o))
    {
        return core_types_variant_int(PyInt_AsLong(o));
    } else if(PyString_Check(o)) {
        char *s = PyString_AsString(o);
        if(s)
        {
            PyObject *p = PyInt_FromString(s, NULL, 0);
            if(p)
            {
                return core_types_variant_int(PyInt_AsLong(p));
            }
        }
    }
    if(PyFloat_Check(o))
    {
        return core_types_variant_double(PyFloat_AsDouble(o));
    } else if(PyString_Check(o)) {
        PyObject *s = PyObject_Str(o);
        if(s)
        {
            PyObject *p = PyFloat_FromString(s, NULL);
            if(p)
            {
                return core_types_variant_double(PyFloat_AsDouble(p));
            }
        }
    }
    return VARIANT_NIL;
}

PyObject* core_python_variant_to_py(variant v)
{
    if(core_types_variant_is_int(v))
    {
        return PyInt_FromLong(core_types_int(v));
    }
    if(core_types_variant_is_double(v))
    {
        return PyFloat_FromDouble(core_types_double(v));
    }
    return NULL;
}

CORE_PYTHON_ERROR core_python_py_to_double(PyObject* o, double* d)
{
    if(PyFloat_Check(o))
    {
        *d = PyFloat_AsDouble(o);
    } else if(PyString_Check(o)) {
        PyObject *s = PyObject_Str(o);
        if(s)
        {
            *d = PyFloat_AsDouble(PyFloat_FromString(s, NULL));
        }
    } else {
        return CORE_PYTHON_ERROR_FAILURE;
    }
    return CORE_PYTHON_ERROR_SUCCESS;
}

