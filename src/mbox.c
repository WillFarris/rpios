#include "mbox.h"
#include "types.h"
#include "regstruct.h"

volatile unsigned int  __attribute__((aligned(16))) mbox[36];

// Returns 0 on failure or the result code on success
i32 mbox_call(u8 ch)
{
    u32 r = (((unsigned int)((unsigned long)&mbox)&~0xF) | (ch&0xF));

    do{asm volatile("nop");}while(VIDEOCORE_MAILBOX->mbox_status & MBox_Full);

    VIDEOCORE_MAILBOX->mbox_write = r;

    while(1)
    {
        do{asm volatile("nop");}while(VIDEOCORE_MAILBOX->mbox_status & MBox_Empty);

        u32 mr = VIDEOCORE_MAILBOX->mbox_read;
        if (r == mr)
            return mbox[1];
    }

    return 0;
}