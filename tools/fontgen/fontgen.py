import argparse
from ctypes import Structure, c_uint8, c_uint16, c_int8

import freetype


class BasePayload(Structure):
    _pack_ = 1

    @classmethod
    def parse(cls, buffer):
        ret = cls.from_buffer(buffer)
        ret._buffer_ = bytearray(buffer)
        return ret

    def to_bytes(self):
        buf = bytearray(self)
        return buf


class _FontInfoPayload(BasePayload):
    _fields_ = [
        ('version', c_uint8),
        ('max_height', c_uint8),
        ('num_glyphs', c_uint16),
        ('wildcard_codepoint', c_uint16),
        ('hash_table_size', c_uint8),
        ('codepoint_bytes', c_uint8)
    ]


class _HashTableEntry(BasePayload):
    _fields_ = [
        ('hash_value', c_uint8),
        ('offset_table_size', c_uint8),
        ('offset', c_uint8)
    ]


class _OffsetTableEntry(BasePayload):
    _fields_ = [
        ('codepoint', c_uint16),
        ('offset', c_uint16)
    ]


class _GlyphEntry(BasePayload):
    _fields_ = [
        ('offset_top', c_int8)
        ('offset_left', c_int8),
        ('bitmap_height', c_uint8),
        ('horizontal_advance', c_int8),
        ('unused_0', c_uint8),
        ('unused_1', c_uint8),
        ('unused_2', c_uint8)
    ]


class FontGenConfig:

    def __init__(self):
        pass


class FontGen:

    def __init__(self, config: FontGenConfig):
        pass


class FontGenCLI:

    def __init__(self):
        self.__setup_parser()
        self.__parse_args()

    def __setup_parser(self):
        self.parser = argparse.ArgumentParser(
            prog="fontgen",
            description="Tool to generate fonts for Pebble OS."
        )

        # Position arguments
        self.parser.add_argument('font')
        self.parser.add_argument('output_file')

        # Flags
        self.parser.add_argument('-H', '--max_height', type=int, required=True)

    def __parse_args(self):
        self.parser.parse_args()


# Appease the python gods
if __name__ == '__main__':
    cli = FontGenCLI()
