#!/usr/bin/env python3

import sys
import os

body = sys.stdin.read()

print("Content-Type: text/plain", end="\r\n\r\n")

print("=== Environment ===")
for k, v in os.environ.items():
    print(f"{k}={v}")

print("\n=== Body ===")
print(body)
