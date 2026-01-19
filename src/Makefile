CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra
TARGET = protolex
SRCS = protolex.c runtime.c runtime_io.c runtime_time.c runtime_sys.c runtime_log.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
