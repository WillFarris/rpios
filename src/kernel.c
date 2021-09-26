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
#include "schedule.h"
#include "shell.h"
#include "gfx.h"
#include "reg.h"
#include "pi_logo.h"

struct FrameBuffer fb;

void core_welcome() {
    u8 core = get_core();
    u64 sp = get_sp();
    fbprintf("Core %d online with sp=0x%X\n", core, sp);
}

void test(u32 id) {
    while(1) {
        fbprintf("test proc: %d\n", id);
        sys_timer_sleep_ms(2000);
    }
}

void print_sys_info()
{
    while(1)
    {
        irq_disable();
        u32 core = get_core();

        u64 sys_timer = SYS_TIMER_REGS->timer_clo;      // Read low 32 bits
        sys_timer |= ((u64)SYS_TIMER_REGS->timer_chi << 32); // Read high 32 bits and combine
        sys_timer /= 1000;                              // Divide by 1000 to get value in ms

        u64 sec = sys_timer / 1000;
        u64 min = sec / 60;
        u64 hr  = min / 60;
        
        u64 x = fb.cursor_x[core];
        u64 y = fb.cursor_y[core];
        fb.cursor_x[core] = 0;
        fb.cursor_y[core] = fb.height - (char_height * (core+1) * 2);
        fbprintf("uptime: %d:%d:%d:%d", hr, min%60, sec%60, sys_timer%1000);
        fb.cursor_x[core] = x;
        fb.cursor_y[core] = y;
        irq_enable();
    }
    exit();
}

void draw_rects(u32 offset) {
    int i=0;//pi_logo.height;
    while(1) {// for(int i=0;i<100;++i) {
        //u32 color = wheel(pos);
        //drawRect(fb.width-100, 0, 100, 100, color);
        //pos += 1;
        u32 margin = 5;
        draw_pi_logo(fb.width-pi_logo.width - margin - offset, margin + i++);
        sys_timer_sleep_ms(50);
        if(i > fb.height) i = 0;
    }
    exit();
}

void loop_schedule() {
    //printf("Starting scheduler on core %d\n", get_core());
    while(1) schedule();
}

void kernel_main() 
{
    uart_init_alt();    
    init_printf(0, putc);

    fbinit(DISPLAY_WIDTH, DISPLAY_HEIGHT);

    printf("\n\nBooting Raspberry Pi 3\n\nBuilt "__TIME__" on "__DATE__"\n\n");
    fbprintf("\n\nBooting Raspberry Pi 3\n\nBuilt "__TIME__" on "__DATE__"\n\n");
    
    //enable_interrupt_controller();
    //fbprintf("Interrupt controller initialized\n");
    
    //sys_timer_init();
    //fbprintf("Enabled system timer\n");

    /*reg32 local_timer_core = 0;
    local_timer_init(local_timer_core, 0); // route to core local_timer_core IRQ (fiq = false)
    printf("Enabled local timer routing to core %d\n", local_timer_core);*/
    
    u8 red = fb.bg >> 16 & 0xFF;
    u8 green = fb.bg >> 8 & 0xFF;
    u8 blue = fb.bg & 0xFF;
    fbprintf("\nFrameBuffer\n  width: %d\n  height: %d\n  pitch: %d\n  background: r=%d, g=%d, b=%d\n  address: 0x%X\n\n", fb.width, fb.height, fb.pitch, red, green, blue, fb.ptr);

    //printf("Turning on MMU\n");
    //mmu_init();
    //printf("MMU enabled\n");

    QA7->control_register = 0b00 << 8;

    init_scheduler();
    core_timer_init();

    //new_process((u64) test, 420);
    //new_process((u64) shell, 0);
    
    new_process((u64) print_sys_info, 0, "sys_info");
    new_process((u64) draw_rects, 0, "raspberry_pi_logo");
    new_process((u64) shell, 0, "shell");

    //core_execute(1, loop_schedule);
    //core_execute(2, loop_schedule);
    //core_execute(3, loop_schedule);
    
    loop_schedule();
}