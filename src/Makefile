CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra
LDLIBS ?= -lm
TARGET = protolex
SRCS = protolex.c runtime.c runtime_io.c runtime_time.c runtime_sys.c runtime_log.c runtime_string.c runtime_int.c runtime_float.c runtime_math.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
