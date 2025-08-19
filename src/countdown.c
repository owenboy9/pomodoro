#include "timer.h"
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