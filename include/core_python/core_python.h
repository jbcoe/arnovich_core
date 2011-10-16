/*
 * @file core_python.h
 * @brief Python utility functions
 */

#ifndef CORE_PYTHON_H_
#define CORE_PYTHON_H_

#include <core_types/core_types_variant.h>
#include <core_types/core_types_matrix.h>

typedef enum
{
    CORE_PYTHON_ERROR_SUCCESS = 0,
    CORE_PYTHON_ERROR_FAILURE = 1
} CORE_PYTHON_ERROR;

#include <Python.h>

variant core_python_py_to_variant(PyObject*);

PyObject* core_python_variant_to_py(variant);

matrix core_python_py_to_matrix(PyObject*);

PyObject* core_python_matrix_to_py(matrix);

#endif /* CORE_PYTHON_H_ */
