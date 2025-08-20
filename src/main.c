#include "ui.h"
#include "sound.h"
#include "pomodoro.h"
#include "countdown.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <wait.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <spawn.h>
#endif

#ifdef _WIN32
void open_new_terminal(int work_min, int break_min, int rounds, int socket_fd) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char command[256];

    snprintf(command, sizeof(command), "cmd.exe /c start cmd.exe /k \"timer_script %d %d %d %d\"", work_min, break_min, rounds, socket_fd);

    if (!CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        fprintf(stderr, "failed to open new terminal\n");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
#else
void open_new_terminal(int work_min, int break_min, int rounds, int socket_fd) {
    pid_t pid;
    char *argv[8];
    char work_str[11], break_str[11], rounds_str[11], socket_str[11];

    snprintf(work_str, sizeof(work_str), "%d", work_min);
    snprintf(break_str, sizeof(break_str), "%d", break_min);
    snprintf(rounds_str, sizeof(rounds_str), "%d", rounds);
    snprintf(socket_str, sizeof(socket_str), "%d", socket_fd);

    argv[0] = "/usr/bin/x-terminal-emulator";
    argv[1] = "-e";
    argv[2] = "./timer_script";
    argv[3] = work_str;
    argv[4] = break_str;
    argv[5] = rounds_str;
    argv[6] = socket_str;
    argv[7] = NULL;

    // get current env variables
    extern char **environ;
    char **env = environ;

    printf("Executing command: /usr/bin/x-terminal-emulator -e ./timer_script %s %s %s %s\n", work_str, break_str, rounds_str, socket_str); // debug

    if (posix_spawn(&pid, "/usr/bin/x-terminal-emulator", NULL, NULL, argv, env) != 0) {
        perror("posix_spawn");
        exit(EXIT_FAILURE);
    }
}
#endif

void print_environment() {
    char *display = getenv("DISPLAY");
    char *gtk_theme = getenv("GTK_THEME");
    printf("DISPLAY: %s\n", display ? display : "not set");
    printf("GTK_THEME: %s\n", gtk_theme ? gtk_theme : "not set");
}

int main() {
    // print_environment();

    int work, breaktime, rounds;
    int socket_pair[2];

    // create socket pair
    if(socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pair) == -1) {
        perror("socketpair");
        exit(EXIT_FAILURE);
    }

    prompt_user(&work, &breaktime, &rounds);

    // fork the process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // child process: runs the timer logic
        close(socket_pair[0]);  // close unused read end

        char work_str[11], break_str[11], rounds_str[11], socket_str[11];
        snprintf(work_str, sizeof(work_str), "%d", work);
        snprintf(break_str, sizeof(break_str), "%d", breaktime);
        snprintf(rounds_str, sizeof(rounds_str), "%d", rounds);
        snprintf(socket_str, sizeof(socket_str), "%d", socket_pair[1]);

        execlp("./timer_script", "./timer_script", work_str, break_str, rounds_str, socket_str, (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        // parent process: runs the main pomodoro logic
        close(socket_pair[1]);  // close unused write end
        open_new_terminal(work, breaktime, rounds, socket_pair[0]);
        printf("starting pomodoro\n");
        start_pomodoro(work, breaktime, rounds, socket_pair[0]);
        close(socket_pair[0]);  // close read end
        wait(NULL);
    }

    return 0;
}