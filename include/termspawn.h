#ifndef TERMSPAWN_H
#define TERMSPAWN_H

#include <stddef.h>

int spawn_timer_terminal(const char *self_exe, const char *sock_path, int work_min, int break_min, int rounds);

#endif // TERMSPAWN_H