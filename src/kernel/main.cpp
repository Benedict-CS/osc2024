#include "board/mini-uart.hpp"
#include "cmd.hpp"
#include "fdt.hpp"
#include "heap.hpp"
#include "initramfs.hpp"

extern "C" void kernel_main(void* dtb_addr) {
  mini_uart_setup();
  mini_uart_puts("Hello Kernel!\n");

  heap_reset();
  if (!fdt.init(dtb_addr)) {
    mini_uart_printf("devicetree initialize failed\n");
    prog_hang();
  }

  initramfs.init((char*)0x20000000);

  char buf[0x100];
  for (;;) {
    mini_uart_puts("$ ");
    int len = mini_uart_getline_echo(buf, sizeof(buf));
    if (len <= 0)
      continue;
    runcmd(buf, len);
  }
}
