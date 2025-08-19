#include "timer.h"
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
void open_new_terminal() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL, "cmd.exe /c start cmd.exe /k \"./dist/pomodoro\"", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        fprintf(stderr, "Failed to open new terminal\n");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
#else
void open_new_terminal() {
    pid_t pid;
    char *argv[] = {"/usr/bin/x-terminal-emulator", "-e", "./dist/pomodoro", NULL};
    char *envp[] = {NULL};

    if (posix_spawn(&pid, "/usr/bin/x-terminal-emulator", NULL, NULL, argv, envp) != 0) {
        perror("posix_spawn");
        exit(EXIT_FAILURE);
    }
}
#endif

int main() {

    int work, breaktime, rounds;
    int socket_pair[2];

    // create socket pair
    if(socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pair) == -1) {
        perror("socketpair");
        exit(EXIT_FAILURE);
    }

    prompt_user(&work, &breaktime, &rounds);

    open_new_terminal();

    // fork the process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // child process: runs the timer logic
        close(socket_pair[0]);  // close unused read end
        start_timer(work, breaktime, rounds, socket_pair[1]);
        close(socket_pair[1]);  // close write end
        exit(EXIT_SUCCESS);
    } else {
        // parent process: runs the main pomodoro logic
        close(socket_pair[1]);  // close unused write end
        start_pomodoro(work, breaktime, rounds, socket_pair[0]);
        close(socket_pair[0]);  // close read end
        wait(NULL);
    }

    return 0;
}