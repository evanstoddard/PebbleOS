# Pebble OS

A recreation of Pebble OS built on [Zephyr RTOS](https://zephyrproject.org/).

**This project is very much a work in progress.**

## Supported Platforms

| Platform | SoC | Status |
|----------|-----|--------|
| Asterix (p2d) | nRF52840 | Primary target |

## Building

### Prerequisites

- Python 3
- python3-venv
- Whatever is dependencies are listed on the Zephyr getting started page (toolchain will automatically be installed by build script)

### Build

```bash
./scripts/build.sh
```

The build script will automatically set up a virtual environment and install all required dependencies (West, Zephyr SDK, etc.) on first run.

## Contributing

PRs are welcome! This is an early-stage project and there's plenty of work to be done.

## License

See individual file headers for license information.

## Acknowledgments

- Original Pebble team for creating an amazing smartwatch platform
- Zephyr Project for the RTOS foundation
- [Rebble](https://rebble.io/) community for keeping Pebble alive
