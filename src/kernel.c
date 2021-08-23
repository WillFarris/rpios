#include "types.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "fb.h"
#include "font.h"
#include "timer.h"
#include "regstruct.h"
#include "math.h"

#include "mem.h"
#include "reg.h"

struct FrameBuffer fb;

void el0_entry() {
    printf("EL0 entry reached?\n");
    int x = 0;
    while(1) {
        ++x;
    }
}

void kernel_main() 
{
    uart_init_alt();    
    init_printf(0, putc);

    int fb_status = fbinit(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    u32 bg = 0x800000;
    fbclear(bg);

    printf("Booted Raspberry Pi 3 on core %d\nWe are in EL%d\n\n", get_core(), get_el());

    u64 * irq_table_addr = irq_init_vectors();
    printf("IRQ vector table initalized at 0x%x\n", irq_table_addr);
    
    enable_interrupt_controller();
    printf("Interrupt controller initialized\n");
    
    irq_enable();
    printf("Enabled IRQ interrupts\n");
    
    sys_timer_init();
    fbprintf("Enabled system timer\n");
    
    //local_timer_init();
    //printf("Enabled local timer on core %d\n", get_core());
    
    printf("\nFrameBuffer\n  width: %d\n  height: %d\n  pitch: %d\n  background: 0x%X\n  address: 0x%X\n\n", fb.width, fb.height, fb.pitch, fb.bg, fb.ptr);
    
    printf("Trying to enter EL0\n");
    enter_el0();
    //print_el();
    //printf("We just called enter_el()\n");

    //shell();
}