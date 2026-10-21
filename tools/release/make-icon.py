#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Draws the port's icon, tools/release/dstlk.png (256x256): a crescent moon over the port's own
night-sky gradient, with a few stars. Every shape is arithmetic - no artwork from the game is used or
copied, so the releases ship none. Rebuild it with:

    python3 tools/release/make-icon.py

Each pixel is sampled SS times on a side and averaged down, which is what gives the edges their
smoothness.
"""

import math
import os
import struct
import zlib

W = H = 256
SS = 4

SKY_TOP = (8, 7, 20)
SKY_BOTTOM = (34, 16, 48)
MOON = (238, 232, 214)
MOON_EDGE = (255, 248, 226)
HALO = (70, 44, 96)
STAR = (226, 214, 240)


def mix(a, b, t):
    return tuple(int(round(a[i] + (b[i] - a[i]) * t)) for i in range(3))


def disc(x, y, cx, cy, r):
    return (x - cx) ** 2 + (y - cy) ** 2 <= r * r


def star(x, y, cx, cy, r):
    # a four-pointed star: |dx|^0.6 + |dy|^0.6 <= r^0.6
    dx, dy = abs(x - cx), abs(y - cy)
    return dx ** 0.65 + dy ** 0.65 <= r ** 0.65


def pixel(x, y):
    t = y / (H - 1)
    colour = mix(SKY_TOP, SKY_BOTTOM, t * t)
    # the moon's halo, a faint wash that fades out
    d = math.hypot(x - 108, y - 116)
    if d < 104:
        colour = mix(colour, HALO, 0.55 * (1.0 - d / 104.0) ** 2)
    # the stars, before the moon so the moon covers any that would touch it
    for sx, sy, sr, a in ((196, 58, 7, 0.95), (58, 44, 4, 0.8), (206, 176, 5, 0.85), (44, 190, 3, 0.65)):
        if star(x, y, sx, sy, sr):
            colour = mix(colour, STAR, a)
    # the crescent: a disc with a second disc taken out of its upper right
    if disc(x, y, 108, 116, 66) and not disc(x, y, 140, 98, 60):
        edge = abs(math.hypot(x - 108, y - 116) - 66)
        colour = mix(MOON, MOON_EDGE, max(0.0, 1.0 - edge / 9.0) * 0.8)
    return colour


def main():
    rows = []
    for py in range(H):
        row = bytearray()
        for px in range(W):
            r = g = b = 0
            for sy in range(SS):
                for sx in range(SS):
                    c = pixel(px + (sx + 0.5) / SS, py + (sy + 0.5) / SS)
                    r += c[0]
                    g += c[1]
                    b += c[2]
            n = SS * SS
            row += bytes((r // n, g // n, b // n))
        rows.append(bytes(row))

    def chunk(kind, data):
        return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data) & 0xFFFFFFFF)

    raw = b"".join(b"\x00" + row for row in rows)
    png = (b"\x89PNG\r\n\x1a\n"
           + chunk(b"IHDR", struct.pack(">IIBBBBB", W, H, 8, 2, 0, 0, 0))
           + chunk(b"IDAT", zlib.compress(raw, 9))
           + chunk(b"IEND", b""))
    out = os.path.join(os.path.dirname(os.path.abspath(__file__)), "dstlk.png")
    with open(out, "wb") as f:
        f.write(png)
    print("wrote %s" % out)


if __name__ == "__main__":
    main()
