#!/usr/bin/env python3
import unicodedata

# G0 and G1 are _mostly_ Latin-1.
G0_CHARS = ''.join(bytes([p]).decode('latin-1')
                   for p in range(0x20, 0x7f)) + '♪'
G1_CHARS = ''.join(bytes([p]).decode('latin-1')
                   for p in range(0xa0, 0x100))

# Excludes TSP, NBTSP
G2_CHARS = ('…ŠŒ'
            '█‘’“”•™šœ℠Ÿ'
            '⅛⅜⅝⅞│┐└─┘┌')


def c_hex(s: str) -> str:
    o = '"'
    for c in s:
        o += f'\\x{c:02x}'
    return o + '"'


for p in (G0_CHARS + G1_CHARS + G2_CHARS):
    utf_escaped = c_hex(p.encode('utf-8'))
    utf_name = unicodedata.name(p).replace(' ', '_').replace('-', '_')

    print(f'#define CEA708_CHAR_{utf_name} {utf_escaped}')

