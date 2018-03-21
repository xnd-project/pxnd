#include "Python.h"
#include "plasma.h"

#ifdef __cplusplus
extern "C" {
#endif

ObjectID pyObjectID_data(PyObject *p);

PlasmaClient pyPlasmaClient_client(PyObject *p);

#ifdef __cplusplus
}
#endif
