import sys

class ValidationFailed(Exception):
    pass

CFG = {
    "flags": [],
    "pars": {}
}

def init():
    global CFG

    for arg in sys.argv[1:]:
        if "=" in arg: 
            arg = arg.split("=")
            CFG["pars"][arg[0]] = arg[1]
        else:
            CFG["flags"].append(arg)

    #Validating
    essentials = ["rport", "node"]

    # Rtf flag check... If it is service
    if "rtf" in CFG["flags"]: essentials.remove("node")

    for v in CFG["pars"].keys():
        essentials.remove(v)

    if len(essentials) > 0: raise ValidationFailed(f"Couldn't start script -> essential input parameters missing {essentials}")
