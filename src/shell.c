#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../inc/shell.h"

static inline uint8_t shell_launch(char *const *args);

const char *builtin_commands[] = {
    "cd",
    "help",
    "exit"
};

uint8_t (*builtin_functions[]) (char *const *) = {
    &shell_change_directory,
    &shell_help,
    &shell_exit
};

void shell_loop(void) {
    char **arguments;
    char *line;
    int8_t error_status;

    do {
        fputs("$ ", stdout);

        line = shell_read_line();
        arguments = shell_parse_line(line);
        error_status = shell_execute(arguments);

        free(line);
        free(arguments);

    } while (error_status == 0);
}

char *shell_read_line(void) {
    char *buffer;
    int32_t buffer_size, position;
    int16_t c;
    
    buffer_size = SHELL_BUFFER_SIZE;
    position = 0;
    buffer = malloc(buffer_size * sizeof(char));

    if (buffer == NULL) {
        fputs("shell: allocation failed\n", stderr);
        exit(EXIT_FAILURE);
    }

    for (;;) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }

        position++;

        if (position >= buffer_size) {
            buffer_size += SHELL_BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size * sizeof(char));

            if (buffer == NULL) {
                fputs("shell: allocation failed\n", stderr);
                exit(EXIT_FAILURE);
            }

        }
    }
}

char **shell_parse_line(char *line) {
    char **tokens;
    char *token;
    int32_t buffer_size, position;

    buffer_size = SHELL_TOKEN_BUFFER_SIZE;
    position = 0;
    tokens = malloc(buffer_size * sizeof(char*));

    if (tokens == NULL) {
        fputs("shell: allocation failed\n", stderr);
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SHELL_TOKEN_DELIMITER);
    while (token != NULL) {
        tokens[position++] = token;

        if (position >= buffer_size) {
            buffer_size += SHELL_TOKEN_BUFFER_SIZE;
            tokens = realloc(tokens, buffer_size * sizeof(char*));

            if (tokens == NULL) {
                fputs("shell: allocation failed\n", stderr);
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, SHELL_TOKEN_DELIMITER);
    }
    
    tokens[position] = NULL;

    return tokens;
}


uint8_t shell_execute(char *const *args) {
    if (args[0] == NULL) {
        return 1;
    }

    for (size_t i = 0; i < shell_number_of_builtins(); i++) {
        if (strcmp(args[0], builtin_commands[i]) == 0) {
            return (*builtin_functions[i])(args);
        }
    }

    return shell_launch(args);
}

size_t shell_number_of_builtins() {
    return sizeof(builtin_commands) / sizeof(char*);
}

uint8_t shell_change_directory(char *const *args) {
    if (args[1] == NULL) {
        fputs("shell: expected argument to \"cd\"\n", stderr);
    } else {
        if (chdir(args[1]) != 0) {
            fputs("shell: error changing directory\n", stderr);
        }
    }
    return 0;
}

uint8_t shell_help(char *const *args) {
    (void)args;

    fputs("Shell\n", stdout);
    fputs("The following commands are built in:\n", stdout);

    for (size_t i = 0; i < shell_number_of_builtins(); i++) {
        puts(builtin_commands[i]);
    }

    return 0;
}

uint8_t shell_exit(char *const *args) {
    (void)args;
    return 1;
}

static inline uint8_t shell_launch(char *const *args) {
    int32_t status;
    pid_t pid, wpid;

    pid = fork();

    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            fputs("shell: error creating child process\n", stderr);
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        fputs("shell: error creating child process\n", stderr);
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 0;
}
