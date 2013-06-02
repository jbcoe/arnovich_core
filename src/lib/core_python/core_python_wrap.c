
/**
 * @file core_python_wrap.c
 *
 * @brief Implementation of various Python helpers.
 */

#include <core_python/core_python_wrap.h>

typedef variant (*wrapped_function0)(void*);
typedef variant (*wrapped_function1)(void*, variant);
typedef variant (*wrapped_function2)(void*, variant, variant);
typedef variant (*wrapped_function3)(void*, variant, variant, variant);
typedef variant (*wrapped_function4)(void*, variant, variant, variant, variant);
typedef variant (*wrapped_function5)(void*, variant, variant, variant, variant, variant);

static char* variant_error_type(char* str)
{
	static char rtn[50];
	char* p = strtok(str, ": ");
	if(p)
	{
		strcpy(rtn, p);
		return rtn;
	}
	return "";
}

static char* variant_error_msg(char* str)
{
	static char rtn[50];
	char* p = strtok(str, ":");
	if(p)
	{
		p = strtok(NULL, ":");
		if(p)
		{
			size_t i = strspn(p, " ");
			strcpy(rtn, p+i);
			return rtn;
		}
	}
	return "";
}

// simple function with no python state 
PyObject* python_wrap_function(PyObject* args, int nargs, void* function, py_error_function err, void *self)
{
    if((args && (PyTuple_Size(args) != nargs)) || (!args && (nargs != 0)))
    {
        PyErr_Format(PyExc_StandardError, "Wrong number of arguments: expected %i got %i", nargs, (int)PyTuple_Size(args));
        return NULL;
    }
    PyObject *obj1 = NULL, *obj2 = NULL, *obj3 = NULL, *obj4 = NULL, *obj5 = NULL; //etc
    if((nargs != 0) && !PyArg_ParseTuple(args, "|OOOOO", &obj1, &obj2, &obj3, &obj4, &obj5))
    {
        PyErr_SetString(PyExc_StandardError, "Wrong number of arguments");
        return NULL;
    }

    variant rtn;
    switch(nargs)
    {
    case 0:
        rtn = ((wrapped_function0)(function))(self);
        break;
    case 1:
    {
        variant v1 = core_python_py_to_variant(obj1);
        rtn = ((wrapped_function1)(function))(self, v1);
        variant_free(v1);
        break;
    }
    case 2:
    {
        variant v1 = core_python_py_to_variant(obj1);
        variant v2 = core_python_py_to_variant(obj2);
        rtn = ((wrapped_function2)(function))(self, v1, v2);
        variant_free(v1);
        variant_free(v2);
        break;
    }
    case 3:
    {
        variant v1 = core_python_py_to_variant(obj1);
        variant v2 = core_python_py_to_variant(obj2);
        variant v3 = core_python_py_to_variant(obj3);
        rtn = ((wrapped_function3)(function))(self, v1, v2, v3);
        variant_free(v1);
        variant_free(v2);
        variant_free(v3);
        break;
    }
    case 4:
    {
        variant v1 = core_python_py_to_variant(obj1);
        variant v2 = core_python_py_to_variant(obj2);
        variant v3 = core_python_py_to_variant(obj3);
        variant v4 = core_python_py_to_variant(obj4);
        rtn = ((wrapped_function4)(function))(self, v1, v2, v3, v4);
        variant_free(v1);
        variant_free(v2);
        variant_free(v3);
        variant_free(v4);
        break;
    }
    case 5:
    {
        variant v1 = core_python_py_to_variant(obj1);
        variant v2 = core_python_py_to_variant(obj2);
        variant v3 = core_python_py_to_variant(obj3);
        variant v4 = core_python_py_to_variant(obj4);
        variant v5 = core_python_py_to_variant(obj5);
        rtn = ((wrapped_function5)(function))(self, v1, v2, v3, v4, v5);
        variant_free(v1);
        variant_free(v2);
        variant_free(v3);
        variant_free(v4);
        variant_free(v5);
        break;
    }
    }

    if(variant_is_error(rtn))
    {
		char *str = variant_as_error(rtn);
		if(!err || !err(variant_error_type(str), variant_error_msg(str)))
		{
			PyErr_SetString(PyExc_StandardError, variant_as_error(rtn));
		}
        variant_free(rtn);
		return NULL;
    }
    
    PyObject* py_rtn = core_python_variant_to_py(rtn);
    variant_free(rtn);
    return py_rtn;
}

