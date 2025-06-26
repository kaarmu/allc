# AGENTS.md - allc Project Root

## Project Context

This is **allc**, a collection of STB-style header-only C libraries providing everyday, general-purpose functionality. The project emphasizes simplicity, portability, and ease of integration.

## Key Information for AI Agents

### Project Structure
- `dev/` - Development headers (unstable, work in progress)
- `include/` - Stable public headers (future release location, currently empty)
- `tests/` - Unit tests corresponding to each module
- `src/` - Implementation files when header-only isn't sufficient
- Root `.c` files - Example/demo programs

### Coding Standards
- **Language**: C23 standard
- **Naming**: All public functions prefixed with `allc_<module>_`
- **Memory**: Use allocator abstraction (`allc_allocator_t`)
- **Error Handling**: Return error codes, use logger for critical errors
- **Documentation**: Inline comments in header files

### STB-Style Pattern
- Headers contain both declarations and implementations
- Implementation activated with `#define ALLC_IMPL` before inclusion
- Each module is self-contained in a single header file
- Users can selectively enable implementations with `#define ALLC_IMPL_<MODULE>`

### Build System
- Makefile-based build with dependency tracking
- Test targets: `make test_<module>`
- Object files cached in `.cache/` directory
- Compiler flags: `-std=c23 -g -Wall -Wextra -Wunused-result`

### Core Modules Status
- ✅ `types.h` - Type system foundation
- ✅ `allocator.h` - Memory management abstraction
- ✅ `cstr.h` - C string utilities (includes case conversion functions)
- 🔄 `strbuf.h` - Dynamic string buffer (partial implementation)
- ✅ `list.h` - Doubly linked list
- ✅ `array.h` - Dynamic arrays (growable, type-agnostic, slice operations)
- 🔄 `path.h` - File system path handling
- ✅ `logger.h` - Logging system
- ✅ `macro.h` - Utility macros
- 🔄 `builder.h` - Meta-build system

### Planned Data Structure Modules
- 📋 `hashmap.h` - Hash tables and dictionaries
- 📋 `set.h` - Set data structure
- 📋 `queue.h` - Queues and stacks
- 📋 `heap.h` - Priority queues
- 📋 `tree.h` - Tree data structures

### Planned Utility Modules
- 📋 `math.h` - Mathematical operations
- 📋 `random.h` - Random number generation
- 📋 `crypto.h` - Cryptographic functions
- 📋 `datetime.h` - Date and time operations
- 📋 `unicode.h` - Unicode support

### Planned I/O and Serialization Modules
- 📋 `json.h` - JSON processing
- 📋 `csv.h` - CSV handling
- 📋 `config.h` - Configuration files
- 📋 `encoding.h` - Text encoding

### Planned Concurrency Modules
- 📋 `thread.h` - Threading primitives
- 📋 `sync.h` - Synchronization mechanisms
- 📋 `threadpool.h` - Thread pool management

### System Abstraction Status
- 🔄 `os/process.h` - Process management
- 🔄 `os/file.h` - File operations
- 🔄 `os/directory.h` - Directory operations
- 🔄 `os/memory.h` - Memory management
- 🔄 `os/network.h` - Network operations
- 🔄 `os/time.h` - Time utilities
- 🔄 `os/environ.h` - Environment variables

### When Working on This Project
1. **Read DESIGN.md** first for architectural context
2. **Check existing tests** in `tests/` directory for usage examples
3. **Follow STB pattern** - declarations in header, implementation in `#ifdef ALLC_IMPL` block
4. **Use allocator abstraction** - don't call malloc/free directly
5. **Write tests** for new functionality
6. **Update relevant AGENTS.md** files when making architectural changes

### Common Patterns
```c
// Module initialization
ModuleType allc_module_new(allc_allocator_t allocator) {
    ModuleType self = allocator.alloc(sizeof(*self));
    // ... initialize
    return self;
}

// Module cleanup
void allc_module_delete(ModuleType self) {
    self->allocator.free(self);
}

// Error handling
if (error_condition) {
    allc_logger_log_erro(&logger, "Error: %s", error_message);
    return ERROR_CODE;
}
```

### Author
Kaj Munhoz Arfvidsson, 2023

### Related Files
- `DESIGN.md` - Complete project design document
- `Makefile` - Build system configuration
- Individual `AGENTS.md` files in subdirectories

### Standard Library Scope
This project aims to provide a comprehensive alternative to system standard libraries with:
- **Complete Data Structures**: Hash tables, sets, trees, queues, stacks, heaps
- **Mathematical Operations**: Arithmetic, trigonometry, statistics, random numbers
- **String Processing**: Unicode support, advanced pattern matching, formatting
- **I/O and Serialization**: JSON, CSV, configuration files, text encoding
- **System Integration**: Process management, file systems, networking
- **Concurrency Support**: Threading, synchronization, thread pools
- **Cryptographic Functions**: Hashing, encoding, secure random generation
- **Time and Date**: Comprehensive date/time handling with timezone support

### Project Goals
- **STB-Style Headers**: Single-file, header-only distribution
- **Allocator Abstraction**: Custom memory management throughout
- **Cross-Platform**: Consistent API across operating systems
- **Zero Dependencies**: No external libraries beyond system calls
- **Performance**: Optimized implementations without sacrificing readability
