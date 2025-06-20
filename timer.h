#ifndef TIMER_H
#define TIMER_H

void start_pomodoro(int work_min, int break_min, int rounds);
void countdown(int minutes, const char *label);
void play_sound();

#endif // TIMER_H