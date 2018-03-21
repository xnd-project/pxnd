#include "Python.h"
#include "pxnd.h"
#include "ndtypes.h"
#include<stdio.h>
#include "_plasma.h"
// typedef struct {
//     PyObject_HEAD
//     ObjectID data;
// } PyObjectID;

// #define PRINT_OPAQUE_STRUCT(p)  print_mem((p), sizeof(*(p)))

// void print_mem(void const *vp, size_t n)
// {
//     unsigned char const *p = vp;
//     for (size_t i=0; i<n; i++)
//         printf("%02x\n", p[i]);
//     putchar('\n');
// }

static PyObject *
pypxnd_put(PyObject *self, PyObject *args)
{
    PyObject * py_client;
    PyObject * py_object_id;
    PyObject * data;

    if (!PyArg_ParseTuple(args, "OOO", &py_client, &py_object_id, &data))
        return NULL;
    
    ObjectID object_id = pyObjectID_data(py_object_id);
    PlasmaClient client = pyPlasmaClient_client(py_client);

    printf("object_id = %s\n", objectID_hex(object_id));
    int info;
    printf("status = %i\n", status_ok(plasmaClient_info(client, object_id, &info)));
    printf("info = %i\n", info);
    return Py_None;
}

static PyMethodDef pypxnd_methods[] = {
    {"put", pypxnd_put, METH_VARARGS,
     PyDoc_STR("put(client, object_id, data) -> new xnd object")},
    // {"get", pypxnd_get, METH_VARARGS,
    //  PyDoc_STR("get(client, object_id) -> new xnd object")},
    {NULL, NULL}
};

PyDoc_STRVAR(pypxnd_module_doc, "Module for storing xnd in the Plasma store.");

static int
pypxnd_exec(PyObject *m)
{
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

PyMODINIT_FUNC
PyInit__pxnd(void)
{
    return PyModuleDef_Init(&pypxndmodule);
}
