
#ifndef __CORE_PYTHON_WRAP_H__
#define __CORE_PYTHON_WRAP_H__

#include <Python.h>
#include "structmember.h"
#include <core_python/core_python.h>

// simple function with no python state 
typedef int (*py_error_function)(char* name, char* msg);
PyObject* python_wrap_function(PyObject* args, int nargs, void* function, py_error_function err, void *self);

struct Generic_Py_Struct
{
    PyObject_HEAD
    void* m_data;
};

#define DEFINE_PY_FUNCTION_BASE(NAME, FUNCTION, NARGS, DESCRIPTION) \
	static py_error_function NAME##_error = NULL; \
    static PyObject* PyFunc_##NAME(PyObject* self, PyObject* args) \
    { \
	    if(self) \
        { \
            return python_wrap_function(args, NARGS, (void*)FUNCTION, NAME##_error, ((struct Generic_Py_Struct*)self)->m_data); \
        } \
        return python_wrap_function(args, NARGS, (void*)FUNCTION, NAME##_error, NULL); \
    }\
    static char NAME##_name[] = #NAME; \
    static char NAME##_description[] = #DESCRIPTION;

#define EQUIV_PY_FUNCTION0(NAME) \
static PyObject* PyFunc_##NAME##0(PyObject* self) \
{ \
    return PyFunc_##NAME(self, NULL); \
}


#define DEFINE_PY_FUNCTION(NAME, FUNCTION, NARGS, DESCRIPTION) \
        variant NAME##_py_wrap_##FUNCTION(void* self, ...) \
        { \
            typedef variant (*wrapped_function0)(); \
            typedef variant (*wrapped_function1)(variant); \
            typedef variant (*wrapped_function2)(variant, variant); \
            typedef variant (*wrapped_function3)(variant, variant, variant); \
            typedef variant (*wrapped_function4)(variant, variant, variant, variant); \
            typedef variant (*wrapped_function5)(variant, variant, variant, variant, variant); \
            va_list vl; \
            va_start(vl, self); \
            variant rtn;\
            void* func = FUNCTION; \
            switch(NARGS) \
            { \
            case 0: \
                rtn = ((wrapped_function0)func)(); \
                break; \
            case 1: \
            { \
                variant v1 = va_arg(vl,variant); \
                rtn = ((wrapped_function1)func)(v1); \
                break; \
            } \
            case 2: \
            { \
                variant v1 = va_arg(vl,variant); \
                variant v2 = va_arg(vl,variant); \
                rtn = ((wrapped_function2)func)(v1, v2);\
                break; \
            } \
            case 3: \
            { \
                variant v1 = va_arg(vl,variant); \
                variant v2 = va_arg(vl,variant); \
                variant v3 = va_arg(vl,variant); \
                rtn = ((wrapped_function3)func)(v1, v2, v3);\
                break; \
            } \
            case 4: \
            { \
                variant v1 = va_arg(vl,variant); \
                variant v2 = va_arg(vl,variant); \
                variant v3 = va_arg(vl,variant); \
                variant v4 = va_arg(vl,variant); \
                rtn = ((wrapped_function4)func)(v1, v2, v3, v4);\
                break; \
            } \
            case 5: \
            { \
                variant v1 = va_arg(vl,variant); \
                variant v2 = va_arg(vl,variant); \
                variant v3 = va_arg(vl,variant); \
                variant v4 = va_arg(vl,variant); \
                variant v5 = va_arg(vl,variant); \
                rtn = ((wrapped_function5)func)(v1, v2, v3, v4, v5);\
                break; \
            } \
            default: \
                break; \
            } \
            va_end(vl); \
            return rtn; \
        }; \
        DEFINE_PY_FUNCTION_BASE(NAME, NAME##_py_wrap_##FUNCTION, NARGS, DESCRIPTION)

#define DEFINE_PY_OBJECT_FUNCTION(NAME, FUNCTION, NARGS, DESCRIPTION, TYPE) \
        variant NAME##_py_wrap_##FUNCTION(void* self, ...) \
        { \
            typedef variant (*wrapped_function0)(TYPE*); \
            typedef variant (*wrapped_function1)(TYPE*, variant); \
            typedef variant (*wrapped_function2)(TYPE*, variant, variant); \
            typedef variant (*wrapped_function3)(TYPE*, variant, variant, variant); \
            typedef variant (*wrapped_function4)(TYPE*, variant, variant, variant, variant); \
            typedef variant (*wrapped_function5)(TYPE*, variant, variant, variant, variant, variant); \
            va_list vl; \
            va_start(vl, self); \
            variant rtn; \
            TYPE* slf = (TYPE*)self; \
            void* func = FUNCTION; \
            switch(NARGS) \
            { \
            case 0: \
                rtn = ((wrapped_function0)func)(slf); \
                break; \
            case 1: \
            { \
                variant v1 = va_arg(vl,variant); \
                rtn = ((wrapped_function1)func)(slf, v1); \
                break; \
            } \
            case 2: \
            { \
                variant v1 = va_arg(vl,variant); \
                variant v2 = va_arg(vl,variant); \
                rtn = ((wrapped_function2)func)(slf, v1, v2);\
                break; \
            } \
            case 3: \
            { \
                variant v1 = va_arg(vl,variant); \
                variant v2 = va_arg(vl,variant); \
                variant v3 = va_arg(vl,variant); \
                rtn = ((wrapped_function3)func)(slf, v1, v2, v3);\
                break; \
            } \
            case 4: \
            { \
                variant v1 = va_arg(vl,variant); \
                variant v2 = va_arg(vl,variant); \
                variant v3 = va_arg(vl,variant); \
                variant v4 = va_arg(vl,variant); \
                rtn = ((wrapped_function4)func)(slf, v1, v2, v3, v4);\
                break; \
            } \
            case 5: \
            { \
                variant v1 = va_arg(vl,variant); \
                variant v2 = va_arg(vl,variant); \
                variant v3 = va_arg(vl,variant); \
                variant v4 = va_arg(vl,variant); \
                variant v5 = va_arg(vl,variant); \
                rtn = ((wrapped_function5)func)(slf, v1, v2, v3, v4, v5);\
                break; \
            } \
            default: \
                rtn = VARIANT_EMPTY; \
                break; \
            } \
            va_end(vl); \
            return rtn; \
        }; \
        DEFINE_PY_FUNCTION_BASE(NAME, NAME##_py_wrap_##FUNCTION, NARGS, DESCRIPTION)

#define START_PY_OBJECT2(NAME, DATA_TYPE, DATA_INIT, DATA_FREE, DATA_NEW) \
    DEFINE_PY_OBJECT_FUNCTION(DATA_NEW##_internal_new, DATA_NEW, 1, dummy, DATA_TYPE) \
    typedef struct NAME##_struct \
    { \
        PyObject_HEAD \
        DATA_TYPE* m_data; \
    } Py##NAME; \
    void NAME##_dealloc(Py##NAME* self) \
    { \
        DATA_FREE(self->m_data); \
        self->ob_type->tp_free((PyObject*)self); \
    } \
    PyObject* NAME##_new(PyTypeObject *type, PyObject *args, PyObject *kwds) \
    { \
        Py##NAME *self = (Py##NAME*)type->tp_alloc(type, 0); \
        if(self) \
        { \
            DATA_INIT(&(self->m_data)); \
            PyFunc_##DATA_NEW##_internal_new((PyObject*)self, args); \
        } \
        return (PyObject*)self; \
    } \
    int NAME##_init(Py##NAME *self, PyObject *args, PyObject *kwds) \
    { \
        return 0; \
    } \
    static PyMemberDef NAME##_members[] = { { NULL } }; \
    static PyMethodDef NAME##_methods[] = {

#define START_PY_OBJECT(NAME, DATA_TYPE, DATA_INIT, DATA_FREE) \
    static void dummy_##NAME(void* p, void* q) {} \
    START_PY_OBJECT2(NAME, DATA_TYPE, DATA_INIT, DATA_FREE, dummy)

#define ADD_OBJECT_FUNCTION(NAME) \
    {NAME##_name, (PyCFunction)PyFunc_##NAME, METH_VARARGS, NAME##_description},

#define ADD_OBJECT_FUNCTION2(NAME, FUNC) \
    {#NAME, (PyCFunction)PyFunc_##FUNC, METH_VARARGS, FUNC##_description},

#define END_PY_OBJECT(NAME) \
        {NULL,NULL,0,NULL} \
    }; \
    static PyTypeObject NAME##Type = { \
        PyObject_HEAD_INIT(NULL) \
        0,                                          /*ob_size*/ \
        #NAME,                                      /*tp_name*/ \
        sizeof(Py##NAME),                           /*tp_basicsize*/ \
        0,                                          /*tp_itemsize*/ \
        (destructor)NAME##_dealloc,                 /*tp_dealloc*/ \
        0,                                          /*tp_print*/ \
        0,                                          /*tp_getattr*/ \
        0,                                          /*tp_setattr*/ \
        0,                                          /*tp_compare*/ \
        0,                                          /*tp_repr*/ \
        0,                                          /*tp_as_number*/ \
        0,                                          /*tp_as_sequence*/ \
        0,                                          /*tp_as_mapping*/ \
        0,                                          /*tp_hash*/ \
        0,                                          /*tp_call*/ \
        0,                                          /*tp_str*/ \
        0,                                          /*tp_getattro*/ \
        0,                                          /*tp_setattro*/ \
        0,                                          /*tp_as_buffer*/ \
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /*tp_flags*/ \
        #NAME" object",                             /*tp_doc*/ \
        0,                                          /*tp_traverse*/ \
        0,                                          /*tp_clear*/ \
        0,                                          /*tp_richcompare*/ \
        0,                                          /*tp_weaklistoffset*/ \
        0,                                          /*tp_iter*/ \
        0,                                          /*tp_iternext*/ \
        NAME##_methods,                             /*tp_methods*/ \
        NAME##_members,                             /*tp_members*/ \
        0,                                          /*tp_getset*/ \
        0,                                          /*tp_base*/ \
        0,                                          /*tp_dict*/ \
        0,                                          /*tp_descr_get*/ \
        0,                                          /*tp_descr_set*/ \
        0,                                          /*tp_dictoffset*/ \
        (initproc)NAME##_init,                      /*tp_init*/ \
        0,                                          /*tp_alloc*/ \
        NAME##_new,                                 /*tp_new*/ \
    }; \
    PyTypeObject* NAME##_type(void) \
    { \
        return &NAME##Type; \
    }

#define START_PY_MODULE(NAME, DESCRIPTION) \
    static struct PyObjects { \
                    char*           m_name; \
                    PyTypeObject*   m_type; \
                  } *NAME##_objects = NULL; \
    static PyMethodDef *NAME##_methods = NULL; \
    static size_t NAME##_nmethods = 0; \
    static size_t NAME##_nobjects = 0; \
    static struct PyConstants { \
                    char*   m_name; \
                    long    m_value; \
                  } *NAME##_constants = NULL; \
    static size_t NAME##_nconstants = 0; \
    static void NAME##_at_exit() \
    { \
        free(NAME##_methods); \
        free(NAME##_objects); \
        free(NAME##_constants); \
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
    static void Py_add_object(char* name, PyTypeObject* type) \
    { \
        struct PyObjects *m = NAME##_objects; \
        NAME##_objects = (struct PyObjects*)malloc((NAME##_nobjects+1)*sizeof(struct PyObjects)); \
        if(m) \
        { \
            memcpy(NAME##_objects+1, m, NAME##_nobjects*sizeof(struct PyObjects)); \
        } \
        NAME##_objects[0].m_name = name; \
        NAME##_objects[0].m_type = type; \
        free(m); \
        NAME##_nobjects = NAME##_nobjects + 1; \
    } \
    static void Py_add_constant(char* name, long value) \
    { \
        struct PyConstants *m = NAME##_constants; \
        NAME##_constants = (struct PyConstants*)malloc((NAME##_nconstants+1)*sizeof(struct PyConstants)); \
        if(m) \
        { \
            memcpy(NAME##_constants+1, m, NAME##_nconstants*sizeof(struct PyConstants)); \
        } \
        NAME##_constants[0].m_name = name; \
        NAME##_constants[0].m_value = value; \
        free(m); \
        NAME##_nconstants = NAME##_nconstants + 1; \
    } \
    int NAME##_error(char* name, char* msg) \
    { \
        int i = 0;\
        for(;i<NAME##_nobjects;++i) \
        { \
            if(strcmp(name, NAME##_objects[i].m_name) == 0) \
            { \
                PyErr_SetString((PyObject*)NAME##_objects[i].m_type, msg); \
                return 1; \
            } \
        } \
		return 0; \
    } \
    PyMODINIT_FUNC init_##NAME(void) \
    { \
        Py_AtExit(NAME##_at_exit); \
        Py_add_method(NULL, NULL, 0, NULL); \
        char* name = "arnovich._"#NAME; \
        char* expname = "arnovich."#NAME; \
        char* description = #DESCRIPTION; \
        struct PyObjects **objects = &NAME##_objects; \
        size_t *nobjects = &NAME##_nobjects; \
        struct PyConstants **constants = &NAME##_constants; \
        size_t *nconstants = &NAME##_nconstants; \
        PyMethodDef **methods = &NAME##_methods; \
        py_error_function error_func = NAME##_error;

#define ADD_PY_FUNCTION(NAME) \
        NAME##_error = error_func; \
        Py_add_method(NAME##_name,  PyFunc_##NAME, METH_VARARGS, NAME##_description);

#define ADD_PY_EXCEPTION(NAME) \
		char *exp_##NAME = (char*)malloc(strlen(name)+strlen(#NAME)+2); \
		strcpy(exp_##NAME, expname); \
		strcat(exp_##NAME, "."#NAME); \
		Py_add_object(#NAME, (PyTypeObject*)PyErr_NewException(exp_##NAME, PyExc_StandardError, NULL)); \
		free(exp_##NAME);

#define ADD_PY_OBJECT(NAME) \
		Py_add_object(#NAME, NAME##_type());

#define ADD_PY_CONSTANT(NAME, VALUE) \
        Py_add_constant(#NAME, VALUE);

#define END_PY_MODULE \
        PyObject *module = Py_InitModule3(name, *methods, description); \
        if(module) \
        { \
            int i = 0; \
            for(;i<*nobjects;++i) \
            { \
                if(PyType_Ready((*objects)[i].m_type) >= 0) \
                { \
                    Py_INCREF((*objects)[i].m_type); \
                    PyModule_AddObject(module, (*objects)[i].m_name, (PyObject*)(*objects)[i].m_type); \
                } \
            } \
            for(i = 0;i<*nconstants;++i) \
            { \
                PyModule_AddIntConstant(module, (*constants)[i].m_name, (*constants)[i].m_value); \
            } \
        } \
    } 
        
#endif

