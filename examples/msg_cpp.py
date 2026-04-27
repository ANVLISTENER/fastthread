from fth import Thread
import time

# 1. Start the Timer
start_time = time.perf_counter()

def test_func(a, b, c):
    print(f"Got: {a}, {b}, {c}")

t = Thread(test_func, "Hello", 123, [1, 2, 3])
t.join()

# 2. Stop the Timer
print(f"Execution time: {time.perf_counter() - start_time:.6f} seconds")