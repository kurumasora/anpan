CC      = gcc
CFLAGS  = -std=gnu11 -Wall -Wextra -Wpedantic -g -pthread
LDFLAGS = -lm -pthread
TARGET  = anpan
SRCS    = main.c controller.c conveyor.c dough.c oven1.c oven2.c breadstocker.c
OBJS    = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
