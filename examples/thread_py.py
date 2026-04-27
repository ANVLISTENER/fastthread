from threading import Thread

def hello(msg):
    print(f"Hello {msg}")

t1 = Thread(target=hello, args=("amma",))

t1.start()

t1.join()
print("Clean exit!")