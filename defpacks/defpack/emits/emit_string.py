from pympofl import *

def main():
    res = uniform.pref("ival", mtype="text")
    if type(res)!=str: raise InnerException("Type corrupted")
    uniform.drop("string", "out", res)
    pass

call(main_f=main)