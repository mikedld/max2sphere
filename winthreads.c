#include "winthreads.h"

#include <stdlib.h>

typedef struct {
    thrd_start_t func;
    void* arg;
} thrd_data_t;

int mtx_init(mtx_t* mutex, int type)
{
    if (mutex == NULL || type != mtx_plain)
        return thrd_error;
    const HANDLE h = CreateMutexW(NULL, FALSE, NULL);
    if (h == NULL)
        return thrd_error;
    mutex->handle = h;
    return thrd_success;
}

int mtx_lock(mtx_t* mutex)
{
    if (mutex == NULL)
        return thrd_error;
    return WaitForSingleObject(mutex->handle, INFINITE) == WAIT_OBJECT_0 ? thrd_success : thrd_error;
}

int mtx_unlock(mtx_t* mutex)
{
    if (mutex == NULL)
        return thrd_error;
    return ReleaseMutex(mutex->handle) ? thrd_success : thrd_error;
}

static DWORD WINAPI thrd_proc(LPVOID param)
{
    thrd_data_t* const d = (thrd_data_t*)param;
    const int ret = (*d->func)(d->arg);
    free(d);
    return (DWORD)ret;
}

int thrd_create(thrd_t* thr, thrd_start_t func, void* arg)
{
    if (thr == NULL || func == NULL)
        return thrd_error;
    thrd_data_t* const d = malloc(sizeof(thrd_data_t));
    if (d == NULL)
        return thrd_nomem;
    d->func = func;
    d->arg = arg;
    const HANDLE h = CreateThread(NULL, 0, &thrd_proc, d, 0, NULL);
    if (h == NULL) {
        free(d);
        return thrd_error;
    }
    thr->handle = h;
    return thrd_success;
}

int thrd_join(thrd_t thr, int* res)
{
    if (WaitForSingleObject(thr.handle, INFINITE) != WAIT_OBJECT_0)
        return thrd_error;
    if (res != NULL) {
        DWORD ec;
        if (!GetExitCodeThread(thr.handle, &ec))
            ec = (DWORD)-1;
        *res = (int)ec;
    }
    return thrd_success;
}

LONG atomic_int_fetch_add_explicit(volatile LONG* obj, LONG arg, enum memory_order order)
{
    (void)order;
    return InterlockedAdd(obj, arg) - arg;
}
