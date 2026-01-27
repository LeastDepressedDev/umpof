from pympofl import *

def main():
    inp = uniform.include("int", "in")
    if type(inp)!=int: raise InnerException("Type corrupted")
    uniform.drop("bytes", "out", str(inp).encode("utf-8"))
    pass

call(main_f=main)