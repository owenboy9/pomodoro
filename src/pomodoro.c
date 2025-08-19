#include "timer.h"
#include "ui.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void start_pomodoro(int work_min, int break_min, int rounds, int socket) {
    char buffer[128];
    int received;

    for (int i = 0; i < rounds; i++) {
        printf("\nwork session %d\n", i+1);
        write(socket, "start_work", 10);

        received = read(socket, buffer, sizeof(buffer));
        if (received <= 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (strcmp(buffer, "start_break") == 0) {
            printf("break\n");
            write(socket, "start_break", 11);
        }

        // wait for message to start work again
        received = read(socket, buffer, sizeof(buffer));
        if (received <= 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
    }

    printf("\ndone for today!\n");
    write(socket, "end_day", 8);

    // wait for message to confirm end of day
    received = read(socket, buffer, sizeof(buffer));
    if (received <= 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    if (strcmp(buffer, "end_day") == 0) {
        printf("great, closing sockets and exiting.\n");
    }

    close(socket);
}