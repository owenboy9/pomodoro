#include "roles.h"
#include "ipc.h"
#include "termspawn.h"
#include "countdown.h"
#include "sound.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// controller: creates server, spawns terminal for timer, accepts conn, orchestrates work-break flow

int run_controller(const char *self_exe, int work, int brk, int rounds) {
    IpcEndpoint srv = {0};
    if (!ipc_server_start(&srv)) return 1;

    if (!ipc_timer_terminal(self_exe, srv.sock_path, work, brk, rounds)) {
        ipc_server_cleanup(&srv);
        return 1;
    }

    printf("waiting for timer to connect...\n");
    if(!ipc_server_accept(&srv)) {
        ipc_server_cleanup(&srv);
        return 1;
    }

    printf("timer connected :)\n");

    char line[256];

    for (int i = 0; i < rounds; ++i) {
        printf("round %d/%d -- starting WORK on timer terminal...\n", i+1, rounds);
        if (!ipc_sendf(srv.conn_fd, "RUN_WORK %d", work)) break;

        // wait for timer to finish
        if (!ipc_recvline(srv.conn_fd, line, sizeof(line))) break;
        if (strcmp(line, "WORK_DONE") != 0) {
            fprintf(stderr, "unexpected message: %s\n", line);
            break;
        }

        // last round?
        if (i == rounds -1) {
            printf("last round done. signaling END_DAY...\n");
            play_sound(END_DAY_SOUND);
            ipc_sendf(srv.conn_fd, "END_DAY");  // timer will also play the end-of-day sound
            if (!ipc_recvline(srv.conn_fd, line, sizeof(line))) break;
            if (strcmp(line, "END_ACK") != 0) fprintf(stderr, "unexpected message: %s\n", line);
            printf("done!\n");
            ipc_server_cleanup(&srv);
            return 0;
        }
        // not last round --> run BREAK locally
        printf("starting BREAK in this terminal...\n");
        countdown(brk, "break");
        play_sound(START_WORK_SOUND); // break over
        if (!ipc_sendf(srv.conn_fd, "BREAK_DONE")) break;
        // timer loops to next RUN_WORK
    }
    ipc_server_cleanup(&srv);
    return 0;
}

// timer: connect to server, run work countdowns on RUN_WORK, play break sound after each work round, signal WORK_DONE, respond to END_DAY

int run_timer(const char *sock_path, int work, int brk, int rounds) {
    (void)brk; (void)rounds;  // work length is authoritative from controller

    IpcEndpoint cli = {0};
    if (!ipc_client_connect(&cli, sock_path)) return 1;

    printf("timer connected to %s\n", sock_path);

    char line[256], cmd[64];
    int arg = 0;

    for (;;) {
        if (!ipc_recvline(cli.conn_fd, line, sizeof(line))) break;

        if (sscanf(line, "%63s %d", cmd, &arg) >= 1) {
            if (strcmp(cmd, "RUN_WORK") == 0) {
                int minutes = (sscanf(line, "%*s %d", &arg) == 1) ? arg: work;
                printf("starting WORK for %d min...\n", minutes);
                countdown(minutes, "work");
                play_sound(START_BREAK_SOUND);
                if (!ipc_sendf(cli.conn_fd, "WORK_DONE")) break;
            }
        } else if (strcmp(cmd, "BREAK_DONE") == 0) {
            printf("break finished on controller. waiting for next RUN_WORK... \n");
        } else if (strcmp(cmd, "END_DAY") == 0) {
            play_sound(END_DAY_SOUND);
            ipc_sendf(cli.conn_fd, "END_ACK");
            break;
        } else {
            fprintf(stderr, "unknown command: %s\n", line);
        }
    }
    close(cli.conn_fd);
    return 0;
}