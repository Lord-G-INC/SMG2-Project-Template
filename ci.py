import os
import subprocess
import sys
from pathlib import Path
from glob import glob
from zipfile import ZipFile, ZIP_LZMA
import shutil

def err(message: str):
    print(f"Error: {message}")
    sys.exit(1)

def dep(path, name):
    path = Path(path)
    if not os.path.exists(path):
        err(f"{name} not found!")
    return path

BUILD = dep("build.py", "Build script")
SYATI = dep("deps\\Syati", "Syati").absolute()
SYATI_SETUP = dep("deps\\syatisetup.exe", "SyatiSetup").absolute()
HOME_DIR = dep(__file__, "ci.py").parent.absolute()
ZIP = Path("PT Debug.zip").absolute()

os.chdir("deps")

if os.path.exists("CodeWarrior") != True and os.path.exists("Kamek") != True:
    subprocess.run(SYATI_SETUP)

os.chdir(HOME_DIR)

subprocess.run(f"python {BUILD}", shell=True)

os.chdir(SYATI)

if os.path.exists("deps") != True:
    os.mkdir("deps")

os.chdir("deps")

if os.path.exists("CodeWarrior") != True and os.path.exists("Kamek") != True:
    subprocess.run(SYATI_SETUP)

os.chdir(SYATI)

subprocess.run("python buildloader.py")

with ZipFile(ZIP, "w", ZIP_LZMA) as w:
    os.chdir("loader")
    files = glob("*.xml")
    for file in files:
        p = Path(file)
        w.write(p.absolute(), p.name)
    os.chdir(HOME_DIR)
    files = glob("*.bin")
    for file in files:
        p = Path(file)
        w.write(p.absolute(), p.name)
    for entry,_,_ in os.walk("SMG2PTD"):
        if entry != "SMG2PTD":
            entry = Path(entry)
            for file in entry.glob("*.*"):
                idx = str(file).index("\\")
                name = str(file)[idx+1:]
                w.write(file, name)