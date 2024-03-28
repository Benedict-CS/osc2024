#include "dtb.h"
#include "fb.h"
#include "shell.h"

#define CMD_LEN 128

enum shell_status { Read, Parse };

int main() {
  shell_init();
  // lfb_init();
  fdt_traverse(get_cpio_addr);

  enum shell_status status = Read;
  while (1) {
    char cmd[CMD_LEN];
    switch (status) {
      case Read:
        shell_input(cmd);
        status = Parse;
        break;

      case Parse:
        shell_controller(cmd);
        status = Read;
        break;
    }
  }
}