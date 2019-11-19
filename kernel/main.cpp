/**
 * @file main.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The entry point into the Panix kernel. Everything is loaded from here.
 * @version 0.1
 * @date 2019-11-14
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */
// System library functions
#include <sys/sys.hpp>
// Intel i386 architecture
#include <arch/i386/gdt.hpp>
#include <arch/i386/idt.hpp>
#include <arch/i386/isr.hpp>
#include <arch/i386/timer.hpp>
// Generic devices
#include <devices/smbios/smbios.hpp>
#include <devices/kbd/kbd.hpp>
#include <devices/rtc/rtc.hpp>

void px_kernel_print_splash();

/**
 * @brief Global constructor called from the boot assembly
 * 
 */
typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void px_call_constructors() {
    for (constructor* i = &start_ctors; i != &end_ctors; i++) {
        (*i)();
    }
}

/**
 * @brief This is the Panix kernel entry point. This function is called directly from the
 * assembly written in boot.S located in kernel/arch/i386/boot.S.
 * 
 */
extern "C" void px_kernel_main(const void* multiboot_structure, uint32_t multiboot_magic) {
    // Print the splash screen to show we've booted into the kernel properly.
    px_kernel_print_splash();
    kprintSetColor(Blue, Black);
    // Install the GDT
    px_gdt_install() ? px_print_debug("Loaded GDT.", Success) : panic("Unable to install the GDT!");
    // @todo Make success and fail conditions for all of these and fix SMBIOS
    char* smbios_addr = px_get_smbios_addr();
                                // Begin installs and inits
    px_isr_install();           // Interrupt Service Requests
    px_kbd_init();              // Keyboard
    px_rtc_init();              // Real Time Clock
    px_timer_init(60);          // Programmable Interrupt Timer
    px_interrupts_enable();     // Enable interrupts
    // Print some info to show we did things right
    px_rtc_print();
    px_print_debug("Done.", Success);
    while (true) {
        
    }
    panic("0xDEADDEAD\nKernel terminated unexpectedly.");
}

void px_kernel_print_splash() {
    clearScreen();
    kprintSetColor(Yellow, Black);
    kprint("Welcome to Panix\n");
    kprint("Developed by graduates and undergraduates of Cedarville University.\n");
    kprint("Copyright Keeton Feavel et al (c) 2019. All rights reserved.\n\n");
    kprintSetColor(LightCyan, Black);
    kprint("Gloria in te domine, Gloria exultate\n\n");
    kprintSetColor(White, Black);
}