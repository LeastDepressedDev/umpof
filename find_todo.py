import os

def proc_dir(pth):
    nk = os.listdir(pth)
    for v in nk:
        if v in [".git", ".vscode", "_deps", "find_todo.py"]: continue
        nw = f"{pth}/{v}"
        if os.path.isdir(nw): proc_dir(nw)
        else:
            with open(nw, 'r', errors='ignore') as f:
                i = 0
                for p in f.readlines()[:-1]:
                    if "TODO:".lower() in p.lower(): 
                        print(f"{nw}    line: {i}")
                        print(p)
                    i+=1
    pass

proc_dir(".")