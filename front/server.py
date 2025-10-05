#!/bin/python3
from flask import *
import sys
import fcfg

app = Flask(__name__)

@app.route("/")
def main():
    pass

# Argv: [1] = port: int
if __name__ == "__main__":
    app.run(port=int(sys.argv[1]))