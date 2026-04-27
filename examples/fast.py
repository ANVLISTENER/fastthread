from fth import Thread
import time

def on_complete(result):
    print(f"Hello, {result}")
    pass

# 1. Start the Timer
start_time = time.perf_counter()

# 2. Run the C++ Worker
# We use 10,000,000 as before
worker = Thread(on_complete, 10000)

# 3. Join immediately
# This waits ONLY for the C++ code to finish
worker.join()

end_time = time.perf_counter()

print(f"Execution Time: {end_time - start_time:.6f} seconds")