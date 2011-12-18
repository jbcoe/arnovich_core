
#ifndef __CORE_PYTHON_WRAP_H__
#define __CORE_PYTHON_WRAP_H__

#include <Python.h>
#include <core_python/core_python.h>

// simple function with no python state 
PyObject* python_wrap_function(PyObject* self, PyObject* args, int nargs, void* function);

#define DEFINE_PY_FUNCTION(NAME, FUNCTION, NARGS, DESCRIPTION) \
    static PyObject* PyFunc_##NAME(PyObject* self, PyObject* args) \
    { \
        return python_wrap_function(self, args, NARGS, FUNCTION); \
    }\
    static char NAME##_name[] = #NAME; \
    static char NAME##_description[] = #DESCRIPTION;

#define START_PY_MODULE(NAME, DESCRIPTION) \
    static PyMethodDef *NAME##_methods = NULL; \
    static size_t NAME##_nmethods = 0; \
    static void NAME##_at_exit() \
    { \
        free(NAME##_methods); \
    } \
    static void Py_add_method(char* name, PyCFunction function, int flags, char* description) \
    { \
        PyMethodDef *m = NAME##_methods; \
        NAME##_methods = (PyMethodDef*)malloc((NAME##_nmethods+1)*sizeof(PyMethodDef)); \
        if(m) \
        { \
            memcpy(NAME##_methods+1, m, NAME##_nmethods*sizeof(PyMethodDef)); \
        } \
        NAME##_methods[0].ml_name = name; \
        NAME##_methods[0].ml_meth = function; \
        NAME##_methods[0].ml_flags = flags; \
        NAME##_methods[0].ml_doc = description; \
        free(m); \
        NAME##_nmethods = NAME##_nmethods + 1; \
    } \
    PyMODINIT_FUNC init_##NAME(void) \
    { \
        Py_AtExit(NAME##_at_exit); \
        Py_add_method(NULL, NULL, 0, NULL); \
        char* name = "arnovich._"#NAME; \
        char* description = #DESCRIPTION; \
        PyMethodDef **methods = &NAME##_methods; \

#define ADD_PY_FUNCTION(NAME) \
    Py_add_method(NAME##_name,  PyFunc_##NAME, METH_VARARGS, NAME##_description);

#define END_PY_MODULE \
        Py_InitModule3(name, *methods, description); \
    } 
        
#endif

