#include "_plasma.h"
#include "plasma/client.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
typedef struct {
    PyObject_HEAD
    plasma::ObjectID data;
} PyObjectID;
#pragma clang diagnostic pop

ObjectID pyObjectID_data(PyObject *p) {
    return reinterpret_cast<ObjectID>(&(reinterpret_cast<PyObjectID *>(p)->data));
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
typedef struct {
  PyObject_HEAD
  void *__pyx_vtab;
  std::shared_ptr< plasma::PlasmaClient>  client;
  int notification_fd;
  std::string store_socket_name;
  std::string manager_socket_name;
} PyPlasmaClient;
#pragma clang diagnostic pop


PlasmaClient pyPlasmaClient_client(PyObject *p) {
    plasma::PlasmaClient * client = reinterpret_cast<PyPlasmaClient *>(p)->client.get();
    return reinterpret_cast<PlasmaClient>(client);
}
