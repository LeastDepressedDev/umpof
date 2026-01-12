from pympofl import *

ops = {
    "+": lambda a, b: a+b,
    "-": lambda a, b: a-b,
    "*": lambda a, b: a*b,
    "/": lambda a, b: a//b,
    "**": lambda a, b: a**b,
    "^": lambda a, b: a**b,
    "%": lambda a, b: a%b
}

def main():
    a = uniform.include("int", "a")
    b = uniform.include("int", "b")
    op = uniform.pref("opcode", "text")
    if type(a)!=int or type(b)!=int or type(op)!=str: raise InnerException("Type(s) corrupted")
    if op not in ops.keys(): raise InnerException("Incorrect or unsupported operation")
    uniform.drop("int", "r", ops[op]())
    pass

call(main_f=main)