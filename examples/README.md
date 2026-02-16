# Examples

Usage examples for the rmc75e library, demonstrating register read/write with a Delta RMC75E motion controller via EtherNet/IP explicit messaging.

## Available Examples

### Python Example (`example_python.py`)

Complete example using the Python package. Demonstrates:

1. Reading a single float register
2. Reading multiple float registers
3. Reading integer registers
4. Writing float registers with readback verification
5. Continuous monitoring with Ctrl+C handling

**Usage:**
```bash
# Default address (192.168.17.200)
python example_python.py

# Custom address
python example_python.py 192.168.1.100
```

**Requirements:** The `rmc75e` package must be installed or `PYTHONPATH` must point to `src/`:
```bash
# Option 1: Install the wheel
pip install dist/rmc75e-*.whl

# Option 2: Use PYTHONPATH (development)
PYTHONPATH=../src python example_python.py          # Linux
set PYTHONPATH=..\src && python example_python.py   # Windows
```

### C++ Example (`example_cpp.cpp`)

Same functionality as the Python example but using the C++ `RMC75EClient` class directly. Useful for debugging the C++ layer without the Python binding.

**Build:**
```bash
python scripts/build_example_cpp.py
```

**Run:**
```bash
# Linux
./build/example_cpp/example_cpp [plc_address]

# Windows
build\example_cpp\example_cpp.exe [plc_address]
```

On Windows, ensure the DLLs are accessible:
```cmd
set PATH=src\rmc75e\lib;%PATH%
build\example_cpp\example_cpp.exe
```

## Configuration

Both examples use the same default configuration:

| Parameter | Default | Description |
|-----------|---------|-------------|
| PLC address | `192.168.17.200` | RMC75E IP address (pass as first argument) |
| Port | `44818` (0xAF12) | EtherNet/IP standard port |

Edit the `plc_address` variable in the source code or pass the address as a command-line argument.

## Register Map Reference

The examples use the following registers:

| Register | Type | Description |
|----------|------|-------------|
| F57:30 | float | Variable 286 current value |
| F57:31 | float | Variable 287 current value |
| L57:32 | int32 | Variable 288 as integer |
| F57:33 | float | Variable 289 (used for write test) |

Refer to the Delta RMC75E documentation for the full register map.

## Debugging with VSCode

See [DEVELOPMENT.md](../DEVELOPMENT.md) for instructions on debugging examples with VSCode, including F5 launch configurations for both C++ and Python.

## Troubleshooting

### Connection refused / timeout

- Verify the RMC75E is powered on and connected to the network
- Check the IP address: `ping 192.168.17.200`
- Ensure no firewall is blocking port 44818
- Verify the RMC75E has EtherNet/IP enabled in RMCTools

### "Module not found" (Python)

Make sure the package is installed or `PYTHONPATH` is set correctly. See usage instructions above.

### "DLL not found" (C++ Windows)

Add the lib directory to PATH before running:
```cmd
set PATH=src\rmc75e\lib;%PATH%
```
