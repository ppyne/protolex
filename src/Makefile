CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra
TARGET = protolex

.PHONY: all clean

all: $(TARGET)

$(TARGET): protolex.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
