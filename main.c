#include "timer.h"
#include "ui.h"

int main() {

    int work, breaktime, rounds;

    prompt_user(&work, &breaktime, &rounds);
    start_pomodoro(work, breaktime, rounds);

    return 0;
}