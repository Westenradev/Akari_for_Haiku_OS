#!/usr/bin/env python3
"""Generate the lit and unlit HVIF lantern icons for Akari.

Pure stdlib. Emits Haiku Vector Icon Format (HVIF) binaries per
haiku src/libs/icon/flat_icon/FlatIconImporter.cpp and FlatIconFormat.cpp:

  magic 'ficn' (LE uint32)
  uint8 style_count, then per style: uint8 type, payload
  uint8 path_count, then per path: uint8 flags, uint8 point_count, coords
  uint8 shape_count, then per shape: uint8 type, style_index, path_count,
      path indices, uint8 shape_flags

Coordinates use the write_coord encoding from FlatIconFormat.cpp:
  integer coords in [-32, 95] -> single byte (coord + 32)
  otherwise two bytes: (coord + 128) * 102 with the high bit set in byte 0

Each icon is self-verified by re-parsing the emitted bytes with the same
read_coord decoding the Haiku importer uses.
"""

import math
import os
import struct
import sys

MAGIC = 0x6E636966  # 'ficn' little-endian

STYLE_TYPE_SOLID_COLOR = 1
SHAPE_TYPE_PATH_SOURCE = 10
PATH_FLAG_CLOSED = 1 << 1
PATH_FLAG_NO_CURVES = 1 << 3

CANVAS = 64.0

LIT = {
    "paper": (0xF5, 0xB8, 0x5E),
    "glow": (0xFF, 0xE9, 0xB0),
    "ribs": (0x9A, 0x62, 0x2E),
    "caps": (0x4A, 0x32, 0x1E),
}

UNLIT = {
    "paper": (0xA9, 0xB6, 0xC4),
    "glow": (0xC9, 0xD4, 0xDF),
    "ribs": (0x5E, 0x6B, 0x78),
    "caps": (0x4E, 0x59, 0x64),
}


def write_coord(value):
    value = max(-128.0, min(192.0, float(value)))
    if int(value * 100.0) == int(value) * 100 and -32.0 <= value <= 95.0:
        return bytes([int(value + 32.0) & 0xFF])
    encoded = int((value + 128.0) * 102.0) | 0x8000
    return bytes([(encoded >> 8) & 0xFF, encoded & 0xFF])


def read_coord(data, offset):
    value = data[offset]
    if value & 0x80:
        coord_value = ((value & 0x7F) << 8) | data[offset + 1]
        return coord_value / 102.0 - 128.0, offset + 2
    return value - 32.0, offset + 1


def polygon_points(cx, cy, rx, ry, count, rotation=0.0):
    points = []
    for i in range(count):
        angle = rotation + 2.0 * math.pi * i / count
        points.append((cx + rx * math.cos(angle), cy + ry * math.sin(angle)))
    return points


def line_points(x1, y1, x2, y2):
    return [(x1, y1), (x2, y2)]


def build_geometry():
    """Chōchin lantern: cord, tassel, caps, 8-sided barrel, 3 ribs."""
    paths = []
    cx, cy = CANVAS / 2.0, CANVAS / 2.0 + 4.0

    paths.append(line_points(cx, 2.0, cx, 12.0))
    paths.append(polygon_points(cx, 8.0, 2.5, 2.5, 4, math.pi / 4.0))
    paths.append(line_points(cx - 1.5, 58.0, cx + 1.5, 58.0))
    paths.append(line_points(cx - 1.0, 58.0, cx - 0.5, 62.0))
    paths.append(line_points(cx + 1.0, 58.0, cx + 0.5, 62.0))
    paths.append(line_points(cx, 58.0, cx, 63.0))

    top_cap = polygon_points(cx, 14.0, 12.0, 3.0, 4, math.pi / 4.0)
    bottom_cap = polygon_points(cx, 54.0, 12.0, 3.0, 4, math.pi / 4.0)
    paths.append(top_cap)
    paths.append(bottom_cap)

    barrel = polygon_points(cx, cy, 17.0, 20.0, 8, math.pi / 8.0)
    paths.append(barrel)

    for y in (cy - 10.0, cy, cy + 10.0):
        paths.append(line_points(cx - 16.0, y, cx + 16.0, y))

    return paths


