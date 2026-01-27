from pympofl import *

def main():
    inp = uniform.include("bytes", "in")
    if type(inp)!=bytes: raise InnerException("Type corrupted")
    uniform.drop("int", "out", int(inp.decode("utf-8")))
    pass

call(main_f=main)