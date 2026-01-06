import sys

CFG = {
    "flags": {},
    "pars": {}
}


for arg in sys.argv[1:]:
    if "=" in arg: 
        arg = arg.split("=")
        CFG["pars"][arg[0]] = arg[1]
    else:
        CFG["flags"][arg[0]] = 1
