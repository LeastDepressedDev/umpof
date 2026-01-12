from pympofl import *

def main():
    res = uniform.pref("ival", mtype="float")
    if type(res)!=float: raise InnerException("Type corrupted")
    uniform.drop("float", "out", res)
    pass

call(main_f=main)