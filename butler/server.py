#!/bin/python3
from flask import *
import butler
import sys

app = Flask("Butler")

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
    if butler.cfg["allow-reload"]:
        butler.load()
        return "Reloaded"
    else: return "Not allowed"

@app.route("/packs")
def packs_list():
    return butler.packs

# Requires arguments: [1] = port: int
if __name__ == "__main__":
    butler.load()
    app.run(port=int(sys.argv[1]))