def encode_hvif(palette):
    paths = build_geometry()

    out = bytearray()
    out += struct.pack("<I", MAGIC)

    styles = [palette["paper"], palette["glow"], palette["ribs"], palette["caps"]]
    out.append(len(styles))
    for (r, g, b) in styles:
        out.append(STYLE_TYPE_SOLID_COLOR)
        out += bytes([r, g, b, 255])

    out.append(len(paths))
    for points in paths:
        out.append(PATH_FLAG_CLOSED | PATH_FLAG_NO_CURVES)
        out.append(len(points))
        for (x, y) in points:
            out += write_coord(x)
            out += write_coord(y)

    shapes = [
        (0, [8]),          # barrel body: paper
        (1, [9, 10, 11]),  # ribs: glow core lines
        (2, [0, 1, 2, 3, 4]),  # cord, tassel cap, tassels
        (3, [5, 6, 7]),    # caps: caps color
    ]
    out.append(len(shapes))
    for (style_index, path_indices) in shapes:
        out.append(SHAPE_TYPE_PATH_SOURCE)
        out.append(style_index)
        out.append(len(path_indices))
        for index in path_indices:
            out.append(index)
        out.append(0)  # shape flags

    return bytes(out)


def verify_hvif(data):
    """Re-parse using the importer's read_coord logic; raise on any error."""
    if len(data) < 5:
        raise ValueError("too short")
    magic, = struct.unpack_from("<I", data, 0)
    if magic != MAGIC:
        raise ValueError("bad magic")
    offset = 4
    style_count = data[offset]; offset += 1
    for _ in range(style_count):
        style_type = data[offset]; offset += 1
        if style_type not in (1, 2, 3, 4, 5):
            raise ValueError("bad style type %d" % style_type)
        if style_type == 1:
            offset += 4
        elif style_type == 3:
            offset += 3
        elif style_type == 4:
            offset += 2
        elif style_type == 5:
            offset += 1
        else:
            raise ValueError("gradient not expected here")
    path_count = data[offset]; offset += 1
    for _ in range(path_count):
        flags = data[offset]; offset += 1
        point_count = data[offset]; offset += 1
        for _ in range(point_count):
            if flags & PATH_FLAG_NO_CURVES:
                _, offset = read_coord(data, offset)
                _, offset = read_coord(data, offset)
            else:
                for _ in range(3):
                    _, offset = read_coord(data, offset)
                    _, offset = read_coord(data, offset)
    shape_count = data[offset]; offset += 1
    for _ in range(shape_count):
        shape_type = data[offset]; offset += 1
        if shape_type != SHAPE_TYPE_PATH_SOURCE:
            tag_length, = struct.unpack_from("<H", data, offset)
            offset += 2 + tag_length
            continue
        style_index = data[offset]; offset += 1
        path_count_ = data[offset]; offset += 1
        if style_index >= style_count:
            raise ValueError("shape references missing style")
        for _ in range(path_count_):
            index = data[offset]; offset += 1
            if index >= path_count:
                raise ValueError("shape references missing path")
        shape_flags = data[offset]; offset += 1
        if shape_flags & (1 << 1):
            offset += 18  # transformable: 6 x float24
        elif shape_flags & (1 << 5):
            _, offset = read_coord(data, offset)
            _, offset = read_coord(data, offset)
        if shape_flags & (1 << 3):
            offset += 2
        if shape_flags & (1 << 4):
            transformer_count = data[offset]; offset += 1
            offset += transformer_count  # unknown transformer payloads vary
    if offset != len(data):
        raise ValueError("trailing bytes: parsed %d of %d" % (offset, len(data)))
    return True


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    out_dir = os.path.join(here, "..", "resources")
    os.makedirs(out_dir, exist_ok=True)

    for name, palette in (("icon_lit", LIT), ("icon_unlit", UNLIT)):
        data = encode_hvif(palette)
        verify_hvif(data)
        path = os.path.join(out_dir, name + ".hvif")
        with open(path, "wb") as f:
            f.write(data)
        print("%s: %d bytes, self-verify OK" % (path, len(data)))

    lit = encode_hvif(LIT)
    lines = []
    for i in range(0, len(lit), 32):
        chunk = lit[i:i + 32]
        lines.append('\t$"%s"' % chunk.hex().upper())
    rdef_icon = "\n".join(lines)
    print("\nresource vector_icon {\n%s\n};" % rdef_icon)


if __name__ == "__main__":
    sys.exit(main())
