# A simple Raspberry Pi 3 "Operating System"

## About
This repo contains all that I've learned experimenting with OS development on the Raspberry Pi 3. Most of what I learned is from a combination of Sergey Matyukevich's [Raspberry Pi OS tutorials](https://github.com/s-matyukevich/raspberry-pi-os) and Low Level Devel's [YouTube tutorials](https://www.youtube.com/channel/UCRWXAQsN5S3FPDHY4Ttq1Xg) on bare-metal programming for the Pi. The end goal is to learn some aarch64 assembly and bare-metal programming on ARM devices. This isn't really intended for anything usable. I'm just interested in the inner workings of computer hardware and this seems like a good way to learn.

## Status
So far, here's what's working:

* UART output
* The system timer (which doesn't really do anything)
* ARM generic timers, used to generate interrupts for running the scheduler
* A simple round-robin scheduler
* The entire address space is identity mapped in 64KB pages
  * This is required for aarch64's load/store exclusive instructions as memory must be marked as outer-sharable
* Framebuffer + bitmap text
* Shell with some built-in commands

## Build and Run
I do all my development on Manjaro so I don't really have a need for building for other platforms. 

### Toolchain
The [ARM GNU Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) is what I use along with `make` to compile the OS. I run the OS in QEMU for debugging, so you'll need a version of QEMU that supports aarch64.

## Compiling
See the `Makefile` for what specific `make` commands are doing. You'll want to create a file called `gccpath.mk` and add the `ARMGNU` variable to tell Make where to find the compiler.

The command `make qemu` will compile the OS and run it in a VM. `make qemus` pauses QEMU on startup and waits for a GDB connection for debugging. `make gdb` connects to QEMU and allows stepping through code.
