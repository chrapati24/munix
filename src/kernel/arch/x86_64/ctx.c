#include <abstract/const.h>
#include <abstract/ctx.h>
#include <abstract/entry.h>
#include <core/pmm.h>
#include <debug/debug.h>
#include <misc/lock.h>

#include "asm.h"
#include "gdt.h"

static Spinlock lock;

void context_init(Context *self, uintptr_t ip, TaskArgs args)
{
    // self->regs.cs = (GDT_KERNEL_CODE * 8);
    self->regs.cs = (GDT_USER_CODE * 8) | 3;
    // self->regs.ss = (GDT_KERNEL_DATA * 8);
    self->regs.ss = (GDT_USER_DATA * 8) | 3;
    self->regs.rip = ip;
    self->regs.rsp = USER_STACK_BASE + STACK_SIZE;
    self->regs.rbp = USER_STACK_BASE;
    self->regs.rflags = 0x202;

    self->regs.rdi = args.arg1;
    self->regs.rsi = args.arg2;
    self->regs.rdx = args.arg3;
    self->regs.rcx = args.arg4;
    self->regs.r8 = args.arg5;

    Alloc pmm = pmm_acquire();

    self->syscall_kernel_bstack = abstract_apply_hhdm((uintptr_t)non_null$(pmm.calloc(&pmm, 1, PAGE_SIZE)));
    self->syscall_kernel_stack = self->syscall_kernel_bstack + STACK_SIZE;
    pmm.release(&pmm);
}

void context_save(Context *self, Regs *regs)
{
    self->regs = *regs;
}

void context_switch(Context *ctx, Regs *regs)
{
    spinlock_acquire(&lock);

    asm_write_msr(MSR_GS_BASE, (uintptr_t)ctx);
    asm_write_msr(MSR_KERN_GS_BASE, (uintptr_t)ctx);

    *regs = ctx->regs;

    spinlock_release(&lock);
}
