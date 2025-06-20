#include "timer.h"
#include "sound.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void countdown(int minutes, const char *label) {
    int seconds = minutes * 60;
    if (minutes > 1) {
        printf("%s for %d minute(s)...\n", label, minutes);
    } else {
        printf("%s for one minute...\n", label);
    }

    while (seconds > 0) {
        int mins = seconds / 60;
        int secs = seconds % 60;
        printf("\r%02d:%02d remaining", mins, secs);
        fflush(stdout);
        sleep(1);
        seconds--;
    }
    printf("\r%s completed!          \n", label);
}

void start_pomodoro(int work_min, int break_min, int rounds) {
    for (int i = 0; i < rounds; i++) {
        printf("\nwork session %d\n", i + 1);
        play_sound(START_WORK_SOUND);
        countdown(work_min, "study");

        printf("break\n");
        play_sound(START_BREAK_SOUND);
        countdown(break_min, "break");
    }
    printf("\ndone for the day!\n");
    play_sound(END_WORK_SOUND);
}

void play_sound(const char *sound_file) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "mpg123 %s > /dev/null 2>&1", sound_file);
    system(cmd);
}