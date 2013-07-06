
/**
  * @file core_srv.c
  */

#include <Python.h>
#include <structmember.h>

#include <core_srv/core_srv_client.h>
#include <core_srv/core_srv_types.h>

#ifdef _WIN32
#include <Winsock2.h>
#endif

void srv_seterror(const char * msg);

struct Tick {
    PyObject_HEAD
    PyObject *data;
    double    time;
};

void Tick_dealloc(struct Tick* self)
{
    Py_XDECREF(self->data);
    self->ob_type->tp_free((PyObject*)self);
}

PyObject* Tick_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    struct Tick *self;

    self = (struct Tick*)type->tp_alloc(type, 0);
    if (self != NULL)
    {
        self->data = PyString_FromString("");
        if (self->data == NULL)
        {
            Py_DECREF(self);
            return NULL;
        }
        self->time = 0.0;
    }
    return (PyObject *)self;
}

int Tick_init(struct Tick *self, PyObject *args, PyObject *kwds)
{
    return 0;
}

static PyMemberDef Tick_members[] = {
    {"data", T_OBJECT_EX, offsetof(struct Tick, data), 0, "data"},
    {"time", T_DOUBLE, offsetof(struct Tick, time), 0, "time"},
    {NULL}
};

static PyGetSetDef Tick_getsetters[] = {
    {NULL}
};

static PyMethodDef Tick_methods[] = {
    {NULL,NULL,0,NULL}
};

static PyTypeObject TickType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "srv.Tick",         /*tp_name*/
    sizeof(struct Tick),            /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Tick_dealloc,/*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Tick object",           /* tp_doc */
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    0,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    Tick_methods,             /* tp_methods */
    Tick_members,             /* tp_members */
    Tick_getsetters,           /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)Tick_init,      /* tp_init */
    0,                          /* tp_alloc */
    Tick_new,                 /* tp_new */
};

PyTypeObject* Tick_type(void)
{
    return &TickType;
}

struct Connection {
    PyObject_HEAD
    struct srv_client_connection *m_connection;
};

PyObject* Connection_add_ticker(struct Connection *self, PyObject *args, PyObject *kwds)
{
    char         *ticker;
    int           id;
    SRV_ERROR     rtn;

    if (!PyArg_ParseTuple(args, "s", &ticker))
    {
        srv_seterror(
                "add_ticker input should be string");
        return NULL;
    }

    rtn = srv_client_add_ticker(self->m_connection, ticker, &id);

    if(rtn != SRV_ERROR_SUCCESS) {
        srv_seterror(
                "could not add ticker");
				return NULL;
    }

    return Py_BuildValue("i", id);
}

PyObject* Connection_get_ticker(struct Connection *self, PyObject *args, PyObject *kwds)
{
    char        *ticker;
    int          id;
    SRV_ERROR    rtn;

    if (!PyArg_ParseTuple(args, "s", &ticker))
    {
        srv_seterror(
                "get_ticker input should be string");
        return NULL;
    }

    rtn = srv_client_get_ticker(self->m_connection, ticker, &id);

    if(rtn != SRV_ERROR_SUCCESS) {
        srv_seterror(
                "could not get ticker");
				return NULL;
    }

    return Py_BuildValue("i", id);
}

PyObject* Connection_push_tick(struct Connection *self, PyObject *args, PyObject *kwds)
{
    int         id;
    char       *tickdata;
    SRV_ERROR   rtn;

    if (!PyArg_ParseTuple(args, "is", &id, &tickdata))
    {
        srv_seterror(
                "push_tick input should be string");
        return NULL;
    }

    rtn = srv_client_push_tick(self->m_connection, id, (void *)tickdata, (strlen(tickdata)+1)*sizeof(char));

    if(rtn != SRV_ERROR_SUCCESS) {
        srv_seterror(
                "could not push tick");
				return NULL;
    }

    Py_RETURN_TRUE;
}

PyObject* Connection_pull_tick(struct Connection *self, PyObject *args, PyObject *kwds)
{
    int                id;
    char              *tickdata;
    size_t             len;
    struct timeval     datetime;
    SRV_ERROR          rtn;
    struct Tick       *tick;

    if (!PyArg_ParseTuple(args, "i", &id))
    {
        srv_seterror(
                "pull_tick input should be string");
        return NULL;
    }

    rtn = srv_client_pull_tick(self->m_connection, id, (void **)&tickdata, &len, &datetime);

    if(rtn != SRV_ERROR_SUCCESS) {
        srv_seterror(
                "could not pull tick");
				return NULL;
    }

    //datetime.tv_sec * 1.0 + datetime.tv_usec * 1.0e-6
    //return Py_BuildValue("{s:s,s:d}", "Data", tickdata, "Time", ((double)datetime.tv_sec));
	tick = (struct Tick*)Tick_new(&TickType, NULL, NULL);
	tick->data = PyString_FromString(tickdata);
	tick->time = (double)datetime.tv_sec;
	return (PyObject*)tick;
}

PyObject* Connection_pull_tick_update(struct Connection *self, PyObject *args, PyObject *kwds)
{
    int                id;
    char              *tickdata;
    size_t             len;
    struct timeval     datetime;
    SRV_ERROR          rtn;
    struct Tick       *tick;

    if (!PyArg_ParseTuple(args, "i", &id))
    {
        srv_seterror(
                "pull_tick input should be string");
        return NULL;
    }

    rtn = srv_client_pull_tick_update(self->m_connection, id, (void **)&tickdata, &len, &datetime);

    if(rtn != SRV_ERROR_SUCCESS) {
        srv_seterror(
                "could not pull tick");
				return NULL;
    }

	tick = (struct Tick*)Tick_new(&TickType, NULL, NULL);
	tick->data = PyString_FromString(tickdata);
	tick->time = (double)datetime.tv_sec;
	return (PyObject*)tick;
}

