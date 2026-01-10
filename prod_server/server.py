#!/bin/python3
from flask import *
from flask_socketio import SocketIO, emit
import prod_server
import sys
import json
import atexit
import os

app = Flask("Production server")
app.config['SECRET_KEY'] = 'cocal?cocal?cocal?cocal?cocal?cocal?cocal?cocal?cocal?cocal?cocal?cocal?cocal?'
socketio = SocketIO(app, cors_allowed_origins="*")

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

@app.route("/build", methods=['GET', 'POST'])
def build_req():
    if request.is_json:
        return prod_server.create_build_session(request.get_json())
    else: return {"verdict": 0, "res": "Wrong"}

@app.route("/get/<runid>")
def get_req(runid):
    if runid in prod_server.BUILD_SESSIONS.keys() and prod_server.BUILD_SESSIONS[runid]["status"] == "finished":
        try:
            c = f"{runid}.ppl"
            kop = send_file(c)
            os.remove(c)
            del prod_server.BUILD_SESSIONS[runid]
            del prod_server.USER_WEB_LINKER[runid]
            return kop
        except Exception as e:
            print(e)
            return send_file("WeAreSorry.txt")
    else:
        return send_file("WeAreSorry.txt")


@socketio.on('connect')
def handle_connect():
    emit('linkrq', {})

@socketio.on('linkaw')
def handle_linkaw(asw):
    user_id = request.sid
    #print(asw, user_id, prod_server.USER_WEB_LINKER.keys(), sep='\n')
    if asw["key"] in prod_server.USER_WEB_LINKER.keys():
        prod_server.USER_WEB_LINKER[asw["key"]] = user_id
        emit('linkres', {'result': 'OCK'})

        if prod_server.BUILD_SESSIONS[asw["key"]]["status"] == "finished":
            if prod_server.BUILD_SESSIONS[asw["key"]]["result"] == 0:
                emit('build_ready', {"subid": asw["key"]})

                #Disconnect if it is all
                if prod_server.BUILD_SESSIONS[asw["key"]]["rtexec"]:
                    prod_server.call_for_run(asw["key"])
                else:
                    emit("byebye", {})
            else:
                emit('msgm', {"msgm": "Error occured while building"})
            prod_server.BUILD_SESSIONS[asw["key"]]["sent"] = True
    else:
        emit('linkres', {'result': 'DEN'})




def cleanup():
    for id in prod_server.BUILD_SESSIONS.keys():
        c = f"{id}.ppl"
        if os.path.exists(c): os.remove(c)
    pass


atexit.register(cleanup)

# Requires arguments: [1] = port: int
if __name__ == "__main__":
    prod_server.load()
    socketio.run(app, port=int(sys.argv[1]))