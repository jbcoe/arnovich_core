
/**
 * @file core_python.c
 * 
 * @brief Implementation of Python utility functions.
 */

#include <core_python/core_python.h>

variant core_python_py_to_variant(PyObject* o)
{
    if((o == NULL) || (o == Py_None))return VARIANT_NIL();
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
            PyErr_Clear();
            return variant_from_string(s);
        }
    }
    else if(PyFloat_Check(o))
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
    else if(PyList_Check(o))
    {
        return variant_from_matrix(core_python_py_to_matrix(o));
    }
    else if(PyDict_Check(o))
    {
        return variant_from_matrix(core_python_py_to_matrix(o));
    }
    else if(PyBool_Check(o))
    {
        if(o == Py_True)
        {
            return variant_from_bool(1);
        }
        return variant_from_bool(0);
    }
    return VARIANT_NIL();
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
    if(variant_is_string(v))
    {
        return PyString_FromString(variant_as_string(v));
    }
    if(variant_is_matrix(v))
    {
        return core_python_matrix_to_py(variant_as_matrix(v));
    }
    if(variant_is_empty(v))
    {
        Py_RETURN_TRUE;
    }
    if(variant_is_bool(v))
    {
        if(variant_as_bool(v))
        {
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
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
        matrix m;
        for(;i<height;++i)
        {
            PyObject* obj = PyList_GetItem(o, i);
            if(PyList_Check(obj))
            {
                width = (width < PyList_Size(obj))?PyList_Size(obj):width;
            }
        }
        m = matrix_init(width, height);
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
                matrix_set(m, j, i, VARIANT_NIL());
            }
        }
        return m;
    }
    if(PyDict_Check(o))
    {
        int height = PyDict_Size(o), width = 2;
        matrix m = matrix_init(width, height);
        PyObject *key, *value;
        Py_ssize_t pos = 0;
        int i=0;
        while(PyDict_Next(o, &pos, &key, &value))
        {
            matrix_set(m, 0, i, core_python_py_to_variant(key));
            matrix_set(m, 1, i, core_python_py_to_variant(value));
            ++i;
        }
        return m;
    }
    return MATRIX_IDENTITY(0);
}

PyObject* core_python_matrix_to_py(matrix m)
{
    PyObject* rows;
    int height = m.m_height;
    if(height == 1)
    {
        int width = m.m_width;
        PyObject *row = PyList_New(width);
        if(row)
        {
            int j = 0;
            for(;j<width;++j)
            {
                PyList_SetItem(row, j, core_python_variant_to_py(matrix_get(m, j, 0)));
            }
        }
        return row;
    }
    rows = PyList_New(height);
    if(rows)
    {
        int width = m.m_width;
        int i=0;
        for(;i<height;++i)
        {
            PyObject *row = PyList_New(width);
            if(row)
            {
                int j = 0;
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

PyObject* core_python_variants_to_py_dict(variant vars, variant vals)
{
    PyObject* rtn;
    if(variant_is_matrix(vars) && variant_is_matrix(vals))
    {
        matrix m1 = variant_as_matrix(vars);
        matrix m2 = variant_as_matrix(vals);

        int n = m1.m_height, i = 0;
        if(n != m2.m_height) return NULL;

        rtn = PyDict_New();
        for(; i<n; ++i)
        {
            PyDict_SetItem(rtn, 
                           core_python_variant_to_py(matrix_get(m1, 0, i)),
                           core_python_variant_to_py(matrix_get(m2, 0, i))
                          );
        }

        return rtn;
    }

    return NULL;
}
