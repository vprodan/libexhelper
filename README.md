# libexhelper

A lightweight C/assembly library for bridging exception handling between managed and native code using the DWARF unwinder.

## Overview

A re-implementation of [MonoMod's libexhelper](https://github.com/MonoMod/MonoMod), consolidated into a single cross-platform codebase. The original had separate implementations for each platform/architecture combination, which was difficult to maintain. This version uses unified C and assembly sources with platform-specific macros for Linux/macOS on x86_64/arm64.

`libexhelper` provides three exported functions for exception handling interoperability:

- **`eh_get_exception_ptr()`** - Returns a pointer to the thread-local current exception pointer
- **`eh_managed_to_native(target)`** - Calls native code from managed context, catching any C++ exceptions
- **`eh_native_to_managed(target)`** - Calls managed code from native context, rethrowing any pending exceptions

## Supported Platforms

| Platform | Architecture |
|----------|-------------|
| Linux    | x86_64, arm64 |
| macOS    | x86_64, arm64 |

## Building

### Using Clang (recommended for smallest binary)

```bash
./build.sh
```

#### Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `MODE` | `debug` | Build mode: `debug` or `release` |
| `TEST` | `0` | Build and run test executable: `0` or `1` |
| `OS` | auto-detect | Target OS: `Darwin` or `Linux` |
| `PLATFORM` | auto-detect | Platform name: e.g. `macos`, `linux`, `linux-gnu` |
| `ARCH` | auto-detect | Target architecture: `x86_64` or `arm64` |
| `CC` | `clang` | C compiler (for zig `zig cc`) |
| `CXX` | `clang++` | C++ compiler (for zig `zig c++`) |

#### Examples

```bash
# Release build
MODE=release ./build.sh

# Build and run tests
MODE=release TEST=1 ./build.sh

# Cross-compile for Linux arm64 on macOS
OS=Linux PLATFORM=linux ARCH=arm64 ./build.sh
```

Outputs to `build/release/bin/` and `build/debug/bin/`.

### Using Zig (recommended for cross-compilation)

```bash
# Native build
zig build -Doptimize=ReleaseFast

# Cross-compile for Linux x86_64
zig build -Doptimize=ReleaseFast -Dtarget=x86_64-linux-gnu

# Cross-compile for Linux arm64
zig build -Doptimize=ReleaseFast -Dtarget=aarch64-linux-gnu
```

Outputs to `zig-out/lib/`.

## Testing

```bash
# With build.sh
TEST=1 ./build.sh

# With zig
zig build test
```

## Contributing

Contributions are welcome! Feel free to open issues and pull requests.

## License

MIT
