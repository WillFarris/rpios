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
#include "tasks.h"

#include "reg.h"

void print_uptime();

struct FrameBuffer fb;

void core_welcome() {
    u8 core = get_core();
    u64 sp = get_sp();
    fbprintf("Core %d online with sp=0x%X\n", core, sp);
}

void test(u32 id) {
    while(1) {
        printf("test proc: %d\n", id);
        sys_timer_sleep_ms(2000);
    }
}

void draw_rects() {
    u32 color = 0xF00000;
    while(1) {
        drawRect(fb.width-100, 0, 100, 100, color--);
        color = color - 0xF;
        sys_timer_sleep_ms(750);
    }
}

void kernel_main() 
{
    uart_init_alt();    
    init_printf(0, putc);

    fbinit(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    //u32 bg = 0x800000;
    u32 bg = 0xF00000;
    fbclear(bg);

    fbprintf("Booting Raspberry Pi 3\n\n");
    
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

    /*fbprintf("Here are the available cores:\n\n");
    core_welcome();
    sys_timer_sleep_ms(100);
    for(int i=1;i<4;++i) {
        core_execute(i, core_welcome);
        sys_timer_sleep_ms(100);
    }*/

    QA7->control_register = 0b00 << 8;

    //core_execute(1, core_timer_init);

    //fb.cursor_y[2] = fb.cursor_y[0];
    //fb.cursor_x[2] = fb.cursor_x[0];
    //core_execute(2, shell);

    init_scheduler();
    core_timer_init();

    //new_process((u64) test, 420);
    //new_process((u64) shell, 0);
    if(!new_process((u64) print_uptime, 0))
        printf("Could not create uptime ticker\n");
    if(!new_process((u64) draw_rects, 0))
        printf("Could not create draw_rects task");
    if(!new_process((u64) shell, 0))
        printf("Could not create shell");

    //shell();

    printf("Starting scheduler\n");
    while(1) {
        //shell();
        schedule();
    }
}