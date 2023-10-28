import sys
from pathlib import Path

class Symbol:
    def __init__(self, start: int, address: int, name: str):
        self.address = start + address
        self.name = name
    def __repr__(self) -> str:
        return f".set {self.name}, 0x{self.address:X}"
    
PAL = 0x807FCE98
US = 0x807f7798

if len(sys.argv) < 1:
    exit(1)

path = Path(sys.argv[1])

if not path.exists():
    exit(1)

start = PAL if "PAL" in str(path) else US

with open(path, "r") as f:
    lines = f.readlines()[1:]

lines = [x[2:] for x in lines if "section layout" not in x]

symbols = []

for i in range(0, len(lines)):
    split = lines[i].split('\t')
    symbols.append((int(split[0], 16), split[2]))

symbols = [Symbol(start, a, n) for (a, n) in symbols]

lines = [str(x) for x in symbols]

with open(path.with_suffix(".asm"), "w") as f:
    text = "\n".join(lines)
    f.write(text)