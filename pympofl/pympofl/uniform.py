import redis

class UniformIncludeException(Exception): pass
class UniformDropException(Exception): pass
class UniformPrefException(Exception): pass

__insets = {
    "nid": None,
    "st_complete": False,
    "noe": {}
}
__rapp = None

def gen_npar(node, id):
    return f"{node}.{id}"

def gen_npref(id):
    return f"__pref::{__insets["nid"]}.{id}"



# Returns None on incorrect type...
# Not default str to avoid debugging hell
def include(mtype, id):
    if mtype == None or id == None or len(id) == 0 or len(mtype) == 0: raise UniformIncludeException("Type or id is None")
    if id not in __insets["noe"].keys(): raise UniformIncludeException("Requested param is not provided")
    resp = __rapp.get(gen_npar(__insets["noe"][id], id))

    if mtype == "text": return str(resp)
    elif mtype == "int": return int(resp)
    elif mtype == "float": return float(resp)
    elif mtype == "list": return str(resp).split("\t")
    else: return None

def drop(mtype, id, val):
    if mtype == None or id == None or len(id) == 0 or len(mtype) == 0: raise UniformDropException("Type or id is None")
    if mtype == "text": val = str(val)
    elif mtype == "int": val = str(val)
    elif mtype == "float": val = str(val)
    elif mtype == "list": val = "\t".join(val)
    __rapp.set(gen_npar(__insets["nid"], id), val)


# Here default is str, cuz it's pref
def pref(id, mtype=None):
    resp = __rapp.get(gen_npref(id))
    if mtype==None: return str(resp)
    else:
        if len(mtype) == 0: raise UniformPrefException("Type is defined but empty")
        if mtype == "text": return str(resp)
        elif mtype == "int": return int(resp)
        elif mtype == "float": return float(resp)
        else: return str(resp)



def __setup__(cfg):
    global __insets, __rapp
    pars = cfg["pars"]
    if ("node" in pars.keys()): __insets["nid"] = pars["node"]

    for k, v in pars.items():
        if k.startswith("oe"):
            __insets["noe"][k.split(":")[1]] = v
    __rapp = redis.Redis(host="localhost", port=pars["rport"], decode_responses=True)
    try:
        __rapp.get("__upmx")
    except:
        raise RuntimeError("Failed to connect to redis server")
    
    __insets["st_complete"] = True
    pass

def change_node(uuid):
    __insets["nid"] = uuid