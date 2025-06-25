# AGENTS.md - Development Headers

## Module Context

This directory contains **development headers** for the allc project. These are work-in-progress implementations that are not yet stable or complete.

## Module Overview

### Core Infrastructure
- **`types.h`** - Fundamental type definitions and platform detection
- **`allocator.h`** - Memory management abstraction with multi-size support
- **`cstr.h`** - C string utility functions (inspection, manipulation, searching)
- **`strbuf.h`** - Dynamic string buffer with growth management
- **`list.h`** - Generic doubly-linked list implementation
- **`macro.h`** - Debugging, testing, and utility macros

### System Integration
- **`path.h`** - Cross-platform file system path handling
- **`logger.h`** - Multi-level logging system
- **`builder.h`** - Meta-build system for project-specific build tools
- **`iterator.h`** - Iterator abstraction patterns
- **`regex.h`** - Regular expression support

### OS Abstraction Layer (`os/`)
- **`process.h`** - Process creation and management
- **`file.h`** - File system operations
- **`directory.h`** - Directory manipulation
- **`memory.h`** - Memory management primitives
- **`network.h`** - Network operations
- **`time.h`** - Time and date utilities
- **`environ.h`** - Environment variable access

## Key Patterns for AI Agents

### Header Structure
```c
/**
 * Module description and purpose
 * Options, configuration, authorship
 */

#ifndef ALLC_MODULE__GUARD
#define ALLC_MODULE__GUARD

// Includes
#include "dependencies.h"

// Macros and configuration
#ifdef ALLC_IMPL
#define ALLC_MODULE_IMPL
#endif

// Type definitions
typedef struct { ... } ModuleType;

// Function declarations
ModuleType allc_module_new(allc_allocator_t allocator);
void allc_module_delete(ModuleType self);

#endif // ALLC_MODULE__GUARD

// Implementation section
#ifdef ALLC_MODULE_IMPL
// Implementation code here
#endif
```

### Memory Management Pattern
All modules that allocate memory should:
1. Accept `allc_allocator_t` parameter in constructors
2. Store allocator in struct for later use
3. Use allocator for all allocations/deallocations
4. Provide clean deletion functions

### Development Status
- 🚧 **Under Active Development** - APIs may change frequently
- ⚠️ **Incomplete Implementations** - Some functions may be stubs
- 🔄 **Iterative Design** - Structure evolving based on usage patterns

### Dependencies Between Modules
```
types.h (foundation)
├── allocator.h
├── cstr.h
├── math.h (depends on: types)
├── random.h (depends on: types)
├── crypto.h (depends on: types, math)
├── strbuf.h (depends on: allocator, cstr)
├── unicode.h (depends on: allocator, strbuf)
├── encoding.h (depends on: strbuf, unicode)
├── list.h (depends on: allocator)
├── array.h (depends on: allocator)
├── hashmap.h (depends on: allocator, list)
├── set.h (depends on: hashmap)
├── queue.h (depends on: list)
├── heap.h (depends on: array)
├── tree.h (depends on: allocator)
├── datetime.h (depends on: strbuf, types)
├── json.h (depends on: strbuf, hashmap, array)
├── csv.h (depends on: strbuf, array)
├── config.h (depends on: strbuf, hashmap)
├── path.h (depends on: strbuf)
├── logger.h (depends on: allocator)
├── thread.h (depends on: os abstraction)
├── sync.h (depends on: thread)
├── threadpool.h (depends on: thread, sync, queue)
└── os/ modules (depend on various core modules)
```

### Testing Integration
Each module should have corresponding test file in `../tests/`:
- Test all public functions
- Use `ALLC_TEST_*` macros from `macro.h`
- Test error conditions and edge cases
- Verify memory management (no leaks)

### When Working on These Modules

1. **Check Dependencies** - Ensure required modules are implemented first
2. **Follow Allocator Pattern** - Use provided allocator, don't call malloc directly
3. **Implement Incrementally** - Get basic functionality working before adding features
4. **Write Tests Early** - Create test file alongside implementation
5. **Document as You Go** - Add clear comments for complex logic
6. **Consider Platform Differences** - Especially for `os/` modules

### Common Issues to Watch For

- **Memory Leaks** - Always pair allocations with deallocations
- **Buffer Overflows** - Validate sizes before string/memory operations
- **Platform Dependencies** - Keep platform-specific code in `os/` modules
- **API Consistency** - Follow established naming and parameter conventions
- **Error Handling** - Return meaningful error codes, log when appropriate

### Build Integration
All headers here are built through the Makefile dependency system:
- Add new dependencies to Makefile variable definitions
- Test targets are automatically generated
- Build dependencies are tracked automatically

### Future Migration
When modules mature:
1. Move from `dev/` to `include/`
2. Update documentation
3. Add to stable API
4. Create migration guide for users

### Planned Data Structure Modules
- **`array.h`** - Dynamic arrays with automatic resizing
- **`hashmap.h`** - Hash tables with configurable hash functions
- **`set.h`** - Hash-based set implementation
- **`queue.h`** - FIFO queues and LIFO stacks
- **`heap.h`** - Binary heap and priority queue
- **`tree.h`** - Binary search trees with optional balancing

### Planned Mathematical Modules
- **`math.h`** - Mathematical functions and operations
- **`random.h`** - Random number generation and distributions
- **`crypto.h`** - Cryptographic functions and hashing

### Planned I/O and Serialization Modules
- **`json.h`** - JSON parsing and generation
- **`csv.h`** - CSV processing
- **`config.h`** - Configuration file handling (INI, TOML)
- **`encoding.h`** - Text encoding and URL encoding
- **`unicode.h`** - Unicode string processing

### Planned Concurrency Modules
- **`thread.h`** - Cross-platform threading
- **`sync.h`** - Synchronization primitives
- **`threadpool.h`** - Thread pool management

### Planned Time and Date Modules
- **`datetime.h`** - Date/time parsing, formatting, and arithmetic
