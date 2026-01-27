from pympofl import *

def main():
    inp = uniform.include("string", "in")
    uniform.drop("string", "out", inp)
    pass

call(main_f=main)