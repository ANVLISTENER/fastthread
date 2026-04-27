import thread

def hello(msg):
    print(f"Hello {msg}")

t = thread.Thread(hello, "amma")
t.join()
print("Clean exit!")