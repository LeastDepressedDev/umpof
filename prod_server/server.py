#!/bin/python3
from flask import *
import prod_server
import sys

app = Flask("Production server")

@app.after_request
def after_request(response):
  response.headers.add('Access-Control-Allow-Origin', '*')
  response.headers.add('Access-Control-Allow-Headers', 'Content-Type,Authorization')
  response.headers.add('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE,OPTIONS')
  return response

@app.route("/")
def main():
    pass

@app.route("/reload")
def rel_req():
    if prod_server.cfg["allow-reload"]:
        prod_server.load()
        return "Reloaded"
    else: return "Not allowed"

@app.route("/packs")
def packs_list():
    return prod_server.packs

@app.route("/rtbq")
def rtbq_req():
    return prod_server.cfg["runtime-debug"]

# Requires arguments: [1] = port: int
if __name__ == "__main__":
    prod_server.load()
    app.run(port=int(sys.argv[1]))