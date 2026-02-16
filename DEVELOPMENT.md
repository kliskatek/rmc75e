# Development with VSCode

Guide for developing and debugging rmc75e using Visual Studio Code.

## Requirements

### Recommended Extensions

- **C/C++** (`ms-vscode.cpptools`) - IntelliSense, debugging
- **C/C++ Extension Pack** (`ms-vscode.cpptools-extension-pack`)
- **Python** (`ms-python.python`) - IntelliSense, debugging
- **Pylance** (`ms-python.vscode-pylance`) - Python static analysis
- **CMake** (`twxs.cmake`) - CMake syntax highlighting
- **CMake Tools** (`ms-vscode.cmake-tools`)

### Build Dependencies (first time)

Before building and debugging, compile the dependencies:

```bash
# Linux
./setup_project_linux.sh

# Windows (PowerShell)
.\setup_project_windows.ps1
```

Or manually:
```bash
python scripts/build_eipscanner.py
python scripts/build_binding.py
```

---

## Build Tasks

The project includes preconfigured build tasks. Run with `Ctrl+Shift+B` (default build task) or `Ctrl+Shift+P` > "Tasks: Run Task".

| Task | Description |
|------|-------------|
| `build-example-cpp` | Builds the C++ example (default task with `Ctrl+Shift+B`) |
| `build-binding` | Builds the Python binding (.pyd/.so) |
| `build-eipscanner` | Builds EIPScanner from source |
| `build-all` | Runs eipscanner + binding in sequence |
| `clean` | Removes compiled artifacts |

---

## Debug Configurations (F5)

There are 3 debug configurations available in the Run and Debug dropdown (`Ctrl+Shift+D`):

### C++ Example (build + debug) [Windows]

Debug the C++ example without Python. Uses the Visual Studio debugger (`cppvsdbg`).

- **Pre-launch**: Automatically builds with `build-example-cpp`
- **Executable**: `build/example_cpp/example_cpp.exe`
- **DLLs**: Automatically added to PATH (`src/rmc75e/lib/`)
- **stopAtEntry**: `true` - stops at `main()` so you can set breakpoints

**Typical usage:**
1. Open `examples/example_cpp.cpp` or `src/rmc75e/RMC75EClient.cpp`
2. Set breakpoints
3. Select "C++ Example (build + debug) [Windows]" in the dropdown
4. Press F5

### C++ Example (build + debug) [Linux]

Same as the Windows version but using GDB (`cppdbg`).

- **Executable**: `build/example_cpp/example_cpp`
- **Debugger**: GDB (`/usr/bin/gdb`)

### Python Example (debug)

Debug through the Python binding.

- **Script**: `examples/example_python.py`
- **justMyCode**: `false` - allows stepping into binding code
- **PYTHONPATH**: Points to `src/` to import the local module

**Typical usage:**
1. Open `examples/example_python.py`
2. Set breakpoints in the Python script
3. Select "Python Example (debug)"
4. Press F5

---

## Development Workflow

### Modifying C++ client code

```
1. Edit src/rmc75e/RMC75EClient.cpp or .h
2. Ctrl+Shift+B (builds C++ example)
3. F5 with "C++ Example [Windows/Linux]"
4. Debug with breakpoints
```

The C++ example (`examples/example_cpp.cpp`) allows testing the client directly without Python, simplifying C++ debugging.

### Modifying Python bindings

```
1. Edit src/rmc75e/bindings.cpp
2. Ctrl+Shift+P > "Tasks: Run Task" > "build-binding"
3. F5 with "Python Example (debug)"
```

### Modifying Python example

```
1. Edit examples/example_python.py
2. F5 with "Python Example (debug)"
   (no recompilation needed)
```

---

## C++ IntelliSense

Include paths point to:

- `src/rmc75e/` - Client source code
- `build/dependencies/EIPScanner/src/` - EIPScanner headers

These directories are created automatically when building dependencies. If IntelliSense shows "include not found" errors, run `build-eipscanner` first.

---

## Build Artifacts

After building, artifacts are located at:

```
src/rmc75e/
    rmc75e*.pyd                # Python binding (Windows)
    rmc75e*.so                 # Python binding (Linux)
    lib/
        EIPScanner.dll / libEIPScanner.so  # EIPScanner library

build/
    example_cpp/
        example_cpp.exe / example_cpp  # Compiled C++ example
    dependencies/
        EIPScanner/                    # EIPScanner source + build
    binding/                           # Binding intermediate build
```

---

## Troubleshooting

### IntelliSense cannot find headers

Run the `build-eipscanner` task to clone and compile dependencies. Headers are in `build/dependencies/`.

### "DLL not found" when debugging C++ on Windows

The launch.json configuration already adds `src/rmc75e/lib/` to PATH. If it still fails, verify the DLLs exist:
```
src/rmc75e/lib/EIPScanner.dll
```

### "bad_alloc" when connecting to PLC (C++ Windows)

This happens if the example is compiled in Debug mode but the DLLs are Release (CRT mismatch). The build already uses `RelWithDebInfo` to avoid this. If you rebuild manually, make sure to use `--config RelWithDebInfo`.

### Python binding won't load

Verify the `.pyd`/`.so` exists in `src/rmc75e/`:
```bash
# Windows
dir src\rmc75e\rmc75e*.pyd

# Linux
ls src/rmc75e/rmc75e*.so
```

If missing, run the `build-binding` task.

### EIPScanner build errors (Windows)

EIPScanner has known issues with Winsock on Windows. The `build_eipscanner.py` script applies patches automatically. If it fails, verify that Visual Studio Build Tools is installed with the "Desktop development with C++" component.
