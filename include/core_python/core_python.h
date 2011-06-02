/*
 * @file core_python.h
 */

#ifndef CORE_PYTHON_H_
#define CORE_PYTHON_H_

typedef enum
{
    CORE_PYTHON_ERROR_SUCCESS = 0,
    CORE_PYTHON_ERROR_FAILURE = 1
} CORE_PYTHON_ERROR;

#include <Python.h>

CORE_PYTHON_ERROR core_python_py_to_double(PyObject*, double*);

#endif /* CORE_PYTHON_H_ */
