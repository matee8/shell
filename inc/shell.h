#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include <stdlib.h>

#define SH_BUF_SIZE 1024
#define SH_ARG_COUNT 64
#define SH_ARG_DELIMS " \t\r\n\a"

void sh_loop(void);
size_t sh_num_of_builtins(void);

#endif
