
#include <core_python/core_python_wrap.h>

typedef variant (*wrapped_function0)();
typedef variant (*wrapped_function1)(variant);
typedef variant (*wrapped_function2)(variant, variant);
typedef variant (*wrapped_function3)(variant, variant, variant);
typedef variant (*wrapped_function4)(variant, variant, variant, variant);
typedef variant (*wrapped_function5)(variant, variant, variant, variant, variant);

// simple function with no python state 
PyObject* python_wrap_function(PyObject* args, int nargs, void* function)
{
    if(PyTuple_Size(args) != nargs)
    {
        PyErr_Format(PyExc_StandardError, "Wrong number of arguments: expected %i got %i", nargs, (int)PyTuple_Size(args));
        return NULL;
    }
    PyObject *obj1 = NULL, *obj2 = NULL, *obj3 = NULL, *obj4 = NULL, *obj5 = NULL; //etc
    if(!PyArg_ParseTuple(args, "|OOOOO", &obj1, &obj2, &obj3, &obj4, &obj5))
    {
        PyErr_SetString(PyExc_StandardError, "Wrong number of arguments");
        return NULL;
    }

    variant rtn;
    switch(nargs)
    {
    case 0:
        rtn = ((wrapped_function0)(function))();
        break;
    case 1:
    {
        variant v1 = core_python_py_to_variant(obj1);
        rtn = ((wrapped_function1)(function))(v1);
        break;
    }
    case 2:
    {
        variant v1 = core_python_py_to_variant(obj1);
        variant v2 = core_python_py_to_variant(obj2);
        rtn = ((wrapped_function2)(function))(v1, v2);
        break;
    }
    case 3:
    {
        variant v1 = core_python_py_to_variant(obj1);
        variant v2 = core_python_py_to_variant(obj2);
        variant v3 = core_python_py_to_variant(obj3);
        rtn = ((wrapped_function3)(function))(v1, v2, v3);
        break;
    }
    case 4:
    {
        variant v1 = core_python_py_to_variant(obj1);
        variant v2 = core_python_py_to_variant(obj2);
        variant v3 = core_python_py_to_variant(obj3);
        variant v4 = core_python_py_to_variant(obj4);
        rtn = ((wrapped_function4)(function))(v1, v2, v3, v4);
        break;
    }
    case 5:
    {
        variant v1 = core_python_py_to_variant(obj1);
        variant v2 = core_python_py_to_variant(obj2);
        variant v3 = core_python_py_to_variant(obj3);
        variant v4 = core_python_py_to_variant(obj4);
        variant v5 = core_python_py_to_variant(obj5);
        rtn = ((wrapped_function5)(function))(v1, v2, v3, v4, v5);
        break;
    }
    }

    if(variant_is_error(rtn))
    {
        PyErr_SetString(PyExc_StandardError, variant_as_error(rtn));
        return NULL;
    }
    
    return core_python_variant_to_py(rtn);
}

