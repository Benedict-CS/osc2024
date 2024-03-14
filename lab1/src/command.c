#include "command.h"
#include "mailbox.h"
#include "mini_uart.h"
#include "string.h"
#include "utils.h"


void input_buffer_overflow_message(char cmd[]) {
    uart_puts("The following command: \"");
    uart_puts(cmd);
    uart_puts("\"... is too long to process.\n");

    uart_puts("The maximum length of input is 64.\n");
}

void command_hello() { uart_puts("Hello, World!\n"); }

void command_help() {
    uart_puts("\n");
    uart_puts("help      : print this help menu\n");
    uart_puts("hello     : print Hello World!\n");
    uart_puts("reboot    : reboot the device\n");
    uart_puts("info      : print info of board revision and ARM memory\n");
    uart_puts("\n");
}

void command_not_found(char* s) {
    uart_puts("Error: command ");
    uart_puts(s);
    uart_puts(" not found, try <help>\n");
}

void command_reboot() {
    uart_puts("Start Rebooting...\r\n");
    put32(PM_RSTC, PM_PASSWORD | 0x20); // full reset 
    put32(PM_WDOG, PM_PASSWORD | 100);  // number of watchdog tick
}
void command_info() {
    get_board_revision();
    get_ARM_memory();
}