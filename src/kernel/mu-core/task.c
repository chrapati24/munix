#include "task.h"
#include <mu-base/std.h>
#include <mu-hal/hal.h>
#include <string.h>

#include "const.h"
#include "heap.h"
#include "pmm.h"
#include "sched.h"

MaybeTaskPtr task_init(Str path, HalSpace *space)
{
    Alloc heap = heap_acquire();
    Task *self = Try(MaybeTaskPtr, heap.malloc(&heap, sizeof(Task)));
    heap.release(&heap);

    self->state = TASK_READY;
    self->path = path;
    self->tid = sched_next_tid();
    self->space = space;

    Alloc pmm = pmm_acquire();
    self->stack = (uintptr_t)Try(MaybeTaskPtr, pmm.malloc(&pmm, align_up(STACK_SIZE, PAGE_SIZE) / PAGE_SIZE));
    pmm_release(&pmm);

    hal_space_map(space, USER_STACK_BASE, self->stack, STACK_SIZE, MU_MEM_READ | MU_MEM_WRITE | MU_MEM_USER);

    return Just(MaybeTaskPtr, self);
}

MaybeTaskPtr task_kernel(void)
{
    Alloc heap = heap_acquire();
    Task *self = Try(MaybeTaskPtr, heap.malloc(&heap, sizeof(Task)));
    heap.release(&heap);

    self->space = hal_space_kernel();
    self->state = TASK_READY;
    self->path = str$("kernel");
    self->tid = 0;

    return Just(MaybeTaskPtr, self);
}