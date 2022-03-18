/*
 * QEMU Host Memory Backend by shared memory
 *
 * Hui Peng <peng124@purdue.edu>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */
#include "qemu/osdep.h"
#include "sysemu/hostmem.h"
#include "qapi/error.h"
#include "qom/object_interfaces.h"
#include <sys/ipc.h>
#include <sys/shm.h>

#include "afl.h"

#define TYPE_MEMORY_BACKEND_SHM "memory-backend-shm"


#ifndef MAP_SIZE
#define MAP_SIZE_POW2       16
#define MAP_SIZE            (1 << MAP_SIZE_POW2)
#endif
/*
void memory_region_init_ram_ptr(MemoryRegion *mr,
                                Object *owner,
                                const char *name,
                                uint64_t size,
                                void *ptr)
*/
static void
ram_backend_shm_alloc(HostMemoryBackend *backend, Error **errp)
{
    // const char *path;
    int shmid;

    // path = object_get_canonical_path_component(OBJECT(backend));


    if (!in_afl) {
        ACTF("QEMU: Not running in AFL, create a mock shm_ptr");
        shmid = shmget(IPC_PRIVATE, MAP_SIZE, IPC_CREAT | IPC_EXCL | 0600);
        shm_ptr = shmat(shmid, NULL, 0);
        OKF("QEMU: mock shm_area_ptr: 0x%p\n", shm_ptr);

    } else {
        OKF("QEMU: shm_area_ptr: 0x%p\n", shm_ptr);
    }
    char* name = host_memory_backend_get_name(backend);
    memory_region_init_ram_ptr(&backend->mr,
                                OBJECT(backend),
                                name,
                                MAP_SIZE,
                                shm_ptr);
    g_free(name);
}

static void
shm_backend_class_init(ObjectClass *oc, void *data)
{
    HostMemoryBackendClass *bc = MEMORY_BACKEND_CLASS(oc);

    bc->alloc = ram_backend_shm_alloc;
}

static const TypeInfo shm_backend_info = {
    .name = TYPE_MEMORY_BACKEND_SHM,
    .parent = TYPE_MEMORY_BACKEND,
    .class_init = shm_backend_class_init,
};

static void register_types(void)
{
    type_register_static(&shm_backend_info);
}

type_init(register_types);
