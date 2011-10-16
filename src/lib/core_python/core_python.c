
/*
 * @file core_python.c
 */

#include <core_python/core_python.h>

variant core_python_py_to_variant(PyObject* o)
{
    if(PyInt_Check(o))
    {
        return variant_from_int(PyInt_AsLong(o));
    } else if(PyString_Check(o)) {
        char *s = PyString_AsString(o);
        if(s)
        {
            PyObject *p = PyInt_FromString(s, NULL, 0);
            if(p)
            {
                return variant_from_int(PyInt_AsLong(p));
            }
        }
    }
    if(PyFloat_Check(o))
    {
        return variant_from_double(PyFloat_AsDouble(o));
    } else if(PyString_Check(o)) {
        PyObject *s = PyObject_Str(o);
        if(s)
        {
            PyObject *p = PyFloat_FromString(s, NULL);
            if(p)
            {
                return variant_from_double(PyFloat_AsDouble(p));
            }
        }
    }
    return VARIANT_NIL;
}

PyObject* core_python_variant_to_py(variant v)
{
    if(variant_is_int(v))
    {
        return PyInt_FromLong(variant_as_int(v));
    }
    if(variant_is_double(v))
    {
        return PyFloat_FromDouble(variant_as_double(v));
    }
    return NULL;
}

matrix core_python_py_to_matrix(PyObject* o)
{
    if(PyList_Check(o))
    {
        int height = PyList_Size(o);
        int i=0;
        int width = 1;
        for(;i<height;++i)
        {
            PyObject* obj = PyList_GetItem(o, i);
            if(PyList_Check(obj))
            {
                width = (width < PyList_Size(obj))?PyList_Size(obj):width;
            }
        }
        matrix m = matrix_init(width, height);
        for(i=0;i<height;++i)
        {
            PyObject* obj = PyList_GetItem(o, i);
            int j = 0;
            if(PyList_Check(obj))
            {
                int size = PyList_Size(obj);
                for(;j<size;++j)
                {
                    matrix_set(m, j, i, core_python_py_to_variant(PyList_GetItem(obj,j)));
                }
            } else {
                matrix_set(m, j, i, core_python_py_to_variant(obj));
                ++j;
            }
            for(;j<width;++j)
            {
                matrix_set(m, j, i, VARIANT_NIL);
            }
        }
        return m;
    }
    return MATRIX_IDENTITY(0);
}

PyObject* core_python_matrix_to_py(matrix m)
{
    int height = m.m_height;
    PyObject* rows = PyList_New(height);
    if(rows)
    {
        int i=0;
        int width = m.m_width;
        for(;i<height;++i)
        {
            int j = 0;
            PyObject *row = PyList_New(width);
            if(row)
            {
                for(;j<width;++j)
                {
                    PyList_SetItem(row, j, core_python_variant_to_py(matrix_get(m, j, i))); 
                }
            }
            PyList_SetItem(rows, i, row);
        }
    }
    return rows;
}

