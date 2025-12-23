import os
import json
import sys
import asyncio
import sys
import subprocess
import uuid
from flask_socketio import emit, join_room
import copy

cfg = {
    "allow-reload": True, # True = allow runtime reloads of active packs via /reload
    "pack-path": "/home/sirtage/all/projects/umpof/tpacks", # Path to the packs on server
    "runtime-debug": False # True = if server contains a core umpof service for debugging
}

USER_WEB_LINKER = {}

BUILD_SESSIONS = {}

mpwd = None
packs = []

def cont_t(pth):
    ds = os.listdir(pth)
    for v in ds:
        if not os.path.isdir(v) and v.endswith("pack.json"): return True
    return False

def proc_dir(pth):
    if cont_t(pth):
        with open(f"{pth}/pack.json", 'r') as f:
            packs.append(json.loads(f.read()))
    else:
        for v in os.listdir(pth):
            m = f"{pth}/{v}"
            if os.path.isdir(m): proc_dir(m)

def load():
    global mpwd, packs
    mpwd = sys.path[0]
    packs = []
    proc_dir(cfg["pack-path"])

def finish(uuid, result):
    if (result == 0):
        BUILD_SESSIONS[uuid]["status"] = "finished"
        BUILD_SESSIONS[uuid]["result"] = result
        if USER_WEB_LINKER[uuid] != -1:
            BUILD_SESSIONS[uuid]["sent"] = True
            emit('build_ready', {}, room=USER_WEB_LINKER[uuid])
    else:
        if USER_WEB_LINKER[uuid] != -1:
            BUILD_SESSIONS[uuid]["sent"] = True
            emit('msgm', {"msg": "Error occured while building"}, room=USER_WEB_LINKER[uuid])

async def run_bproc(obj, ruind):
    global BUILD_SESSIONS
    # Create the subprocess; redirect stdout to a pipe
    oviron = copy.deepcopy(os.environ)
    plts = [v for v in obj["rq_body"].items()]
    oviron["MAIN_LINE"] = obj["main_line"]
    oviron["PREF_NODES_SZ"] = str(len(plts))
    oviron["PACKS_DIR"] = cfg["pack-path"]
    for nid in plts:
        for name, val in nid[1].items():
            oviron[f"{nid[0]}.{name}"] = val["val"]
    BUILD_SESSIONS[ruind] = {
        "status": "initialising",
        "sent": False
    }
    proc = await asyncio.create_subprocess_shell(
        f"./cmpf {ruind}",
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE, env=oviron)
    
    BUILD_SESSIONS[ruind]["status"] = "building"
    await proc.wait()
    BUILD_SESSIONS[ruind]["status"] = "finishing"
    finish(ruind, proc.returncode)

async def starter(obj, ruind):
    await run_bproc(obj, ruind)

#TODO: FIX THE FUCKING ASYNC NOT WORKING PROPERLY... I HATE PYTHON ASYNCIO IO BUT FORCED TO COPE WITH IT. BRUUUUUUUUUH
def create_build_session(obj):
    ruind = str(uuid.uuid4())
    USER_WEB_LINKER[ruind] = -1
    try:
        asyncio.run(starter(obj, ruind))
        return {"verdict": 1, "key": ruind, "res": "Streamsocket establishment approved."}
    except:
        return {"verdict": 0, "res": "Error happend"}