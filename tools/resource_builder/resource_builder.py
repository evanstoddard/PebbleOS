import argparse
import logging

from ctypes import Structure, c_uint32

import json

import zlib

logger = logging.getLogger(__name__)


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


class ResourceManifest(BasePayload):
    _fields_ = [
        ('num_resources', c_uint32),
        ('crc32', c_uint32),
        ('timestamp', c_uint32)
    ]

    def __init__(self):
        super().__init__()

        self.num_resources = 0
        self.crc32 = 0
        self.timestamp = 0


class ResourceTableEntry(BasePayload):
    _fields_ = [
        ('resource_id', c_uint32),
        ('offset', c_uint32),
        ('length', c_uint32),
        ('crc32', c_uint32)
    ]

    def __init__(self):
        super().__init__()

        self.resource_id = 0
        self.offset = 0
        self.length = 0
        self.crc32 = 0

        self.payload = bytearray()


class ResourceBuilder:

    def __init__(self, manifest_path, output_file):
        self.manifest_path = manifest_path
        self.output_path = output_file

        self.payload_length = 0

        self.manifest_payload = ResourceManifest()
        self.resource_entries: [ResourceTableEntry] = []

    def generate_resource_bundle(self):
        # Open and parse manifest file
        try:
            manifest_file = open(self.manifest_path, "r")
            self.manifest = json.load(manifest_file)
        except Exception:
            print("Unable to parse manifest JSON file.")
            raise Exception()

        # Create output file
        try:
            self.output_file = open(self.output_path, "wb")
        except Exception:
            raise Exception()

        self.__load_resources()
        self.__build_resource_manifest()
        self.__write_output()

        manifest_file.close()

    def __load_resources(self):
        for entry in self.manifest:
            print(f'Loading {entry["filepath"]}')
            try:
                resource_file = open(entry["filepath"], "rb")
            except Exception:
                logger.fatal(f"Unable to open resource: {entry["filepath"]}")
                raise Exception()

            resource_entry = ResourceTableEntry()
            resource_entry.resource_id = len(self.resource_entries)
            resource_entry.offset = self.payload_length

            for byte in resource_file.read():
                resource_entry.payload.append(byte)

            resource_file.close()

            resource_entry.length = len(resource_entry.payload)
            resource_entry.crc32 = zlib.crc32(resource_entry.payload)

            self.resource_entries.append(resource_entry)

    def __build_resource_manifest(self):
        self.manifest_payload.num_resources = len(self.resource_entries)

        for entry in self.resource_entries:
            self.manifest_payload.crc32 = zlib.crc32(
                entry.to_bytes(), self.manifest_payload.crc32)

    def __write_output(self):
        self.output_file.write(self.manifest_payload.to_bytes())

        for entry in self.resource_entries:
            self.output_file.write(entry.to_bytes())

        for entry in self.resource_entries:
            self.output_file.write(entry.payload)

        self.output_file.close()


class ResourceBuilderCLI:

    def __init__(self):

        self.__setup_parser()
        self.__parse_args()

        resource_builder = ResourceBuilder(
            self.args.manifest, self.args.output_file)

        try:
            resource_builder.generate_resource_bundle()
        except Exception:
            self.__die()

    def __die(self, message=None, error_code=1):
        if message is not None:
            logger.fatal(message)

        exit(error_code)

    def __setup_parser(self):
        self.parser = argparse.ArgumentParser(
            prog="resource_builder",
            description="Tool to generate resource bundle."
        )

        self.parser.add_argument('manifest')
        self.parser.add_argument('output_file')

    def __parse_args(self):
        self.args = self.parser.parse_args()


def main():
    ResourceBuilderCLI()


# Appease the python gods
if __name__ == '__main__':
    main()
