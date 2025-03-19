import argparse
import logging

from ctypes import Structure, c_uint8, c_uint16, c_int8

import json

import freetype

logger = logging.getLogger(__name__)

kHASH_TABLE_SIZE = 255


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

    def size(self):
        return len(self.to_bytes())


class _FontInfoPayload(BasePayload):
    _fields_ = [
        ('version', c_uint8),
        ('max_height', c_uint8),
        ('num_glyphs', c_uint16),
        ('wildcard_codepoint', c_uint16),
        ('hash_table_size', c_uint8),
        ('codepoint_bytes', c_uint8)
    ]


class _GlyphEntry(BasePayload):
    _fields_ = [
        ('width', c_uint8),
        ('height', c_uint8),
        ('offset_top', c_int8),
        ('offset_left', c_int8),
        ('unused_0', c_uint8),
        ('unused_1', c_uint8),
        ('unused_2', c_uint8),
        ('horizontal_advance', c_int8)
    ]

    def __init__(self):
        super().__init__()

        self.bitmap = bytearray()

    def to_bytes(self):
        buf = super().to_bytes()
        buf += self.bitmap

        return buf


class _OffsetTableEntry(BasePayload):
    _fields_ = [
        ('codepoint', c_uint16),
        ('offset', c_uint16)
    ]

    def __init__(self):
        super().__init__()

        self.glyph_entries: [_GlyphEntry] = []


class _HashTableEntry(BasePayload):
    _fields_ = [
        ('hash_value', c_uint8),
        ('offset_table_size', c_uint8),
        ('offset', c_uint8)
    ]

    def __init__(self):
        super().__init__()

        self.offset_entries: [_OffsetTableEntry] = []

    def to_bytes(self):
        self.offset_table_size = _OffsetTableEntry().size()

        return super().to_bytes()


class FontGen:

    def __init__(self, codepoint_path, font_path, max_height, output_path):
        self.font_path = font_path
        self.output_path = output_path

        self.max_height = max_height

        self.font_info = _FontInfoPayload()
        self.hash_table_entries = []

        self.num_glyphs = 0

        for i in range(kHASH_TABLE_SIZE):
            self.hash_table_entries.append(_HashTableEntry())

        try:
            codepoint_file = open(codepoint_path, "r")
        except Exception as e:
            logger.fatal("Failed to open codepoint json file.")
            raise e

        try:
            self.codepoints = json.load(codepoint_file)
        except Exception as e:
            logger.fatal("Failed to parse codepoint json file.")
            raise e

    def _hash_function(self, codepoint):
        return codepoint % kHASH_TABLE_SIZE

    def generate_font(self):
        # Load font face
        try:
            self.face = freetype.Face(self.font_path)
        except Exception as e:
            logger.fatal("Failed to load font.")
            raise e

        self.face.set_pixel_sizes(width=0, height=self.max_height)

        # Generate glyphs
        for _codepoint in self.codepoints:

            utf8_bytes = _codepoint.encode('utf-8')
            utf8_ints = [int(byte) for byte in utf8_bytes]

            codepoint = utf8_ints[0]

            self.num_glyphs += 1

            codepoint_hash = self._hash_function(codepoint)

            offset_entry = _OffsetTableEntry()
            offset_entry.codepoint = codepoint

            offset_entry.glyph_entries.append(self._generate_glyph(codepoint))

            self.hash_table_entries[codepoint_hash].hash_value = codepoint_hash
            self.hash_table_entries[codepoint_hash].offset_entries.append(
                offset_entry)

        self._generate_hash_table_offsets()

        self._write_output()

    def _generate_glyph(self, codepoint):
        flags = freetype.FT_LOAD_RENDER | freetype.FT_LOAD_MONOCHROME | freetype.FT_LOAD_TARGET_MONO
        self.face.load_char(codepoint, flags)

        bitmap = self.face.glyph.bitmap

        glyph_entry = _GlyphEntry()
        glyph_entry.width = bitmap.width
        glyph_entry.height = bitmap.rows
        glyph_entry.horizontal_advance = int(self.face.glyph.advance.x / 64)
        glyph_entry.offset_left = self.face.glyph.bitmap_left
        glyph_entry.offset_top = self.max_height - self.face.glyph.bitmap_top

        for byte in bitmap.buffer:
            glyph_entry.bitmap.append(byte)

        return glyph_entry

    def _generate_hash_table_offsets(self):

        for idx, entry in enumerate(self.hash_table_entries):
            print(f"{idx}: {entry.hash_value}")

    def _write_output(self):

        try:
            file = open(self.output_path, "wb")
        except Exception as e:
            logger.fatal("Unable to create output file.")
            raise e


class FontGenCLI:

    def __init__(self):
        self.__setup_parser()
        self.__parse_args()

        self.font_gen = FontGen(
            codepoint_path=self.args.codepoint_json,
            font_path=self.args.font_file,
            max_height=self.args.max_height,
            output_path=self.args.output_file
        )

        self.font_gen.generate_font()

    def __die(self, message=None, error_code=1):
        if message is not None:
            logger.fatal(message)

        exit(error_code)

    def __setup_parser(self):
        self.parser = argparse.ArgumentParser(
            prog="fontgen",
            description="Tool to generate fonts for Pebble OS."
        )

        # Position arguments
        self.parser.add_argument('codepoint_json')
        self.parser.add_argument('output_file')

        # Flags
        self.parser.add_argument('-H', '--max_height', type=int, required=True)
        self.parser.add_argument('-f', '--font_file', required=True)

    def __parse_args(self):
        self.args = self.parser.parse_args()


# Appease the python gods
if __name__ == '__main__':
    cli = FontGenCLI()
