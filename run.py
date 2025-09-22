import subprocess
import os


command = ["powershell", "g++"]

flags = ["-g", "-O3", "-Wall", "-Wextra", "-pedantic", "-w"]
include = ["-Iheaders"]

source_files = []

path = "src/"
for root, d_names, f_names in os.walk(path):
    if f_names:
        for name in f_names:
            dot = name.find(".")
            if name[dot+1:] == "cpp" and name != "bindings.cpp":
                source_files.append(f"{root}/{name}")

command.extend(source_files)
command.extend(flags)
command.extend(include)
command.extend(["-o", "main.exe"])

subprocess.run(command)
