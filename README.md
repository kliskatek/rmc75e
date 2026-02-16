# rmc75e

EtherNet/IP explicit messaging client for Delta RMC75E motion controllers.

Provides register read/write access via the RMC's Register Map Object (class 0xC0), using service codes 0x4B/0x4C with LSB-first byte order. Equivalent to RMCLink's `readFloat`/`writeFloat` functionality.

Built on top of [EIPScanner](https://github.com/nimbuscontrols/EIPScanner) with Python bindings via [pybind11](https://github.com/pybind/pybind11).

## Installation

### From wheel

```bash
pip install rmc75e
```

### From source

#### Linux

```bash
./setup_project_linux.sh
```

#### Windows

Run from **Developer PowerShell for VS**:

```powershell
.\setup_project_windows.ps1
```

## Quick Start

```python
from rmc75e import RMC75EClient

client = RMC75EClient("192.168.17.200")
client.connect()

# Read float registers (file 57, element 30, count 2)
values = client.read_float(57, 30, 2)
print(values)  # [1.0, 2.5]

# Write float register
client.write_float(57, 33, [3.14])

# Read integer registers
values = client.read_int32(57, 32, 1)

client.disconnect()
```

## API

### `RMC75EClient(plc_address, port=44818)`

Create a new client for an RMC75E controller.

- **`connect()`** - Open an EtherNet/IP session
- **`disconnect()`** - Close the session
- **`is_connected()`** - Check connection status
- **`read_float(file, element, count)`** - Read 32-bit float registers
- **`write_float(file, element, values)`** - Write 32-bit float registers
- **`read_int32(file, element, count)`** - Read 32-bit integer registers
- **`write_int32(file, element, values)`** - Write 32-bit integer registers
- **`send_raw_request(service, data)`** - Send a raw CIP request

## Requirements

- Python >= 3.7
- CMake >= 3.14
- C++17 compiler (GCC on Linux, MSVC on Windows)

## License

MIT
