#include "ui.h"
#include <stdio.h>

void prompt_user(int *work, int *breaktime, int *rounds) {
    printf("okay, time to start!\n");

    printf("enter work session duration in minutes: ");
    scanf("%d", work);

    printf("enter break duration in minutes: ");
    scanf("%d", breaktime);

    printf("enter number of rounds: ");
    scanf("%d", rounds);
}