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

    def __init__(self):
        super().__init__()

        self.version = 0
        self.max_height = 0
        self.num_glyphs = 0
        self.wildcard_codepoint = 0
        self.hash_table_size = 0
        self.codepoint_bytes = 0


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

        self.width = 0
        self.height = 0
        self.offset_top = 0
        self.offset_left = 0
        self.unused_0 = 0
        self.unused_1 = 0
        self.unused_2 = 0
        self.horizontal_advance = 0

        self.bitmap = bytearray()

    def to_bytes(self):
        buf = super().to_bytes()
        buf += self.bitmap

        # Align to 32-bit words
        if len(buf) % 4:
            buf += bytearray(4 - (len(buf) % 4))

        return buf


class _OffsetTableEntry(BasePayload):
    _fields_ = [
        ('codepoint', c_uint16),
        ('offset', c_uint16)
    ]

    def __init__(self):
        super().__init__()
        self.codepoint = 0
        self.offset = 0


class _HashTableEntry(BasePayload):
    _fields_ = [
        ('hash_value', c_uint8),
        ('offset_table_size', c_uint8),
        ('offset', c_uint16)
    ]

    def __init__(self):
        super().__init__()
        self.hash_value = 0
        self.offset_table_size = 0
        self.offset = 0

    def to_bytes(self):
        return super().to_bytes()


class FontGen:

    def __init__(self, codepoint_path, font_path, max_height, output_path):
        self.font_path = font_path
        self.output_path = output_path

        self.max_height = max_height

        self.font_info = _FontInfoPayload()
        self.hash_table_entries = []
        self.offset_tables: [[_OffsetTableEntry]] = []
        self.glyph_entries: [_GlyphEntry] = []

        self.num_glyphs = 0

        self.offset_table_count = 0
        self.total_glyph_size_words = 0

        for i in range(kHASH_TABLE_SIZE):
            self.hash_table_entries.append(_HashTableEntry())
            self.offset_tables.append([])

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

            codepoint = int(_codepoint, 16)
            print(f"Codepoint: {codepoint}")

            self.num_glyphs += 1

            glyph = self._generate_glyph(codepoint)
            self.glyph_entries.append(glyph)

            codepoint_hash = self._hash_function(codepoint)

            offset_entry = _OffsetTableEntry()
            offset_entry.codepoint = codepoint
            offset_entry.offset = self.total_glyph_size_words

            self.total_glyph_size_words += int(glyph.size() / 4)

            self.hash_table_entries[codepoint_hash].hash_value = codepoint_hash
            self.offset_tables[codepoint_hash].append(offset_entry)

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
            entry.offset_table_size = len(self.offset_tables[idx])

            if len(self.offset_tables[idx]):
                entry.offset = self.offset_table_count
            else:
                entry.offset = 0

            self.offset_table_count += len(self.offset_tables[idx])

    def _write_output(self):

        try:
            file = open(self.output_path, "wb")
        except Exception as e:
            logger.fatal("Unable to create output file.")
            raise e

        self.font_info.max_height = self.max_height
        self.font_info.num_glyphs = self.num_glyphs
        self.font_info.hash_table_size = kHASH_TABLE_SIZE

        file.write(self.font_info.to_bytes())

        for hash_entry in self.hash_table_entries:
            file.write(hash_entry.to_bytes())

        for offset_table in self.offset_tables:
            for entry in offset_table:
                file.write(entry.to_bytes())

        for glyph in self.glyph_entries:
            file.write(glyph.to_bytes())


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
