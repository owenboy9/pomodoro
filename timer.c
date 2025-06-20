#include "timer.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void countdown(int minutes, const char *label) {
    int seconds = minutes * 60;
    printf("%s for %d minute(s)...\n", label, minutes);

    while (seconds > 0) {
        int mins = seconds / 60;
        int secs = seconds % 60;
        printf("\r%02d:%02d remaining", mins, secs);
        fflush(stdout);
        sleep(1);
        seconds--;
    }
    printf("\r%s completed!          \n", label);
    play_sound();
}

void start_pomodoro(int work_min, int break_min, int rounds) {
    for (int i = 0; i < rounds; i++) {
        printf("\n pomodoro %d\n", i + 1);
        countdown(work_min, "study");
        countdown(break_min, "break");
    }
    printf("\n all pomodoros completed!\n");
}

void play_sound() {
    printf("\na");
    fflush(stdout);
}