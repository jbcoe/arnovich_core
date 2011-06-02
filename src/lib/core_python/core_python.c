
/*
 * @file core_python.c
 */

#include <core_python/core_python.h>

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
