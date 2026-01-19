#!/bin/sh
set -e

ROOT=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
BIN="$ROOT/src/protolex"

run_test() {
  name=$1
  shift
  printf "test: %s\n" "$name"
  "$BIN" "$@"
}

run_test "lang_basics" "$ROOT/tests/lang_basics.plx"
run_test "lang_absence" "$ROOT/tests/lang_absence.plx"
run_test "lang_proto" "$ROOT/tests/lang_proto.plx"
run_test "lang_mutate" "$ROOT/tests/lang_mutate.plx"
run_test "lang_try" "$ROOT/tests/lang_try.plx"

run_test "lib_io" "$ROOT/tests/lib_io.plx"
run_test "lib_time" "$ROOT/tests/lib_time.plx"
run_test "lib_sys" "$ROOT/tests/lib_sys.plx" extra
run_test "lib_log" "$ROOT/tests/lib_log.plx"
