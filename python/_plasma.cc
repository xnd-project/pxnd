#include "_plasma.h"
#include "plasma/client.h"

typedef struct {
    PyObject_HEAD
    plasma::ObjectID data;
} PyObjectID;

ObjectID pyObjectID_data(PyObject *p) {
    return reinterpret_cast<ObjectID>(&(reinterpret_cast<PyObjectID *>(p)->data));
}


typedef struct {
  PyObject_HEAD
  void *__pyx_vtab;
  std::shared_ptr< plasma::PlasmaClient>  client;
  int notification_fd;
  std::string store_socket_name;
  std::string manager_socket_name;
} PyPlasmaClient;


PlasmaClient pyPlasmaClient_client(PyObject *p) {
    return reinterpret_cast<PlasmaClient>(&(reinterpret_cast<PyPlasmaClient *>(p)->client));
}
