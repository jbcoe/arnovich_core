/*
 * @file core_main.c
 */

#include <Python.h>

#define DEBUG

#include <core_debug/core_debug.h>

static PyObject *CoreError;

void core_setaterror(const char * msg)
{
	PyErr_SetString(CoreError, msg);
}


PyObject* Core_set_level(PyObject *self, PyObject *args, PyObject *kwds)
{
	int level;

    if(!PyArg_ParseTuple(args, "i", &level))
    {
    	core_setaterror("Invalid debug level input");
        return NULL;
    }

    _SET_DEBUG_LEVEL(level);

    Py_RETURN_TRUE;
}

PyObject* Core_set_group(PyObject *self, PyObject *args, PyObject *kwds)
{
	int group;

    if(!PyArg_ParseTuple(args, "i", &group))
    {
    	core_setaterror("Invalid debug group input");
        return NULL;
    }

    _SET_DEBUG_GROUP(group);

    Py_RETURN_TRUE;
}

static PyMethodDef CoreMethods[] = {
	{"set_debug_level", (PyCFunction)Core_set_level, METH_VARARGS,
     "set_debug_level(level)\n\n Set debug level"
	},
	{"set_debug_group", (PyCFunction)Core_set_group, METH_VARARGS,
     "set_debug_group(group)\n\n Set debug group"
	},
	{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_core(void)
{
    PyObject* m;

    m = Py_InitModule3(
    		"arnovich._core",
    		CoreMethods,
    		"Module for Core");

    if (m == NULL)
      return;

    CoreError = PyErr_NewException("Core.error", PyExc_StandardError, NULL);
    Py_INCREF(CoreError);
    PyModule_AddObject(m, "Core error", CoreError);

}

int main(int argc, char *argv[])
{
    Py_SetProgramName(argv[0]);

    Py_Initialize();

    init_core();
    return 0;
}
