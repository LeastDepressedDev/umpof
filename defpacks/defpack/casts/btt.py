from pympofl import *

def main():
    inp = uniform.include("bytes", "in")
    if type(inp)!=bytes: raise InnerException("Type corrupted")
    uniform.drop("text", "out", inp.decode(uniform.__insets["charset"]))
    pass

call(main_f=main)