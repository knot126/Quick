#!/usr/bin/env python3
import sys

f = open(sys.argv[1], "r")
d = f.read()
f.close()

x = d.replace("\\", "\\\\").replace("\n", "\\n").replace("\t", "\\t").replace("\"", "\\\"")
print(f'"{x}"')
