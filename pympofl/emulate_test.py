import time
import os
import redis
from em_consts import *

app = redis.Redis(host="localhost", port=RPORT, decode_responses=True)
os.system(f"python3 test-script.py rport={RPORT} node={NODE_ID}")


for pr in NODE_PARS:
    if pr[1] != "out": continue
    print(gen_npar(pr[0]), app.get(gen_npar(pr[0])))