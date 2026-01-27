from pympofl import *

def main():
    fname = uniform.include("text", "fname")
    if type(fname)!=str: raise InnerException("Type(s) corrupted")
    if len(fname) == 0: raise InnerException("Empty file name")
    if "/" in fname or ":" in fname or "~" in fname: raise InnerException("Restricted symbols in file name")

    with open(fname, 'rb') as f:
        if not f.readable(): raise InnerException("File is not readable")
        s = f.read()
        uniform.drop("bytes", "out", s)

call(main_f=main)