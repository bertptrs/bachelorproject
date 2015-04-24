# encoding=utf-8
import atexit
import time

def endlog():
    elapsed = time.time() - start
    print("--- Runtime: %s seconds ---" % elapsed)

start = time.time()
atexit.register(endlog)
