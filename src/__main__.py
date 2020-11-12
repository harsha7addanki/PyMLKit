from sys import argv
import pip
import venv
import os

if argv[1] == "new":
    if argv[2] == "project":
        venv.create(argv[3])
        open(argv[3] + "/main.py","x")
        os.environ["PYMLKIT_PROJ"] = argv[3]
        pip.main(["install","-t","./" + argv[3] + "/Lib/site-packages","matplotlib","numpy","pandas","scipy","scikit-learn"])
elif argv[1] == "install":
    pip.main(["install","-t","./" + os.environ["PYMLKIT_PROJ"],argv[2]])
