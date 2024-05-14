#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include <stdlib.h>

#define SHELL_BUFFER_SIZE 1024
#define SHELL_TOKEN_BUFFER_SIZE 64
#define SHELL_TOKEN_DELIMITER " \t\r\n\a"

void shell_loop(void);
char *shell_read_line(void);
char **shell_parse_line(char *line);
uint8_t shell_execute(char *const *args);
size_t shell_number_of_builtins();
uint8_t shell_change_directory(char *const *args);
uint8_t shell_help(char *const *args);
uint8_t shell_exit(char *const *args);

#endif
