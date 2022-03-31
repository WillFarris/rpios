SECTIONS
{
    . = 0x80000;
	.text : { *(.text.boot) *(.text) }
	.rodata : { *(.rodata) }
	.data : {
        *(.data)
    }
	. = ALIGN(8);
	bss_begin = .;
	.bss : {
		 *(.bss*)
	} 
	bss_end = .;
	.stack_core0 : {
        . = ALIGN(16);
        __stack_start_core0__ = .;
        . = . + 1024;				/* 1KB EL0 stack */
        __EL0_stack_core0 = .;
        . = . + 32768;				/* 32KB EL1 stack */
        __EL1_stack_core0 = .;
        . = . + 512;				/* 512B EL2 stack */
        __EL2_stack_core0 = .;
        __stack_end_core0__ = .;    
    }
	.stack_core1 : {
        . = ALIGN(16);
        __stack_start_core1__ = .;
        . = . + 1024;				/* 1KB EL0 stack */
        __EL0_stack_core1 = .;
        . = . + 32768;				/* 32KB EL1 stack */
        __EL1_stack_core1 = .;
        . = . + 512;				/* 512B EL2 stack */
        __EL2_stack_core1 = .;
        __stack_end_core1__ = .;    
    }
	.stack_core2 : {
        . = ALIGN(16);
        __stack_start_core2__ = .;
        . = . + 1024;				/* 1KB EL0 stack */
        __EL0_stack_core2 = .;
        . = . + 32768;				/* 32KB EL1 stack */
        __EL1_stack_core2 = .;
        . = . + 512;				/* 512B EL2 stack */
        __EL2_stack_core2 = .;
        __stack_end_core2__ = .;    
    }
	.stack_core3 : {
        . = ALIGN(16);
        __stack_start_core3__ = .;
        . = . + 1024;				/* 1KB EL0 stack */
        __EL0_stack_core3 = .;
        . = . + 32768;				/* 32KB EL1 stack */
        __EL1_stack_core3 = .;
        . = . + 512;				/* 512B EL2 stack */
        __EL2_stack_core3 = .;
        __stack_end_core3__ = .;    
    }

    . = ALIGN(65536);

	.kernel_heap : {
		.  = ALIGN(4);
		__kernel_heap_start = .;
        . = . + 16777216; /* 16MB kernel heap*/
        __kernel_heap_end = .;

        __kernel_heap_size = __kernel_heap_end - __kernel_heap_start;
	}

    . = ALIGN(65536);
    __kernel_img_end = .;

}