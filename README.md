# allc

STB-style header libraries for everyday, general-purpose features with comfortable and easily understandable APIs.

## Overview

**allc** is a collection of single-file header libraries written in C23 that provide commonly needed functionality for C projects. Following the STB pattern popularized by Sean Barrett, each module is self-contained in a single header file with the implementation included via preprocessor definitions.

## Features

### Core Infrastructure
- 🎯 **Memory Management** - Flexible allocator abstraction
- 📝 **String Handling** - C string utilities and dynamic string buffers
- 📋 **Data Structures** - Doubly linked lists with type-agnostic storage
- 🔧 **Utility Macros** - Testing, debugging, and common patterns

### System Integration
- 📁 **Path Handling** - Cross-platform file system path manipulation
- 📊 **Logging** - Multi-level logging system with configurable output
- 🔨 **Build Tools** - Meta-build system for project-specific build scripts
- 💻 **OS Abstraction** - Cross-platform system operations

## Quick Start

### Basic Usage

```c
#define ALLC_IMPL
#include "dev/strbuf.h"
#include "dev/list.h"

int main() {
    // Create string buffer
    allc_allocator_t allocator = allc_allocator();
    StrBuf str = allc_strbuf_new(allocator, 64);
    allc_strbuf_set_cstr(&str, "Hello, World!");
    
    // Create list
    List list = allc_list_new(allocator);
    allc_list_append(&list, sizeof(StrBuf), &str);
    
    // Cleanup
    allc_list_delete(list);
    allc_strbuf_delete(str);
    return 0;
}
```

### Selective Implementation

```c
// Only include specific implementations
#define ALLC_IMPL_STRBUF
#define ALLC_IMPL_LIST
#include "dev/strbuf.h"
#include "dev/list.h"
```

## Available Modules

### ✅ Ready for Use
- **`types.h`** - Fundamental type definitions and platform detection
- **`cstr.h`** - C string inspection, manipulation, and searching
- **`macro.h`** - Testing macros and utility functions
- **`logger.h`** - Multi-level logging with printf-style formatting

### 🔄 In Development
- **`allocator.h`** - Custom memory management (partial implementation)
- **`strbuf.h`** - Dynamic string buffer (some functions incomplete)
- **`list.h`** - Doubly linked list (core functionality working)
- **`path.h`** - File system path handling (needs cross-platform testing)
- **`builder.h`** - Build system utilities (early development)

### 📋 Planned
- **`os/`** - Operating system abstraction layer
- **`regex.h`** - Regular expression support
- **`iterator.h`** - Iterator patterns and utilities

## Building

### Prerequisites
- C23-compatible compiler (GCC, Clang, MSVC)
- Make (for build system)
- POSIX-compliant system (Linux, macOS, BSD)

### Build Commands

```bash
# Build specific module
make build_strbuf

# Build all dependencies for a module
make build_deps_for_strbuf

# Run tests
make test_cstr
make test_list

# Clean build artifacts
make clean
```

### Directory Structure

```
allc/
├── dev/                    # Development headers (unstable)
│   ├── *.h                 # Individual module headers
│   └── os/                 # OS abstraction headers
├── include/                # Stable public headers (future)
├── tests/                  # Unit tests for each module
│   └── *.c                 # Test files
├── examples/               # Usage examples
│   └── *.c                 # Example programs
├── Makefile               # Build configuration
└── DESIGN.md              # Architecture documentation
```

## Design Philosophy

### STB-Style Pattern
- **Single File Distribution** - Each module is self-contained
- **Header-Only Libraries** - Implementation included via `#define ALLC_IMPL`
- **Zero Dependencies** - Minimal reliance on external libraries
- **Easy Integration** - Simple copy-and-include usage

### Memory Management
- **Allocator Abstraction** - Custom memory management support
- **RAII-Style** - Clear ownership with create/delete pairs
- **No Hidden Allocations** - All allocations explicit and configurable

### API Design
- **Consistent Naming** - All functions prefixed with `allc_<module>_`
- **Clear Semantics** - Obvious behavior from function names
- **Error Handling** - Consistent error reporting and logging
- **Platform Agnostic** - Same API across different platforms

## Examples

### String Manipulation
```c
#define ALLC_IMPL
#include "dev/cstr.h"
#include "dev/strbuf.h"

// C string utilities
bool is_number = allc_cstr_is_digit("12345");
size_t pos = allc_cstr_find_blank("hello world");

// Dynamic string buffer
StrBuf buf = allc_strbuf_new(allocator, 0);
allc_strbuf_set_fmt(&buf, "Value: %d", 42);
allc_strbuf_append_cstr(&buf, " (formatted)");
```

### List Operations
```c
#define ALLC_IMPL
#include "dev/list.h"

List numbers = allc_list_new(allocator);

// Add elements
int values[] = {1, 2, 3, 4, 5};
for (int i = 0; i < 5; i++) {
    allc_list_append(&numbers, sizeof(int), &values[i]);
}

// Access elements
int *first = (int*)allc_list_at(&numbers, 0);
int *last = (int*)allc_list_at(&numbers, -1);
```

### Path Handling
```c
#define ALLC_IMPL
#include "dev/path.h"

Path cwd = allc_path_new_from_cwd(allocator);
Path absolute = allc_path_to_absolute_path(
    allc_path_new_from_cstr(allocator, "relative/path")
);

bool is_abs = allc_path_is_absolute(&absolute);
```

## Testing

Run tests for individual modules:

```bash
make test_cstr      # Test C string utilities
make test_list      # Test linked list
make test_strbuf    # Test string buffer
make test_path      # Test path handling
```

Tests use custom assertion macros for clear output:

```c
ALLC_TEST_ANY("expected", "%s", actual_string);
ALLC_TEST_BOOL(true, condition);
```

## Contributing

### Development Status
🚧 **Active Development** - APIs may change frequently during development

### Guidelines
1. **Follow STB Pattern** - Keep modules self-contained
2. **Use Allocator Abstraction** - Don't call malloc/free directly
3. **Write Tests** - Add tests for new functionality
4. **Document Code** - Include clear comments and examples
5. **Check Existing Patterns** - Follow established conventions

### Module Development Process
1. Create header in `dev/` directory
2. Implement core functionality
3. Add comprehensive tests in `tests/`
4. Update build system dependencies
5. Add documentation and examples
6. Review and refine API

## Documentation

- **`DESIGN.md`** - Complete project architecture and design decisions
- **`AGENTS.md`** - Context files for AI development assistance
- **`dev/AGENTS.md`** - Development module documentation
- **`tests/AGENTS.md`** - Testing infrastructure documentation

## License

[License information would go here]

## Author

Authored by Kaj Munhoz Arfvidsson, 2023.
Edited with LLMs.

## Acknowledgments

Inspired by the STB libraries by Sean Barrett and the philosophy of simple, self-contained C libraries.
