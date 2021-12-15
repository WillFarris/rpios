#include "proc.h"
#include "printf.h"
#include "pi_logo.h"
#include "fb.h"

void test_process(u32 id) {
    while(1) {
        printf("> test proc: %d\n", id);
        sys_timer_sleep_ms(2000);
    }
}

/*void print_sys_info()
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
        
        acquire(&fb.lock);
        u64 x = fb.cursor_x;
        u64 y = fb.cursor_y;
        fb.cursor_x = 0;
        fb.cursor_y = fb.height - (char_height * (core+1) * 2);
        release(&fb.lock);
        fbprintf("uptime: %d:%d:%d:%d", hr, min%60, sec%60, sys_timer%1000);
        acquire(&fb.lock);
        fb.cursor_x = x;
        fb.cursor_y = y;
        release(&fb.lock);
        irq_enable();
    }
    exit();
}*/

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
