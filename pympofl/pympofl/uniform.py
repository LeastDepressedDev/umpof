import redis

class UniformIncludeException(Exception): pass
class UniformDropException(Exception): pass

__insets = {
    "nid": None,
    "st_complete": False
}
__rapp = None

def gen_npar(id, section, mtype):
    return f"{section}::{__insets["nid"]}.{id}::{mtype}"



# Returns None on incorrect type...
# Not default str to avoid debugging hell
def include(mtype, id):
    if mtype == None or id == None or len(id) == 0 or len(mtype) == 0: raise UniformIncludeException("Type or id is None")
    resp = __rapp.get(gen_npar(id, "in", mtype))

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
    __rapp.set(gen_npar(id, "out", mtype), val)

def __setup__(cfg):
    global __insets, __rapp
    pars = cfg["pars"]
    if ("node" in pars.keys()): __insets["nid"] = pars["node"]

    __rapp = redis.Redis(host="localhost", port=pars["rport"], decode_responses=True)
    try:
        __rapp.get("__upmx")
    except:
        raise RuntimeError("Failed to connect to redis server")
    
    __insets["st_complete"] = True
    pass

def change_node(uuid):
    __insets["nid"] = uuid