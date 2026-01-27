from pympofl import *

def main():
    inp = uniform.include("text", "in")
    uniform.drop("text", "out", inp)
    pass

call(main_f=main)