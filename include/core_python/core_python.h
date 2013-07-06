
/**
 * @package core_python
 * @brief A collection of Python interface helpers.
 *
 * These functions provide an interface between the types in core_types and native Python types.
 * 
 * They should be used in the interface files between a C/C++ library and the Python.
 *
 * <b>Example</b>
 *
 * A simple C-function written as a Python extension that takes a single \c PyObject as input, passing it into a C-function \c my_function(variant) and outputting the result as a \c PyObject.
 * \code
 * 
 * PyObject* Py_my_function(PyObject *self, PyObject *args)
 * {
 *   PyObject* py_input;
 *   if(!PyArg_ParseTuple(args, "O", &py_input))
 *   {
 *     return NULL;
 *   }
 *   variant input = core_python_py_to_variant(py_input);
 *   variant rtn = my_function(input);
 *   PyObject* py_rtn = core_python_variant_to_py(rtn);
 *   variant_free(input);
 *   variant_free(rtn);
 *   return py_rtn;
 * }
 * \endcode
 *
 * Of course, if you use the C++ interface you will not need to free the \c variant objects after usage.
 *
 *
 * <b>Files of interest</b>
 * - core_python/core_python.h
 * - core_python/core_python_wrap.h
 *
 */

/**
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

#ifdef _DEBUG
#undef _DEBUG
#define _ISDEBUG 1
#endif
#include <Python.h>
#ifdef _ISDEBUG
#undef _ISDEBUG
#define _DEBUG 1
#endif
/**
 * @brief Conversion helper from Python object to variant.
 */
variant core_python_py_to_variant(PyObject*);

/**
 * @brief Conversion helper from variant to Python object.
 */
PyObject* core_python_variant_to_py(variant);

/**
 * @brief Conversion helper from Python object to matrix.
 */
matrix core_python_py_to_matrix(PyObject*);

/**
 * @brief Conversion helper from matrix to Python object.
 */
PyObject* core_python_matrix_to_py(matrix);

/**
 * @brief Conversion helper from (keys, values) variant matrices to Python dictionary.
 */
PyObject* core_python_variants_to_py_dict(variant keys, variant values);

#endif /* CORE_PYTHON_H_ */
