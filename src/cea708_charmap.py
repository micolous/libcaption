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

SPECIALS = {
    0x1025: '…',
    0x102A: 'Š',
    0x102C: 'Œ',

    0x1030: '█',
    0x1031: '‘',
    0x1032: '’',
    0x1033: '“',
    0x1034: '”',
    0x1035: '•',
    0x1039: '™',
    0x103A: 'š',
    0x103C: 'œ',
    0x103D: '℠',
    0x103F: 'Ÿ',

    0x1076: '⅛',
    0x1077: '⅜',
    0x1078: '⅝',
    0x1079: '⅞',
    0x107A: '│',
    0x107B: '┐',
    0x107C: '└',
    0x107D: '─',
    0x107E: '┘',
    0x107F: '┌',
}

for p in range(0xa0, 0x100):
    SPECIALS[p] = bytes([p]).decode('latin-1')

SPECIALS_ORDER = [k for k, v in sorted(SPECIALS.items(), key=lambda x: x[1])]


def c_hex(s: bytes) -> str:
    o = '"'
    for c in s:
        o += f'\\x{c:02x}'
    return o + '"'


def re2c_out(uchar: str, cchar: int):
    print(f'''
    {c_hex(uchar.encode('utf-8'))} {{
        // {unicodedata.name(uchar)}
        return 0x{cchar:04x};
    }}
    ''')


for p in (G0_CHARS + G1_CHARS + G2_CHARS):
    utf_escaped = c_hex(p.encode('utf-8'))
    utf_name = unicodedata.name(p).replace(' ', '_').replace('-', '_')

    print(f'#define CEA708_CHAR_{utf_name} {utf_escaped}')


for k in SPECIALS_ORDER:
    re2c_out(SPECIALS[k], k)
re2c_out('♪', 0x7f)
