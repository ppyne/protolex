import time from "runtime/time"

start = time.monotonic()
time.sleep(50)
finish = time.monotonic()
elapsed = finish - start
