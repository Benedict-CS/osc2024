#include "alloc.h"
#include "uart.h"
#include "shell.h"

int main()
{
	alloc_init();
	uart_init();
	uart_puts("Welcome!\n");
	run_shell();
	return 0;
}