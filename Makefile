ROOT := $(shell pwd)
WEB_DIR := $(ROOT)/web
EMCC ?= emcc
WEB_SRCS := src/protolex.c src/runtime.c src/runtime_io.c src/runtime_time.c src/runtime_sys.c src/runtime_log.c src/runtime_string.c src/runtime_int.c src/runtime_float.c src/runtime_math.c
WEB_OUT := $(WEB_DIR)/protolex.js
WEB_FLAGS := -O2 -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=Protolex -s EXIT_RUNTIME=0 -s FORCE_FILESYSTEM=1 -s ALLOW_MEMORY_GROWTH=1 -s INVOKE_RUN=0 -s EXPORTED_RUNTIME_METHODS="['FS','callMain']"

.PHONY: all test clean web web-clean

all:
	$(MAKE) -C src

test: all
	./tests/run.sh

web: $(WEB_OUT)

$(WEB_OUT): $(WEB_SRCS)
	$(EMCC) $(WEB_FLAGS) -o $(WEB_OUT) $(WEB_SRCS) -lm --preload-file corelib@/corelib

web-clean:
	rm -f $(WEB_DIR)/protolex.js $(WEB_DIR)/protolex.wasm $(WEB_DIR)/protolex.data

clean:
	$(MAKE) -C src clean
	$(MAKE) web-clean
