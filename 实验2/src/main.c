#include <aarch64/intrinsic.h>
#include <core/console.h>
#include <common/string.h>
NORETURN void main() {
	/* TODO: Lab2 set bss */
	extern char edata[], end[];
	

    init_char_device();
    init_console();
	
	/* TODO: Lab2 print */
	
}
