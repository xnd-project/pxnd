#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <string.h>
#include "ndtypes.h"
#include "pxnd.h"
#include "xnd.h"
#include "pxnd.h"

static void *pxnd_alloc(
    int64_t data_size,
    char *metadata,
    PlasmaClient client,
    ObjectID object_id)
{
    CStatus status;
    void *data;
    status = plasmaClient_create(
        client,
        object_id,
        data_size,
        (unsigned char *)metadata,
        strlen(metadata) + 1,
        (unsigned char **)&data);
    if (!status_ok(status))
    {
        return NULL;
    }
    return data;
}

static bool pxnd_free(
    ndt_context_t *ctx,
    PlasmaClient client,
    ObjectID object_id)
{
    if (!status_ok(plasmaClient_release(client, object_id)))
    {
        ndt_err_format(ctx, NDT_RuntimeError,
                       "pxnd_free: failed to release plasma object");
        return false;
    }
    return true;
}

xnd_master_t *pxnd_create(
    const ndt_t *t,
    uint32_t flags,
    ndt_context_t *ctx,
    PlasmaClient client,
    ObjectID object_id)
{
    xnd_master_t *x = xnd_empty_from_type(t, flags, ctx);
    char *ptr;

    ptr = pxnd_alloc(
        t->datasize,
        ndt_as_string(t, ctx),
        client,
        object_id);
    if (ptr == NULL)
    {
        xnd_del(x);
        ndt_memory_error(ctx);
        return NULL;
    }
    x->master.ptr = memcpy(ptr, x->master.ptr, t->datasize);
    return x;
}

// modified from xnd_empty_from_string
xnd_master_t *pxnd_get(
    ndt_context_t *ctx,
    PlasmaClient client,
    ObjectID object_id)
{
    xnd_bitmap_t b = {.data = NULL, .size = 0, .next = NULL};
    ndt_t *t;
    ObjectBuffer objectBuffer = objectBuffer_new();
    xnd_master_t *x;
    char *ptr;

    x = ndt_alloc(1, sizeof *x);
    if (x == NULL)
    {
        return ndt_memory_error(ctx);
    }

    if (!status_ok(plasmaClient_get(client, object_id, 1, 100, objectBuffer)))
    {
        ndt_err_format(ctx, NDT_RuntimeError,
                       "pxnd_get: failed to get plasma object");
        ndt_free(x);
        return NULL;
    };

    t = ndt_from_string((char *)objectBuffer_metadata(objectBuffer), ctx);
    if (t == NULL)
    {
        pxnd_free(ctx, client, object_id);
        ndt_free(x);
        return NULL;
    }

    if (!ndt_is_concrete(t))
    {
        ndt_err_format(ctx, NDT_ValueError, "type must be concrete");
        ndt_del(t);
        pxnd_free(ctx, client, object_id);
        ndt_free(x);
        return NULL;
    }
    if (xnd_bitmap_init(&b, t, ctx) < 0)
    {
        ndt_del(t);
        pxnd_free(ctx, client, object_id);
        ndt_free(x);
        return NULL;
    }
    ptr = (char *)objectBuffer_data(objectBuffer);
    if (ptr == NULL)
    {
        xnd_bitmap_clear(&b);
        ndt_del(t);
        pxnd_free(ctx, client, object_id);
        ndt_free(x);
        return NULL;
    }
    x->flags = 0;
    x->master.bitmap = b;
    x->master.index = 0;
    x->master.type = t;
    x->master.ptr = ptr;

    return x;
}