PyObject* Connection_subscribe(struct Connection *self, PyObject *args, PyObject *kwds)
{
    int                id;
    SRV_ERROR          rtn;

    if (!PyArg_ParseTuple(args, "i", &id))
    {
        srv_seterror("subscribe input should be integer id");
        return NULL;
    }

    rtn = srv_client_subscribe(self->m_connection, id);

    if(rtn != SRV_ERROR_SUCCESS)
    {
        srv_seterror("could not subscribe");
				return NULL;
    }

    return Py_True;
}

PyObject* Connection_unsubscribe(struct Connection *self, PyObject *args, PyObject *kwds)
{
    int                id;
    SRV_ERROR          rtn;

    if (!PyArg_ParseTuple(args, "i", &id))
    {
        srv_seterror("unsubscribe input should be integer id");
        return NULL;
    }

    rtn = srv_client_unsubscribe(self->m_connection, id);

    if(rtn != SRV_ERROR_SUCCESS)
    {
        srv_seterror("could not unsubscribe");
				return NULL;
    }

    return Py_True;
}

PyObject* Connection_wait_for_update(struct Connection *self, PyObject *args, PyObject *kwds)
{
    int                id;
    char              *tickdata;
    size_t             len;
    struct timeval     datetime;
    SRV_ERROR          rtn;
    struct Tick       *tick;

    if (!PyArg_ParseTuple(args, "i", &id))
    {
        srv_seterror("wait_for_update input should be integer id");
        return NULL;
    }

	rtn = SRV_ERROR_SUCCESS;

	Py_BEGIN_ALLOW_THREADS
    rtn = srv_client_wait_for_tick(self->m_connection, (void **)&tickdata, &len, &datetime);
	Py_END_ALLOW_THREADS

    if(rtn != SRV_ERROR_SUCCESS)
    {
        srv_seterror("could not wait for update");
        return NULL;
    }

	tick = (struct Tick*)Tick_new(&TickType, NULL, NULL);
	tick->data = PyString_FromString(tickdata);
	tick->time = (double)datetime.tv_sec;
	return (PyObject*)tick;
}

void Connection_dealloc(struct Connection* self)
{
    srv_client_close_connection(self->m_connection);
    self->ob_type->tp_free((PyObject*)self);
};

PyObject* Connection_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    struct Connection *self;
    SRV_ERROR          rtn;

    self = (struct Connection*)type->tp_alloc(type, 0);
    if (self != NULL)
    {
        //SRV_ERROR rtn = srv_client_connect(&self->m_connection, SRV_SOCKET_TYPES_TCP_IP);
        rtn = srv_client_connect(&self->m_connection, SRV_SOCKET_TYPES_LOCAL_PIPES);
        if(rtn != SRV_ERROR_SUCCESS) {
            srv_seterror(
                    "could not create connection");
                        return NULL;
        }
    }

    return (PyObject *)self;
};

int Connection_init(struct Connection *self, PyObject *args, PyObject *kwds)
{
    return 0;
};

static PyMemberDef Connection_members[] = {
    {NULL}
};

static PyGetSetDef Connection_getsetters[] = {
    {NULL}
};

static PyMethodDef Connection_methods[] = {
    {"add_ticker", (PyCFunction)Connection_add_ticker, METH_VARARGS,
     "add_ticker(ticker)\n\n add a ticker to the data server"
    },
    {"get_ticker", (PyCFunction)Connection_get_ticker, METH_VARARGS,
     "get_ticker(ticker)\n\n get a ticker from the data server"
    },
    {"push_ticker", (PyCFunction)Connection_push_tick, METH_VARARGS,
     "push_ticker(id, tickdata)\n\n push a ticker to the data server"
    },
    {"pull_tick", (PyCFunction)Connection_pull_tick, METH_VARARGS,
     "pull_tick(id)\n\n pull a ticker from the data server"
    },
    {"pull_tick_update", (PyCFunction)Connection_pull_tick_update, METH_VARARGS,
     "pull_tick_update(id)\n\n pull a ticker update from the data server"
    },
    {"subscribe", (PyCFunction)Connection_subscribe, METH_VARARGS,
     "subscribe(id)\n\n subscribe to ticker"
    },
    {"unsubscribe", (PyCFunction)Connection_unsubscribe, METH_VARARGS,
     "unsubscribe(id)\n\n unsubscribe from ticker"
    },
    {"wait_for_update", (PyCFunction)Connection_wait_for_update, METH_VARARGS,
     "wait_for_update(id)\n\n wait for subscribed ticker update"
    },

    {NULL,NULL,0,NULL}
};

static PyTypeObject ConnectionType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "srv.Connection",         /*tp_name*/
    sizeof(struct Connection),            /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Connection_dealloc,/*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Connection object",           /* tp_doc */
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    0,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    Connection_methods,             /* tp_methods */
    Connection_members,             /* tp_members */
    Connection_getsetters,           /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)Connection_init,      /* tp_init */
    0,                          /* tp_alloc */
    Connection_new,                 /* tp_new */
};

PyTypeObject* Connection_type(void)
{
    return &ConnectionType;
}
