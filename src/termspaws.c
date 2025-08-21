#define _GNU_SOURCE
#include "termspawn.h"
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

int spawn_timer_terminal(const char *self_exe, const char *sock_path, int work_min, int break_min, int rounds) {
    char w[16], b[16], r[16];
    snprintf(w, sizeof(w), "%d", work_min);
    snprintf(b, sizeof(b), "%d", break_min);
    snprintf(r, sizeof(r), "%d", rounds);

    // choose gnome-terminal if present, otherwise fallback to xterm
    const char *term = "gnome-terminal";
    char *argv1[] = {
        (char*)term, (char*)"--", (char*)self_exe, (char*)"--timer", (char*)sock_path, w, b, r, NULL
    };
    pid_t pid;
    int rc = posix_spawn(&pid, term, NULL, NULL, argv1, environ);
    if (rc == 0) return 1;

    // fallback
    term = "xterm";
    char *argv2[] = {
        (char*)term, (char*)"-e", (char*)self_exe, (char*)"--timer", (char*)sock_path, w, b, r, NULL
    };

    rc = posix_spawn(&pid, term, NULL, NULL, argv2, environ);
    if (rc != 0) {
        perror("posix_spawn");
        return 0;
    }
    return 1;
}