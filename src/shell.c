#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../inc/shell.h"

static char* sh_readl(void);
static char** sh_parsel(char *line);
static uint8_t sh_exec(char **args);
static uint8_t sh_launch(char **args);

static inline uint8_t sh_cd(char **args);
static inline uint8_t sh_help(char **args);
static inline uint8_t sh_exit(char **args);

static const char *builtin_cmds[] = {
    "cd",
    "help",
    "exit",
};

static uint8_t (*builtins[])(char **) = {
    &sh_cd,
    &sh_help,
    &sh_exit,
};

void 
sh_loop(void) 
{
    int8_t status;
    char **args, *line;

    do {
        fputs("$ ", stdout);

        line = sh_readl();
        args = sh_parsel(line);
        status = sh_exec(args);

        free(line);
        free(args);
    } while (status == EXIT_SUCCESS);
}

size_t 
sh_builtins_num(void) 
{
    return sizeof(builtin_cmds) / sizeof(char*);
}

static char* 
sh_readl(void) 
{
    char *line;

    line = malloc(SH_BUF_SIZE * sizeof(char));

    if (fgets(line, SH_BUF_SIZE, stdin) == NULL) {
        fputs("shell: error reading line\n", stderr);
        exit(EXIT_FAILURE);
    } else if (feof(stdin)) {
        exit(EXIT_SUCCESS);
    }

    line[strcspn(line, "\n")] = '\0';

    return line;
}

static char**
sh_parsel(char *line) 
{
    int32_t position;
    char **res, *tmp;

    position = 0;
    res = malloc(SH_BUF_SIZE * sizeof(char*));

    if (res == NULL) {
        fputs("shell: allocation error\n", stderr);
    }

    for (tmp = strtok(line, SH_ARG_DELIMS); tmp != NULL && position < SH_ARG_COUNT; 
        tmp = strtok(NULL, SH_ARG_DELIMS)) {
        res[position++] = tmp;
    }

    return res;
}


static uint8_t 
sh_exec(char **args) 
{
    size_t i;

    if (args[0] == NULL) {
        return EXIT_FAILURE;
    }

    for (i = 0; i < sh_builtins_num(); i++) {
        if (strcmp(args[0], builtin_cmds[i]) == 0) {
            return (*builtins[i])(args);
        }
    }

    return sh_launch(args);
}

static uint8_t 
sh_launch(char **args) 
{
    int32_t status;
    pid_t pid;

    pid = fork();

    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            fputs("shell: error creating child process\n", stderr);
        }
        exit(EXIT_SUCCESS);
    } else if (pid < 0) {
        fputs("shell: error creating child process\n", stderr);
    } else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return EXIT_SUCCESS;
}

static inline uint8_t 
sh_cd(char **args) 
{
    if (args[1] == NULL) {
        fputs("shell: expected argument to \"cd\"\n", stderr);
    }

    if (chdir(args[1]) != EXIT_SUCCESS) {
        fputs("shell: error changing directory\n", stderr);
    }

    return EXIT_SUCCESS;
}

static inline uint8_t 
sh_help(char **args) 
{
    size_t i;

    (void)args;

    fputs("Shell\n", stdout);
    fputs("The following commands are built in:\n", stdout);

    for (i = 0; i < sh_builtins_num(); i++) {
        fputs("\t", stdout);
        fputs(builtin_cmds[i], stdout);
        fputs("\n", stdout);
    }

    return EXIT_SUCCESS;
}

static inline 
uint8_t sh_exit(char **args) 
{
    (void)args;
    return EXIT_FAILURE;
}
