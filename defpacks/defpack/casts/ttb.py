from pympofl import *

def main():
    inp = uniform.include("text", "in")
    if type(inp)!=str: raise InnerException("Type corrupted")
    uniform.drop("bytes", "out", inp)
    pass

call(main_f=main)