#include "timer.h"
#include "sound.h"
#include "countdown.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void  start_timer(int work_min, int break_min, int rounds, int socket) {
    char buffer[128];
    int received;

    for (int i = 0; i < rounds; i++) {
        // wait for message to start work
        received = read(socket, buffer, sizeof(buffer));
        if (received <= 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (strcmp(buffer, "start_work") == 0) {
            play_sound(START_WORK_SOUND);
            countdown(work_min, "study");
            write(socket, "start_break", 11);
        }

        // wait for message to start break
        received = read(socket, buffer, sizeof(buffer));
        if (received <= 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (strcmp(buffer, "start_break") == 0) {
            play_sound(START_BREAK_SOUND);
            countdown(break_min, "break");
            write(socket, "start_work", 10);
        }
    }

    received = read(socket, buffer, sizeof(buffer));
    if (received <= 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    if (strcmp(buffer, "end_day") == 0) {
        play_sound(END_WORK_SOUND);
        write(socket, "end_day", 8);
    }
    close(socket);
    exit(EXIT_SUCCESS);
}