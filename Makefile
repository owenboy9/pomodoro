CC = gcc
CFLAGS = -Wall -Wextra -std=c99

OBJS = main.o timer.o ui.o sound.o

pomodoro: $(OBJS)
	$(CC) $(CFLAGS) -o pomodoro $(OBJS)

main.o: main.c timer.h ui.h
timer.o: timer.c timer.h sound.h
ui.o: ui.c ui.h
sound.o: sound.c sound.h

clean:
	rm -f *.o pomodoro