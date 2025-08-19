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

int main() {

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