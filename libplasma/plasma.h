#include <stdint.h>

#ifndef LIBPLASMA_PLASMA_H
#define LIBPLASMA_PLASMA_H

#ifdef __cplusplus
extern "C" {
#endif
struct ObjectID_;
typedef struct ObjectID_ *ObjectID;
extern const size_t objectID_size;

ObjectID objectID_from_random(void);
const uint8_t *objectID_data(ObjectID v);
const char *objectID_hex(ObjectID v);

struct Status_;
typedef struct Status_ *CStatus;
bool status_ok(CStatus s);
const char *status_to_string(CStatus s);

struct ObjectBuffer_;
typedef struct ObjectBuffer_ *ObjectBuffer;
ObjectBuffer objectBuffer_new(void);
int64_t objectBuffer_data_size(ObjectBuffer o);
uint8_t *objectBuffer_data(ObjectBuffer o);
int64_t objectBuffer_metadata_size(ObjectBuffer o);
uint8_t *objectBuffer_metadata(ObjectBuffer o);

struct PlasmaClient_;
typedef struct PlasmaClient_ *PlasmaClient;
CStatus plasmaClient_create(
    PlasmaClient client,
    ObjectID object_id,
    int64_t data_size,
    uint8_t *metadata,
    int64_t metadata_size,
    uint8_t **data);
CStatus plasmaClient_get(
    PlasmaClient client,
    ObjectID object_ids,
    int64_t num_objects,
    int64_t timeout_ms,
    ObjectBuffer object_buffers);
CStatus plasmaClient_seal(
    PlasmaClient client,
    ObjectID object_id);
CStatus plasmaClient_abort(
    PlasmaClient client,
    ObjectID object_id);
CStatus plasmaClient_release(
    PlasmaClient client,
    ObjectID object_id);
CStatus plasmaClient_info(
    PlasmaClient client,
    ObjectID object_id,
    int* object_status);

#ifdef __cplusplus
}
#endif

#endif  // LIBPLASMA_PLASMA_H
