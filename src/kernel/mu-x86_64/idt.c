#include <mu-debug/debug.h>

#include "gdt.h"
#include "idt.h"

static Idt idt;
static IdtDesc idt_desc = {
    .len = sizeof(idt) - 1,
    .offset = (uintptr_t)&idt,
};

static void idt_init_entry(IdtEntry *self, u64 base, u8 type)
{
    self->offset_low = base & 0xFFFF;
    self->offset_mid = (base >> 16) & 0xFFFF;
    self->offset_high = (base >> 32) & 0xFFFFFFFF;
    self->ist = 0;
    self->selector = GDT_KERNEL_CODE * 8;
    self->type_attr = IDT_INT_PRESENT | type;
    self->zero = 0;
}

uintptr_t idt_descriptor(void)
{
    return (uintptr_t)&idt_desc;
}

void idt_init(void)
{
    for (usize i = 0; i < IDT_ENTRIES_LENGTH; i++)
    {
        idt_init_entry(&idt.entries[i], __interrupts_vector[i], IDT_INT_GATE);
    }

    idt_flush(idt_descriptor());
    debug_info("IDT initialized");
}
