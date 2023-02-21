import os
import subprocess
import sys
from pathlib import Path
from zipfile import ZipFile
from glob import glob

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

if os.path.exists("deps") != True:
    os.mkdir("deps")

os.chdir("deps")

if os.path.exists("CodeWarrior") != True and os.path.exists("Kamek") != True:
    subprocess.run(SYATI_SETUP)

os.chdir("syati")

if os.path.exists("deps") != True:
    os.mkdir("deps")

os.chdir("deps")

if os.path.exists("CodeWarrior") != True and os.path.exists("Kamek") != True:
    subprocess.run(SYATI_SETUP)

os.chdir(HOME_DIR)

def no_gle():
    subprocess.run(f"python {BUILD} --nogle")

    bins = glob("*.bin")

    # Safety check

    if len(bins) == 0:
        print("Some CodeWarrior or Kamek error occured. Check above for it!")
        exit(1)

    os.chdir(SYATI)

    subprocess.run("python buildloader.py")

    with ZipFile(ZIP, "w") as w:
        os.chdir("loader")
        files = glob("*.xml")
        for file in files:
            p = Path(file)
            w.write(p.absolute(), p.name)
        files = glob("Loader*.bin")
        for file in files:
            p = Path(file)
            w.write(p.absolute(), p.name)
        os.chdir(HOME_DIR)
        for entry,_,_ in os.walk("SMG2PTD"):
            if entry != "SMG2PTD":
                entry = Path(entry)
                for file in entry.glob("*.*"):
                    idx = str(file).index("\\")
                    name = str(file)[idx+1:]
                    w.write(file, name)
        files = glob("CustomCode*.bin")
        for file in files:
            p = Path(file)
            w.write(p.absolute(), f"CustomCode\\{p.name}")

def with_gle():
    subprocess.run(f"python {BUILD} --gle")

    bins = glob("*.bin")

    # Safety check

    if len(bins) == 0:
        print("Some CodeWarrior or Kamek error occured. Check above for it!")
        exit(1)

    os.chdir(SYATI)

    subprocess.run("python buildloader.py")

    with ZipFile(ZIP.with_name("PT Debug with GLE.zip"), "w") as w:
        os.chdir("loader")
        files = glob("*.xml")
        for file in files:
            # GLE does not currently support Korean or Tiawan regions.
            if "KOR" in file or "TWN" in file:
                continue
            p = Path(file)
            w.write(p.absolute(), p.name)
        files = glob("Loader*.bin")
        for file in files:
            # GLE does not currently support Korean or Tiawan regions.
            if "K" in file or "W" in file:
                continue
            p = Path(file)
            w.write(p.absolute(), p.name)
        os.chdir(HOME_DIR)
        for entry,_,_ in os.walk("SMG2PTD"):
            if entry != "SMG2PTD":
                entry = Path(entry)
                for file in entry.glob("*.*"):
                    # GLE does not currently support Korean or Tiawan regions.
                    if "K" in file.name or "W" in file.name:
                        continue
                    idx = str(file).index("\\")
                    name = str(file)[idx+1:]
                    w.write(file, name)
        files = glob("CustomCode*.bin")
        for file in files:
            # GLE does not currently support Korean or Tiawan regions.
            if "KOR" in file or "TWN" in file:
                continue
            p = Path(file)
            w.write(p.absolute(), f"CustomCode\\{p.name}")

no_gle()

files = glob("*.bin")
for file in files:
    os.remove(file)

with_gle()

files = glob("*.bin")
for file in files:
    os.remove(file)