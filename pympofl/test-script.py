import pympofl
from pympofl import uniform

def main():
    a = uniform.include("a")
    b = uniform.include("b")
    op = uniform.include("op")
    if op == "+":
        uniform.drop("a", a+b)
    elif op == "-":
        uniform.drop("a", a-b)
    elif op == "*":
        uniform.drop("a", a*b)
    elif op == "/":
        uniform.drop("a", a/b)
    elif op == "**":
        uniform.drop("a", a**b)

pympofl.call(main_f=main)