from pympofl import *

def main():
    inp = uniform.include("int", "in")
    if type(inp)!=int: raise InnerException("Input type corrupted")
    uniform.drop("text", "out", str(inp))
    pass

call(main_f=main)