#include "mbox.h"
#include "my_string.h"
#include "uart0.h"
#include "utli.h"

enum ANSI_ESC {
    Unknown,
    CursorForward,
    CursorBackward,
    Delete
};

enum ANSI_ESC decode_csi_key() {
    char c = uart_read();
    if (c == 'C') {
        return CursorForward;
    } else if (c == 'D') {
        return CursorBackward;
    } else if (c == '3') {
        c = uart_read();
        if (c == '~') {
            return Delete;
        }
    }
    return Unknown;
}

enum ANSI_ESC decode_ansi_escape() {
    char c = uart_read();
    if (c == '[') {
        return decode_csi_key();
    }
    return Unknown;
}

void shell_init() {
    uart_init();
    uart_flush();
    uart_printf("\n[%f] Init PL011 UART done", get_timestamp());
    uart_printf("\n ____              _     _                    _           \n");
    uart_printf("| __ )  ___   ___ | |_  | |    ___   __ _  __| | ___ _ __ \n");
    uart_printf("|  _ \\ / _ \\ / _ \\| __| | |   / _ \\ / _` |/ _` |/ _ \\ '__|\n");
    uart_printf("| |_) | (_) | (_) | |_  | |__| (_) | (_| | (_| |  __/ |   \n");
    uart_printf("|____/ \\___/ \\___/ \\__| |_____\\___/ \\__,_|\\__,_|\\___|_|   \n\n");
}

void shell_input(char *cmd) {
    uart_printf("\r# ");

    int idx = 0, end = 0, i;
    cmd[0] = '\0';
    char c;
    while ((c = uart_read()) != '\n') {
        // Decode CSI key sequences
        if (c == 27) {
            enum ANSI_ESC key = decode_ansi_escape();
            switch (key) {
            case CursorForward:
                if (idx < end)
                    idx++;
                break;

            case CursorBackward:
                if (idx > 0)
                    idx--;
                break;

            case Delete:
                // left shift command
                for (i = idx; i < end; i++) {
                    cmd[i] = cmd[i + 1];
                }
                cmd[--end] = '\0';
                break;

            case Unknown:
                uart_flush();
                break;
            }
        }
        // CTRL-C
        else if (c == 3) {
            cmd[0] = '\0';
            break;
        }
        // Backspace
        else if (c == 8 || c == 127) {
            if (idx > 0) {
                idx--;
                // left shift command
                for (i = idx; i < end; i++) {
                    cmd[i] = cmd[i + 1];
                }
                cmd[--end] = '\0';
            }
        } else {
            // right shift command
            if (idx < end) {
                for (i = end; i > idx; i--) {
                    cmd[i] = cmd[i - 1];
                }
            }
            cmd[idx++] = c;
            cmd[++end] = '\0';
        }
        uart_printf("\r# %s \r\e[%dC", cmd, idx + 2);
    }

    uart_printf("\n");
}

void shell_controller(char *cmd) {
    if (!strcmp(cmd, "")) {
        return;
    } else if (!strcmp(cmd, "help")) {
        uart_printf("help: print this help menu\n");
        uart_printf("hello: print Hello World!\n");
        uart_printf("timestamp: get current timestamp\n");
        uart_printf("reboot: reboot the device\n");
        uart_printf("brn: get rpi3’s board revision number\n");
        uart_printf("bsn: get rpi3’s board serial number\n");
        uart_printf("arm_mem: get ARM memory base address and size\n");
    } else if (!strcmp(cmd, "hello")) {
        uart_printf("Hello World!\n");
    } else if (!strcmp(cmd, "reboot")) {
        uart_printf("Rebooting...");
        reset();
        while (1)
            ; // hang until reboot
    } else if (!strcmp(cmd, "brn")) {
        get_board_revision();
    } else if (!strcmp(cmd, "bsn")) {
        get_board_serial();
    } else if (!strcmp(cmd, "arm_mem")) {
        get_arm_base_memory_sz();
    } else if (!strcmp(cmd, "timestamp")) {
        uart_printf("%f\n", get_timestamp());
    } else {
        uart_printf("shell: command not found: %s\n", cmd);
    }
}
