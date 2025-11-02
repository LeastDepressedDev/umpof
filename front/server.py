#!/bin/python3
from flask import *
import sys
import fcfg

app = Flask(__name__)

replaces = [
    ("%pack_rq%", fcfg.cfg["packinfo"])
]

resource_list = {
    "index.html": "index.html",
    "ctx.js": "ctx.js",
    "dwindow.js": "dwindow.js",
    "project.js": "project.js",
    "workspace.js": "workspace.js"
}

@app.route("/")
def main():
    return resource_list["index.html"]

@app.route("/ctx.js")
def ctx_rq():
    return resource_list["ctx.js"]

@app.route("/project.js")
def project_rq():
    return resource_list["project.js"]

@app.route("/workspace.js")
def workspace_rq():
    return resource_list["workspace.js"]

@app.route("/dwindow.js")
def dwindow_rq():
    return resource_list["dwindow.js"]


# Argv: [1] = port: int
if __name__ == "__main__":
    tmp = resource_list
    for k in tmp.keys():
        with open(resource_list[k], 'r') as f:
            s = f.read()
            for v in replaces:
                s = s.replace(v[0], v[1])
            resource_list[k] = s
    app.run(port=int(sys.argv[1]))