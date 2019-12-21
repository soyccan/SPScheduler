CC=gcc
CFLAGS=-Wall -Wextra -Wconversion -std=gnu18
SRC=main.c hw3.c
OBJ=scheduler.o

all: release

release: $(SRC) $(OBJ)
	$(CC) $(CFLAGS) -D NDEBUG -g -o hw3 scheduler.o hw3.c
	$(CC) $(CFLAGS) -D NDEBUG -g -o main main.c

debug: $(SRC) $(OBJ)
	$(CC) $(CFLAGS) -D DEBUG -g -o hw3 scheduler.o hw3.c
	$(CC) $(CFLAGS) -D DEBUG -g -o main main.c
