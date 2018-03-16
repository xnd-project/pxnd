#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <ndtypes.h>
#include <plasma.h>
#include <xnd.h>

#ifndef PXND_H
#define PXND_H

xnd_master_t *pxnd_create(
    const ndt_t *t,
    uint32_t flags,
    ndt_context_t *ctx,
    PlasmaClient client,
    ObjectID object_id);

xnd_master_t *pxnd_get(
    ndt_context_t *ctx,
    PlasmaClient client,
    ObjectID object_id);


#endif /* PXND_H */
