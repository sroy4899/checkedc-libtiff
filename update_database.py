#!/usr/bin/python3

import json
import os

basedir = ""
pwd_start = os.getcwd()

with open("convert_all.sh", 'r') as f: 
    for line in f.readlines():
        if "BASEDIR=" in line: 
            basedir = line.rstrip().split("BASEDIR=")[1] 

os.chdir(basedir)
pwd = os.getcwd() 
os.chdir(pwd_start)

with open('compile_commands.json', 'r') as f:
    database = json.loads(f.read())

for entry in database:
    entry['directory'] = pwd


with open('compile_commands.json', 'w') as f:
    f.write(json.dumps(database))

print('Done!')
