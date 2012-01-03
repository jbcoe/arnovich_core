
#ifndef __CORE_PYTHON_WRAP_H__
#define __CORE_PYTHON_WRAP_H__

#include <Python.h>
#include <core_python/core_python.h>

// simple function with no python state 
PyObject* python_wrap_function(PyObject* args, int nargs, void* function);

//TODO PY_EXCEPTION construct is crashing
//TODO PY_Object adder is not handling self-referential functions
//TODO for now: if self=NULL in funciton-wrapper then cast data-member to void* and pass as first 
//argument to wrapped function

#define DEFINE_PY_FUNCTION(NAME, FUNCTION, NARGS, DESCRIPTION) \
    static PyObject* PyFunc_##NAME(PyObject* self, PyObject* args) \
    { \
        return python_wrap_function(args, NARGS, FUNCTION); \
    }\
    static char NAME##_name[] = #NAME; \
    static char NAME##_description[] = #DESCRIPTION;

#define START_PY_OBJECT(NAME, DATA_TYPE, DATA_INIT, DATA_FREE) \
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
            DATA_INIT(&self->m_data); \
        } \
        return (PyObject*)self; \
    } \
    int NAME##_init(Py##NAME *self, PyObject *args, PyObject *kwds) \
    { \
        return 0; \
    } \
    static PyMemberDef NAME##_members[] = { {NULL} }; \
    static PyMemberDef NAME##_getsetters[] = { {NULL} }; \
    static PyMethodDef NAME##_methods[] = {

#define ADD_OBJECT_FUNCTION(NAME) \
    {NAME##_name, (PyCFunction)PyFunc_##NAME, METH_VARARGS, NAME##_description},

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
        NAME##_getsetters,                          /*tp_getset*/ \
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
    static void NAME##_at_exit() \
    { \
        free(NAME##_methods); \
        free(NAME##_objects); \
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
    void NAME##_error(char* name, char* msg) \
    { \
        int i = 0;\
        for(;i<NAME##_nobjects;++i) \
        { \
            if(strcmp(name, NAME##_objects[i].m_name) == 0) \
            { \
                PyErr_SetString((PyObject*)NAME##_objects[i].m_type, msg); \
            } \
        } \
    } \
    PyMODINIT_FUNC init_##NAME(void) \
    { \
        Py_AtExit(NAME##_at_exit); \
        Py_add_method(NULL, NULL, 0, NULL); \
        char* name = "arnovich._"#NAME; \
        char* description = #DESCRIPTION; \
        struct PyObjects **objects = &NAME##_objects; \
        size_t *nobjects = &NAME##_nobjects; \
        PyMethodDef **methods = &NAME##_methods; \

#define ADD_PY_FUNCTION(NAME) \
    Py_add_method(NAME##_name,  PyFunc_##NAME, METH_VARARGS, NAME##_description);

#define ADD_PY_EXCEPTION(NAME) \
    Py_add_object(#NAME, (PyTypeObject*)PyErr_NewException(#NAME, PyExc_StandardError, NULL));

#define ADD_PY_OBJECT(NAME) \
    Py_add_object(#NAME, NAME##_type());

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
        } \
    } 
        
#endif

