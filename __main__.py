from sys import argv
import pip
import os

if argv[1] == "new":
    if argv[2] == "project":
        os.mkdir(argv[3])
        open(argv[3] + "/main.py","x")
        os.environ["PYMLKIT_PROJ"] = argv[3]
elif argv[1] == "install":
    pip.main(["install","-t","/" + os.environ["PYMLKIT_PROJ"],argv[2]])
