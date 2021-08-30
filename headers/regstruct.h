#include "gpio.h"
#include "types.h"

struct QA7Registers
{
    reg32 control_register;
    reg32 unused0;
    reg32 core_timer_prescaler;
    reg32 gpu_interrupts_routing;
    reg32 perf_monitor_interrupts_routing_set;
    reg32 perf_monitor_interrupts_routing_clear;
    reg32 unused1;
    reg32 core_timer_access_ls;
    reg32 core_timer_access_ms;
    reg32 local_interrupt_0_routing;
    reg32 local_interrupts_5_18_routing;
    reg32 axi_outstanding_counters;
    reg32 axi_outstanding_irq;
    reg32 local_timer_ctrl_and_status;
    reg32 local_timer_write_flags;
    reg32 unused2;
    reg32 core_timer_interrupt_control[4];
    reg32 core_mailbox_interrupt_control[4];
    reg32 core_irq_source[4];
    reg32 core_fiq_source[4];
    reg32 core_mailbox_write_set[4][4];
    reg32 core_mailbox_read_write_to_clear[4][4];
};

#define QA7 ((struct QA7Registers *)(QA7_REGISTER_ADDR))

struct VideoCoreMailbox
{
    reg32 mbox_read;
    reg32 reserved[4];
    reg32 mbox_poll;
    reg32 mbox_sender;
    reg32 mbox_status;
    reg32 mbox_config;
    reg32 mbox_write;
};

#define VIDEOCORE_MAILBOX ((struct VideoCoreMailbox *)(VIDEOCORE_MAILBOX_ADDR))

typedef enum _MailBoxResponse
{
    MBoxFull = 0x80000000,
    MBoxEmpty = 0x40000000,
} MailBoxResponse;

struct LocalTimer
{
    reg32 routing;
    reg32 res0[3];
    reg32 control_status;
    reg32 set_clear_reload;
    reg32 res1;
    reg32 timer_control_core0;
    reg32 timer_control_core1;
    reg32 timer_control_core2;
    reg32 timer_control_core3;
};

#define LOCAL_TIMER ((struct LocalTimer *)(LOCAL_TIMER_ADDR))

struct SysTimerRegs
{
    reg32 timer_cs;
    reg32 timer_clo;
    reg32 timer_chi;
    reg32 timer_c0;
    reg32 timer_c1;
    reg32 timer_c2;
    reg32 timer_c3;
};

#define SYS_TIMER_REGS ((struct SysTimerRegs*)(SYS_TIMER_ADDR))

struct AuxRegs
{
    reg32 irq_status;
    reg32 enables;
    reg32 reserved[14];
    reg32 mu_io;
    reg32 mu_ier;
    reg32 mu_iir;
    reg32 mu_lcr;
    reg32 mu_mcr;
    reg32 mu_lsr;
    reg32 mu_msr;
    reg32 mu_scratch;
    reg32 mu_control;
    reg32 mu_status;
    reg32 mu_baud_rate;
};

#define REGS_AUX ((struct AuxRegs *)(AUX_REGS_ADDR))

struct GpioPinData
{
    reg32 reserved;
    reg32 data[2];
};

struct GpioRegs
{
    reg32 func_select[6];
    struct GpioPinData output_set;
    struct GpioPinData output_clear;
    struct GpioPinData level;
    struct GpioPinData ev_detect_status;
    struct GpioPinData re_detect_enable;
    struct GpioPinData fe_detect_enable;
    struct GpioPinData hi_detect_enable;
    struct GpioPinData lo_detect_enable;
    struct GpioPinData async_re_detect;
    struct GpioPinData async_fe_detect;
    reg32 reserved;
    reg32 pupd_enable;
    reg32 pupd_enable_clocks[2];
};

#define REGS_GPIO ((struct GpioRegs *)(GPIO_ADDR))

typedef enum _GpioFunc
{
    GFInput = 0,
    GFOutput = 1,
    GFAlt0 = 4,
    GFAlt1 = 5,
    GFAlt2 = 6,
    GFAlt3 = 7,
    GFAlt4 = 3,
    GFAlt5 = 2
} GpioFunc;

void gpio_pin_set_func(u8 pin, GpioFunc func);
void gpio_pin_enable(u8 pin);