#pragma once

#include <windows.h>

enum {
    thrd_success,
    thrd_nomem,
    thrd_timedout,
    thrd_busy,
    thrd_error,
};

enum {
    mtx_plain,
    mtx_recursive,
    mtx_timed,
};

typedef struct {
    HANDLE handle;
} mtx_t;

int mtx_init(mtx_t* mutex, int type);
int mtx_lock(mtx_t* mutex);
int mtx_unlock(mtx_t* mutex);

typedef struct {
    HANDLE handle;
} thrd_t;

typedef int (*thrd_start_t)(void*);

int thrd_create(thrd_t* thr, thrd_start_t func, void* arg);
int thrd_join(thrd_t thr, int* res);

enum memory_order {
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst
};

typedef LONG atomic_int;

#define atomic_fetch_add_explicit(obj, ...) _Generic(*(obj), LONG: atomic_int_fetch_add_explicit)(obj, __VA_ARGS__)

LONG atomic_int_fetch_add_explicit(volatile LONG* obj, LONG arg, enum memory_order order);
