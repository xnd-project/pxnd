#include <iostream>
#include "plasma/client.h"

#include "plasma.h"

extern "C" {

const size_t objectID_size = sizeof(plasma::ObjectID);

ObjectID objectID_from_random(void)
{
  plasma::ObjectID *object_id = new plasma::ObjectID;
  *object_id = plasma::ObjectID::from_random();
  return reinterpret_cast<ObjectID>(object_id);
}
const uint8_t *objectID_data(ObjectID v)
{
  return reinterpret_cast<plasma::ObjectID *>(v)->data();
}
const char *objectID_hex(ObjectID v)
{
  return reinterpret_cast<plasma::ObjectID *>(v)->hex().c_str();
}

bool status_ok(CStatus s)
{
  return reinterpret_cast<arrow::Status *>(s)->ok();
}

const char *status_to_string(CStatus s)
{
  return reinterpret_cast<arrow::Status *>(s)->ToString().c_str();
}

ObjectBuffer objectBuffer_new(void) {
  return reinterpret_cast<ObjectBuffer>(new plasma::ObjectBuffer());
}

int64_t objectBuffer_data_size(ObjectBuffer o)
{
  return reinterpret_cast<plasma::ObjectBuffer *>(o)->data_size;
}
uint8_t *objectBuffer_data(ObjectBuffer o)
{
  return reinterpret_cast<plasma::ObjectBuffer *>(o)->data.get()->mutable_data();
}
int64_t objectBuffer_metadata_size(ObjectBuffer o)
{
  return reinterpret_cast<plasma::ObjectBuffer *>(o)->metadata_size;
}
uint8_t *objectBuffer_metadata(ObjectBuffer o)
{
  return reinterpret_cast<plasma::ObjectBuffer *>(o)->metadata.get()->mutable_data();
}

CStatus plasmaClient_create(
    PlasmaClient client,
    ObjectID object_id,
    int64_t data_size,
    uint8_t *metadata,
    int64_t metadata_size,
    uint8_t **data)
{
  arrow::Status *status = new arrow::Status();
  std::shared_ptr<arrow::Buffer> dataBuffer;
  *status = reinterpret_cast<plasma::PlasmaClient *>(client)->Create(
      *reinterpret_cast<plasma::ObjectID *>(object_id),
      data_size,
      metadata,
      metadata_size,
      &dataBuffer
  );
  *data = dataBuffer.get()->mutable_data();
  return reinterpret_cast<CStatus>(status);
}
CStatus plasmaClient_get(
    PlasmaClient client,
    ObjectID object_ids,
    int64_t num_objects,
    int64_t timeout_ms,
    ObjectBuffer object_buffers)
{
  arrow::Status *status = new arrow::Status();
  *status = reinterpret_cast<plasma::PlasmaClient *>(client)->Get(
      reinterpret_cast<plasma::ObjectID *>(object_ids),
      num_objects,
      timeout_ms,
      reinterpret_cast<plasma::ObjectBuffer *>(object_buffers));
  return reinterpret_cast<CStatus>(status);
}
CStatus plasmaClient_seal(
    PlasmaClient client,
    ObjectID object_id)
{
  arrow::Status *status = new arrow::Status();
  *status = reinterpret_cast<plasma::PlasmaClient *>(client)->Seal(
          *reinterpret_cast<plasma::ObjectID *>(object_id));
  return reinterpret_cast<CStatus>(status);
}
CStatus plasmaClient_abort(
    PlasmaClient client,
    ObjectID object_id)
{
  arrow::Status *status = new arrow::Status();
  *status = reinterpret_cast<plasma::PlasmaClient *>(client)->Abort(
      *reinterpret_cast<plasma::ObjectID *>(object_id));
  return reinterpret_cast<CStatus>(status);
}
CStatus plasmaClient_release(
    PlasmaClient client,
    ObjectID object_id)
{
  arrow::Status *status = new arrow::Status();
  *status = reinterpret_cast<plasma::PlasmaClient *>(client)->Release(
      *reinterpret_cast<plasma::ObjectID *>(object_id));
  return reinterpret_cast<CStatus>(status);
}
CStatus plasmaClient_info(
    PlasmaClient client,
    ObjectID object_id,
    int* object_status)
{
  arrow::Status *status = new arrow::Status();
  *status = reinterpret_cast<plasma::PlasmaClient *>(client)->Info(
      *reinterpret_cast<plasma::ObjectID *>(object_id), object_status);
  return reinterpret_cast<CStatus>(status);
}
}




int main()
{
  // plasma::PlasmaClient client;
  // client.Connect("/tmp/store", "", 0);

  // plasma::ObjectID object_id = plasma::ObjectID::from_random();
  // // int64_t data_size = 100;
  // // uint8_t metadata[] = {5};
  // // int64_t metadata_size = sizeof(metadata);
  // // std::shared_ptr<Buffer> data;
  // // client.Create(object_id, data_size, metadata, metadata_size, &data);
  // int status;
  // printf("%i\n", );
}
