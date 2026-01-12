import redis
import asyncio
import sys
import os
import time

from em_consts import *

async def run_serv():
    global redis_serv
    redis_serv = await asyncio.create_subprocess_shell(
        f"redis-server --port {RPORT}",
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE, env=os.environ)

asyncio.run(run_serv())

time.sleep(1)

app = redis.Redis(host="localhost", port=RPORT, decode_responses=True)
app.set("__upmx", 0)

for pr in NODE_PARS:
    if pr[1] != "in": continue
    print(gen_npar(pr[1], pr[0], pr[3]), pr[2])
    app.set(gen_npar(pr[1], pr[0], pr[3]), pr[2])

for pr in NODE_PREFS:
    app.set(gen_npref(pr[0]), pr[1])

while 1: pass