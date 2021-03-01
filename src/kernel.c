#include "types.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "mbox.h"
#include "timer.h"
#include "regstruct.h"

void kernel_main() 
{
    uart_init();
    init_printf(0, putc);

    printf("\n\nBooting Raspberry Pi 3\n");
    
    printf("\nRunning on core %d\nWe are in EL%d\n\n", get_core(), get_el());

    irq_init_vectors();
    printf("IRQ vector table initalized\n");
    
    enable_interrupt_controller();
    printf("Interrupt controller initialized\n");
    
    irq_enable();
    printf("Enabled IRQ interrupts\n");
    
    sys_timer_init();
    printf("Enabled system timer\n");
    
    //local_timer_init();
    //printf("Enabled local timer\n");
    
    printf("Trying to init framebuffer wish me luch\n");
    
    mbox[0] = 35*4;
    mbox[1] = MBOX_REQUEST;

    mbox[2] = 0x48003;  //set phy wh
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 1024;         //FrameBufferInfo.width
    mbox[6] = 768;          //FrameBufferInfo.height

    mbox[7] = 0x48004;  //set virt wh
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = 1024;        //FrameBufferInfo.virtual_width
    mbox[11] = 768;         //FrameBufferInfo.virtual_height

    mbox[12] = 0x48009; //set virt offset
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0;           //FrameBufferInfo.x_offset
    mbox[16] = 0;           //FrameBufferInfo.y.offset

    mbox[17] = 0x48005; //set depth
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32;          //FrameBufferInfo.depth

    mbox[21] = 0x48006; //set pixel order
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1;           //RGB, not BGR preferably

    mbox[25] = 0x40001; //get framebuffer, gets alignment on request
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;        //FrameBufferInfo.pointer
    mbox[29] = 0;           //FrameBufferInfo.size

    mbox[30] = 0x40008; //get pitch
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;           //FrameBufferInfo.pitch

    mbox[34] = MBOX_TAG_LAST;

    u32 width;
    u32 height;
    u32 pitch;
    u32 isrgb;
    u8 * lfb;
    //this might not return exactly what we asked for, could be
    //the closest supported resolution instead
    printf("Running mbox_call\n");
    if(mbox_call(MBOX_CH_PROP) && mbox[20]==32 && mbox[28]!=0) {
        mbox[28]&=0x3FFFFFFF;   //convert GPU address to ARM address
        width=mbox[5];          //get actual physical width
        height=mbox[6];         //get actual physical height
        pitch=mbox[33];         //get number of bytes per line
        isrgb=mbox[24];         //get the actual channel order
        lfb=(void*)((unsigned long)mbox[28]);
    } else {
        printf("Unable to set screen resolution to 1024x768x32\n");
    }

    printf("\nSetup done. Running main kernel loop.\n\n");
    
    const char* running = "Running..";
    int len = strlen(running);
    char *c = running;
    while(1)
    {
        if(c < (running+len))
            uart_putc(*c++);
        else
        {
            c = running;
            printf("\r           \r");
        }
        sys_timer_sleep_ms(100);        
    }
    
}