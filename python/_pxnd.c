#include "Python.h"
#include "pxnd.h"
#include "ndtypes.h"

/* List of functions defined in the module */

static PyObject *
pypxnd_put(PyObject *self, PyObject *args)
{
    PyObject * client;
    PyObject * object_id;
    PyObject * data;


    if (!PyArg_ParseTuple(args, "OOO", &client, &object_id, &data))
        return NULL;

    PyObject_Print(PyObject_Dir(client), stdout, Py_PRINT_RAW);
    getchar();
    PyObject_Print(PyObject_Dir(client), stdout, Py_PRINT_RAW);
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
    {Py_mod_exec, pypxnd_exec},
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
