RPORT = 3030
NODE_ID = "test_one"

#ins (id, "in", val, type)
#outs (id, "out", type)
NODE_PARS = [("a", "in", 10, "int"), ("b", "in", 5, "int"), ("res", "out", "int")]

# (id, val)
NODE_PREFS = [("op", "+")]

def gen_npar(parid):
    return f"{NODE_ID}.{parid}"

def gen_npref(id):
    return f"__pref::{NODE_ID}.{id}"