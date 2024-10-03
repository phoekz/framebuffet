#!/usr/bin/env python

import sys
import os
import hashlib


def main():
    if len(sys.argv) < 2:
        print("Usage: python build_print_binary_info.py <binary>")
        sys.exit(1)

    binary = sys.argv[1]
    if not os.path.exists(binary):
        print(f"Error: file not found: {binary}")
        sys.exit(1)

    data = None
    with open(binary, "rb") as f:
        data = f.read()

    size = len(data)
    print(f"Executable size: {size / 1024.0 / 1024.0:.2f} MB ({size} bytes)")

    hash = hashlib.new("sha256")
    hash.update(data)
    print(f"SHA256: {hash.hexdigest()}")


if __name__ == "__main__":
    main()
