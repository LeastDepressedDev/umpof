import os
import json
import sys

cfg = {
    "allow-reload": True,
    "pack-path": "/home/sirtage/all/projects/umpof/tpacks"
}

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
