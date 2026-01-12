from pympofl import *

def main():
    a = uniform.include("int", "a")
    b = uniform.include("int", "b")
    op = uniform.pref("op")
    print(type(a), type(b), type(op))
    if op == "+":
        uniform.drop("int", "res", a+b)
    elif op == "-":
        uniform.drop("int", "res", a-b)
    elif op == "*":
        uniform.drop("int", "res", a*b)
    elif op == "/":
        uniform.drop("int", "res", a/b)
    elif op == "**":
        uniform.drop("int", "res", a**b)

call(main_f=main)