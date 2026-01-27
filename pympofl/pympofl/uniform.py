import redis

class UniformIncludeException(Exception): pass
class UniformDropException(Exception): pass
class UniformPrefException(Exception): pass

__insets = {
    "nid": None,
    "st_complete": False,
    "noe": {},
    "charset": None
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
    if id not in __insets["noe"].keys(): raise UniformIncludeException(f"Requested param is not provided! Request: {mtype}->{id}\n OE list: {__insets["noe"]}")
    resp = bytes(__rapp.get(__insets["noe"][id]))

    if mtype == "byte": return resp[0]
    elif mtype == "bytes": return resp
    else:
        if mtype == "text": return resp.decode(__insets["charset"])
        elif mtype == "int": return int(resp.decode("utf-8"))
        elif mtype == "float": return float(resp.decode("utf-8"))
        elif mtype == "tlist": return str(resp.decode(__insets["charset"])).split("\t")
        else: return None

def drop(mtype, id, val):
    if mtype == None or id == None or len(id) == 0 or len(mtype) == 0: raise UniformDropException("Type or id is None")
    if mtype == "text": 
        if type(val) != str: raise UniformDropException("Incorrect type")
        val = str(val).encode(__insets["charset"])
    elif mtype == "int": 
        if type(val) != int: raise UniformDropException("Incorrect type")
        val = str(val).encode("utf-8")
    elif mtype == "float": 
        if type(val) != float: raise UniformDropException("Incorrect type")
        val = str(val).encode("utf-8")
    elif mtype == "tlist":
        # future valid
        val = ("\t".join(val)).encode(__insets["charset"])
    elif mtype == "byte": 
        if type(val) == bytes: val = val[0]
        else: val = str(val).encode(__insets["charset"])[0]
    elif mtype == "bytes": 
        if type(val) == bytes: val = val
        else: val = str(val).encode(__insets["charset"])
    __rapp.set(gen_npar(__insets["nid"], id), val)


# Here default is str, cuz it's pref
def pref(id, mtype=None):
    resp = bytes(__rapp.get(gen_npref(id)))
    if mtype==None: return str(resp.decode("utf-8"))
    else:
        if len(mtype) == 0: raise UniformPrefException("Type is defined but empty")
        if mtype == "text": return str(resp.decode("utf-8"))
        elif mtype == "int": return int(resp.decode("utf-8"))
        elif mtype == "float": return float(resp.decode("utf-8"))
        else: return str(resp.decode("utf-8"))



def __setup__(cfg):
    global __insets, __rapp
    pars = cfg["pars"]
    if ("node" in pars.keys()): __insets["nid"] = pars["node"]

    for k, v in pars.items():
        if k.startswith("oe"):
            __insets["noe"][k.split(":")[1]] = v
    __rapp = redis.Redis(host="localhost", port=pars["rport"])
    try:
        __rapp.get("__upmx")
    except:
        raise RuntimeError("Failed to connect to redis server")
    
    __insets["charset"] = pars["charset"]
    __insets["st_complete"] = True
    pass

def change_node(uuid):
    __insets["nid"] = uuid