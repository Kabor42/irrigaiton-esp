#!/usr/bin/env python3

import os
from time import time, sleep
import json

from flask import Flask, request, abort, render_template
from flask.json import jsonify, loads
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

zones = {
    "1": False,
    "2": False,
    "3": False,
    "4": False,
    "5": False,
    "6": False,
    "7": False,
    "8": False,
}

h_time = 0
local_time = time()
offset_gmt = 0
config = None
sched = None
buttons = None

host_name = "ESPrinkler2"

sched = """{"scedules":[{ "zone": "Zone 2", "startTime": "23:52", "duration": 15, "days": [ "Monday", "Tuesday", "Thursday", "Friday" ], "begin": "2022-01-12", "end": "2022-01-28" }]}"""
sched = loads(sched.strip('\n'))


@app.route("/")
def root():
    return render_template("index.html")


@app.route("/status")
def status():
    global h_time
    global local_time
    h_time += time() - local_time
    local_time = time()
    payload = {"time": int(h_time), "host": host_name, "offsetGMT": offset_gmt}
    for key, value in zones.items():
        payload[f"zone{key}"] = value
    print(payload)
    return payload

@app.route('/zones',methods=['GET'])
def get_zones():
  print(zones)
  return jsonify([{"id":int(key),"name":f"zone {key}","active":value} for key, value in zones.items()])


@app.route("/settime")
def set_time():
    r_time = int(request.args.get("time"))
    r_offset_gmt = int(request.args.get("offset"))
    print(f"Time:[{r_time}], Offset GMT:[{r_offset_gmt}]")

    if r_time > 0 and r_offset_gmt > 0:
        global local_time
        local_time = r_time
        global offset_gmt
        offset_gmt = r_offset_gmt
    else:
        return jsonify({"error": True, "message": "Wrong format!"})
    return jsonify(success=True)


@app.route("/clear")
def clear_zones():
    global zones
    for _, zone in zones.items():
        zone = False
    return jsonify(status=True)


@app.route("/clean")
def clean_all():
    global config
    global sched
    global buttons
    config = None
    sched = None
    buttons = None
    return jsonify({"error": False, "message": "Persistent storage has been cleaned!"})


@app.route("/toggle/<string:zone>")
def toggle(zone: str):
    global zones
    if zone not in zones.keys():
        print(f"No such zone {zone}")
        return jsonify({"error": True, "message": "No such zone"})
    zones[zone] = not zones[zone]
    print(zones)
    return jsonify(status=True)


@app.route("/restart")
def restart():
    print("Restarting ...")
    sleep(10)
    return jsonify(status=True)


@app.route("/list")
def list():
    return jsonify(
        [
            {"type": "file" if os.path.isfile(file) else "dir", "name": file}
            for file in os.listdir(".")
        ]
    )


@app.route("/config.json")
def get_config_json():
    global config
    if config is None:
        abort(404)
    return jsonify(config)


@app.route("/sched.json")
def get_sched_json():
    global sched
    if sched is None:
        abort(404)
    return jsonify(sched)


@app.route("/buttons.json")
def get_buttons_json():
    global buttons
    if buttons is None:
        abort(404)
    return jsonify(buttons)


@app.route("/edit", methods=["DELETE"])
def delete_file():
    pass


@app.route("/edit", methods=["PUT"])
def create_file():
    pass


@app.route("/edit", methods=["POST"])
def modify_file():
    pass


@app.route("/update", methods=["POST"])
def update_file():
    pass


if __name__ == "__main__":
    h_time = 0
    app.run(host="0.0.0.0", port="8123", debug=True)
