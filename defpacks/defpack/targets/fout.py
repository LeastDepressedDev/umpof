from pympofl import *

def main():
    fname = uniform.include("text", "fname")
    inp = uniform.include("text", "in")
    if type(fname)!=str or type(inp)!=str: raise InnerException("Type(s) corrupted")

    if len(fname) == 0: raise InnerException("Empty file name")
    if "/" in fname or ":" in fname or "~" in fname: raise InnerException("Restricted symbols in file name")

    with open(fname, "w") as f:
        if not f.writable(): raise InnerException("File is not writable")
        f.write(inp)
    pass

call(main_f=main)