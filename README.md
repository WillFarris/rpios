# A simple Raspberry Pi 3 "Kernel"

## About
Okay so it's not really complete enough to call it a kernel. This project contains things that I've learned from experimenting with OS development on the Raspberry Pi 3. Most content is from Sergey Matyukevich's [Raspberry Pi OS tutorials](https://github.com/s-matyukevich/raspberry-pi-os) and Low Level Devel's [YouTube tutorial series](https://www.youtube.com/channel/UCRWXAQsN5S3FPDHY4Ttq1Xg). My goal is to learn aarch64 assembly and bare-metal programming on ARM devices. This isn't really intended for anything usable, I'm just interested in the inner workings of computer hardware and this seems like a good way to learn.

## Status
### What's working:

* UART output
* Timer interrupts
* A simple scheduler (now working on all 4 cores!)
* A shell which can start kernel processes
* Framebuffer creation, text and simple CPU-based graphics

### What's planned:

* Usermode/EL0 process support and system calls
* DMA for faster framebuffer rendering

## Build and Run
I do all my development on Linux so YMMV if you try and run these steps on Windows/macOS, etc. These steps work on both my desktop & Raspberry Pi 4 on Manjaro 

### Toolchain
Use the [ARM GNU Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads) and `make` to compile. I use QEMU + GDB for debugging. Note that QEMU must support the `aarch64` target for debugging

## Compiling
See the `Makefile` for what specific `make` commands are doing. You'll need a file called `gccpath.mk` with the `ARMGNU` variable defined to tell Make where to find the compiler.

`make qemu` will compile the OS and run it in a VM. `make qemus` pauses QEMU on startup and waits for a GDB connection for debugging. `make gdb` runs GDB and connects to QEMU.
