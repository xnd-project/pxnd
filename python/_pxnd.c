#include<stdio.h>
#include "Python.h"
#include "pxnd.h"
#include "ndtypes.h"
#include "_plasma.h"
#include "_xnd.c"

typedef struct {
    PyObject_HEAD
    PyObject *rbuf; /* resource buffer */
    ndt_t *ndt;     /* type */
} NdtObject;

// modified from mblock_empty, but replacing `xnd_empty_from_type` with
// `pxnd_create`
static MemoryBlockObject *
p_mblock_empty(PyObject *client, PyObject *object_id, PyObject *type)
{
    NDT_STATIC_CONTEXT(ctx);
    MemoryBlockObject *self;

    if (!Ndt_Check(type)) {
        PyErr_SetString(PyExc_TypeError, "expected ndt object");
        return NULL;
    }

    self = mblock_alloc();
    if (self == NULL) {
        return NULL;
    }
    self->xnd = pxnd_create(
        ((NdtObject *)(type))->ndt,
        XND_OWN_EMBEDDED,
        &ctx,
        pyPlasmaClient_client(client),
        pyObjectID_data(object_id)
    );
    if (self->xnd == NULL) {
        Py_DECREF(self);
        return (MemoryBlockObject *)seterr(&ctx);
    }
    Py_INCREF(type);
    self->type = type;
    return self;
}

static MemoryBlockObject *
p_mblock_from_typed_value(PyObject *client, PyObject *object_id, PyObject *type, PyObject *value)
{
    MemoryBlockObject *self;

    self = p_mblock_empty(client, object_id, type);
    if (self == NULL) {
        return NULL;
    }
    printf("56\n");

    if (mblock_init(&self->xnd->master, value) < 0) {
        printf("59\n");
        Py_DECREF(self);
        return NULL;
    }
    printf("63\n");

    return self;
}
static PyObject *
pypxnd_put(PyObject *m, PyObject *args)
{
    (void)m;

    PyObject * client;
    PyObject * object_id;
    PyObject * type;
    PyObject * value;
    MemoryBlockObject *mblock;

    if (!PyArg_ParseTuple(args, "OOOO", &client, &object_id, &type, &value))
        return NULL;

    mblock = p_mblock_from_typed_value(client, object_id, type, value);
    if (mblock == NULL) {
        return NULL;
    }

    return pyxnd_from_mblock(&Xnd_Type, mblock);
}

static PyMethodDef pypxnd_methods[] = {
    {"put", pypxnd_put, METH_VARARGS,
     PyDoc_STR("put(client, object_id, type, value) -> new xnd object")},
    // {"get", pypxnd_get, METH_VARARGS,
    //  PyDoc_STR("get(client, object_id) -> new xnd object")},
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wmissing-field-initializers"
    {NULL, NULL}
    #pragma clang diagnostic pop
};

PyDoc_STRVAR(pypxnd_module_doc, "Module for storing xnd in the Plasma store.");

static int
pypxnd_exec(PyObject *m)
{
    (void)m;
    return 0;
}

static struct PyModuleDef_Slot pypxnd_slots[] = {
    {Py_mod_exec, (void *)pypxnd_exec},
    {0, NULL},
};

static struct PyModuleDef pypxndmodule = {
    PyModuleDef_HEAD_INIT,
    "pxnd._pxnd",
    pypxnd_module_doc,
    0,
    pypxnd_methods,
    pypxnd_slots,
    NULL,
    NULL,
    NULL
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
PyMODINIT_FUNC
PyInit__pxnd(void)
{
    return PyModuleDef_Init(&pypxndmodule);
}
#pragma clang diagnostic pop
