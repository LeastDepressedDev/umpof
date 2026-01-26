from pympofl import *

def main():
    res = uniform.pref("ival", mtype="int")
    if type(res)!=int: raise InnerException("Type corrupted")
    uniform.drop("int", "out", res)
    pass

call(main_f=main)