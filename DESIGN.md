# allc - Design Document

## Project Overview

**allc** is a collection of STB-style header-only C libraries that provide everyday, general-purpose functionality with comfortable and easily understandable APIs. The project follows the single-file header library pattern popularized by Sean Barrett's STB libraries, where the implementation is embedded within header files and activated through preprocessor definitions.

## Project Goals

### Primary Goals
1. **Simplicity**: Provide clean, intuitive APIs that are easy to understand and use
2. **Portability**: Write standard C23 code that works across different platforms
3. **Zero Dependencies**: Minimize external dependencies beyond standard C libraries
4. **Header-Only Distribution**: Enable easy integration through simple file inclusion
5. **Memory Management**: Provide flexible allocator abstraction for custom memory management

### Secondary Goals
1. **Performance**: Optimize for reasonable performance without sacrificing readability
2. **Testing**: Maintain comprehensive test coverage for all modules
3. **Documentation**: Provide clear documentation and examples
4. **Modularity**: Allow users to include only the functionality they need

## Design Principles

### 1. STB-Style Header Libraries
- **Implementation Pattern**: Use `#ifdef ALLC_IMPL` guards to include implementation code
- **Single File Distribution**: Each module is self-contained in a single header file
- **Optional Implementation**: Users control when implementation is compiled

### 2. Consistent API Design
- **Naming Convention**: All public functions prefixed with `allc_<module>_`
- **Error Handling**: Consistent error reporting through return values and logging
- **Resource Management**: Clear ownership semantics with explicit create/delete pairs

### 3. Memory Management Abstraction
- **Allocator Interface**: Custom allocator support through `allc_allocator_t`
- **Default Allocators**: Provide sensible defaults (system malloc/free)
- **Flexibility**: Allow users to plug in custom memory management strategies

### 4. Modular Architecture
- **Core Modules**: Essential data structures and utilities
- **OS Abstraction**: Platform-specific functionality cleanly abstracted
- **Optional Components**: Features can be excluded at compile time

## Architecture

### Core Modules

#### 1. **types.h** - Type System Foundation
- Standardized integer types (`allc_u8_t`, `allc_i32_t`, etc.)
- Platform detection and sizing (`ALLC_SYSTEM_SIZE`)
- Common type aliases (`allc_string_t`, `allc_bool_t`)
- Type introspection support

#### 2. **allocator.h** - Memory Management
- Abstract allocator interface supporting different pointer sizes (8/16/32/64-bit)
- System allocator implementation
- Buffer allocators for different address space sizes
- Fast allocation macros for performance-critical code

#### 3. **cstr.h** - C String Utilities
- String inspection functions (`allc_cstr_is_digit`, `allc_cstr_is_alpha`)
- String manipulation (`allc_cstr_strip_blank`, `allc_cstr_replace_char`)
- String searching (`allc_cstr_find_char`, `allc_cstr_find_cstr`)
- Case conversion functions (`allc_cstr_upper_all`, `allc_cstr_capitalize_all`)
- Safe string operations
- Uses standardized types (`allc_string_t`, `allc_cstr_t`)

#### 4. **strbuf.h** - Dynamic String Buffer
- Growable string buffer with automatic memory management
- Format string support (`allc_strbuf_set_fmt`)
- String splitting and manipulation
- Built on allocator abstraction

#### 5. **list.h** - Doubly Linked List
- Generic doubly-linked list implementation
- Type-agnostic storage (stores `void*` elements)
- Iterator support for traversal
- Built-in memory management through allocators

#### 6. **array.h** - Dynamic Arrays
- Growable array implementation with automatic resizing
- Type-agnostic storage with stride-based element access
- Slice operations for array views
- Built on allocator abstraction

#### 7. **hashmap.h** - Hash Tables
- Generic hash map implementation with configurable hash functions
- String-to-value and generic key-to-value mappings
- Collision resolution through chaining or open addressing
- Automatic resizing with load factor management

#### 8. **set.h** - Set Data Structure
- Hash-based set implementation for fast membership testing
- Support for custom equality and hash functions
- Set operations (union, intersection, difference)
- Iterator support for traversal

#### 9. **queue.h** - Queue and Stack
- FIFO queue implementation with efficient enqueue/dequeue
- LIFO stack implementation
- Ring buffer variant for fixed-size queues
- Built on existing list infrastructure

#### 10. **heap.h** - Priority Queue
- Binary heap implementation with configurable comparison
- Min-heap and max-heap variants
- Priority queue operations (insert, extract-min/max)
- Heap sort implementation

#### 11. **tree.h** - Tree Data Structures
- Binary search tree with optional self-balancing (AVL/Red-Black)
- Generic tree traversal (in-order, pre-order, post-order)
- Tree-based map and set implementations
- Iterator support for ordered traversal

### Mathematical and Utility Modules

#### 12. **math.h** - Mathematical Operations
- Basic arithmetic functions (abs, min, max, clamp)
- Power and root functions (pow, sqrt, cbrt)
- Trigonometric functions (sin, cos, tan, asin, acos, atan)
- Logarithmic functions (log, log10, log2, exp)
- Statistical functions (mean, median, standard deviation)
- Bit manipulation utilities (popcount, clz, ctz)

#### 13. **random.h** - Random Number Generation
- Pseudo-random number generators (LCG, Mersenne Twister, PCG)
- Secure random number generation for cryptographic use
- Distribution functions (uniform, normal, exponential)
- Seeding and state management

#### 14. **crypto.h** - Cryptographic Functions
- Hash functions (MD5, SHA-1, SHA-256, SHA-512)
- Message authentication codes (HMAC)
- Basic checksums (CRC32, Adler-32)
- Secure random number generation
- Base64 and hexadecimal encoding/decoding

### Input/Output and Serialization Modules

#### 15. **json.h** - JSON Processing
- JSON parsing with error reporting
- JSON generation from C data structures
- Streaming JSON parser for large files
- JSON validation and pretty-printing

#### 16. **csv.h** - CSV Processing
- CSV parsing with configurable delimiters and escaping
- CSV generation from structured data
- Header handling and field mapping
- Quote and escape character support

#### 17. **config.h** - Configuration File Handling
- INI file parsing and generation
- TOML configuration support
- Environment variable expansion
- Configuration validation and type conversion

#### 18. **encoding.h** - Text Encoding and Decoding
- UTF-8 validation and manipulation
- URL encoding and decoding
- HTML entity encoding/decoding
- Percent encoding for URLs

### Concurrency and Threading Modules

#### 19. **thread.h** - Threading Primitives
- Cross-platform thread creation and management
- Thread-local storage abstraction
- Thread joining and detaching
- Thread attribute configuration

#### 20. **sync.h** - Synchronization Primitives
- Mutexes (recursive and non-recursive)
- Read-write locks
- Condition variables
- Semaphores and barriers
- Atomic operations abstraction

#### 21. **threadpool.h** - Thread Pool Management
- Work queue with configurable thread count
- Task submission and result retrieval
- Dynamic thread pool sizing
- Graceful shutdown and cleanup

### Enhanced System Integration

#### 22. **datetime.h** - Date and Time Operations
- Date/time parsing from strings (ISO 8601, custom formats)
- Date/time formatting and string conversion
- Time zone handling and conversion
- Duration calculations and arithmetic
- High-resolution timing and profiling

#### 23. **unicode.h** - Unicode Support
- UTF-8, UTF-16, and UTF-32 conversion
- Unicode normalization (NFC, NFD, NFKC, NFKD)
- Unicode character classification
- Locale-aware string comparison and sorting

#### 24. **path.h** - File System Path Handling
- Cross-platform path manipulation
- Path resolution and normalization
- Working directory and home directory support
- Built on `StrBuf` for dynamic sizing

#### 25. **logger.h** - Logging System
- Multiple log levels (info, warn, error, panic)
- Configurable output streams
- Printf-style formatting
- Panic function for fatal errors

#### 26. **macro.h** - Utility Macros
- Debugging and testing macros
- Common patterns and convenience functions
- Test assertion helpers

### System Abstraction Layer

#### 27. **os/** - Operating System Interface
- **process.h**: Process creation and management
- **file.h**: File system operations
- **directory.h**: Directory manipulation
- **memory.h**: Memory management primitives
- **network.h**: Network operations
- **time.h**: Time and date utilities
- **environ.h**: Environment variable access

### Build System

#### 28. **builder.h** - Meta-Build System
- Using the same language (C) for both development and building
- Potential code reuse between the project and its build system
- Simple, transparent build process without external dependencies
- Idea originates from tsoding's nob.h:

    The idea is that you should not need anything but a C compiler to build a C project. No make, no cmake, no shell, no cmd, no PowerShell etc. Only C compiler. So with the C compiler you bootstrap your build system and then you use the build system to build everything else.

## File Organization

```
allc/
├── dev/                    # Development headers (work in progress)
│   ├── *.h                 # Individual module headers
│   └── os/                 # OS abstraction headers
├── include/                # Stable public headers (future release location)
├── tests/                  # Unit tests for each module
│   └── *.c                 # Test files corresponding to modules
├── src/                    # Implementation files (when needed)
├── Makefile               # Build configuration
└── *.c                    # Example/demo programs
```

## Implementation Strategy

### Development Phases

#### Phase 1: Core Infrastructure ✅
- [ ] Type system foundation (`types.h`)
- [x] Memory allocator abstraction (`allocator.h`)
- [x] Basic string operations (`cstr.h`, `strbuf.h`)
- [x] Fundamental data structures (`list.h`, `array.h`)
- [ ] Essential utilities (`macro.h`, `logger.h`)

#### Phase 2: Standard Data Structures 🔄
- [ ] Hash tables and maps (`hashmap.h`)
- [ ] Sets and collections (`set.h`)
- [ ] Queues and stacks (`queue.h`)
- [ ] Priority queues and heaps (`heap.h`)
- [ ] Tree data structures (`tree.h`)

#### Phase 3: Mathematical and Utility Operations 📋
- [ ] Mathematical functions (`math.h`)
- [ ] Random number generation (`random.h`)
- [ ] Date and time operations (`datetime.h`)
- [ ] Unicode and text processing (`unicode.h`)
- [ ] Basic cryptographic functions (`crypto.h`)

#### Phase 4: System Integration 🔄
- [ ] File system operations (`os/file.h`, `os/directory.h`)
- [ ] Process management (`os/process.h`)
- [ ] Network operations (`os/network.h`)
- [ ] Environment handling (`os/environ.h`)
- [ ] Build system utilities (`builder.h`)

#### Phase 5: Input/Output and Serialization 📋
- [ ] JSON processing (`json.h`)
- [ ] CSV handling (`csv.h`)
- [ ] Configuration files (`config.h`)
- [ ] Text encoding and decoding (`encoding.h`)

#### Phase 6: Concurrency Support 📋
- [ ] Threading primitives (`thread.h`)
- [ ] Synchronization mechanisms (`sync.h`)
- [ ] Thread pool management (`threadpool.h`)

#### Phase 7: Advanced Features 📋
- [ ] Regular expressions (`regex.h`)
- [ ] Iterator abstractions (`iterator.h`)
- [ ] Advanced path operations (`path.h`)

#### Phase 8: Stabilization and Optimization 📋
- [ ] Comprehensive testing across all modules
- [ ] Performance optimization and profiling
- [ ] Documentation completion and examples
- [ ] API finalization and stability guarantees

### Testing Strategy

#### Test Organization
- Unit tests for each module in `tests/` directory
- Each test file follows the naming pattern `tests/<module>.c`
- Makefile-based build system with test targets
- Standalone test executables for each module

#### Testing Style and Conventions

**Function Structure**: Each test file follows a consistent pattern as exemplified in `tests/cstr.c`:
- One test function per feature or logical grouping
- Clear section headers using `printf()` for readability
- Systematic testing of normal cases, edge cases, and error conditions

**Test Function Pattern**:
```c
#include "../dev/macro.h"

void test__feature_name() {
    printf("\n[%s]\n", __func__);
    
    printf(">> Test normal cases:\n");
    ALLC_TEST_ANY("Expected string", "string format... %s", "varargs for string format");
    
    printf(">> Test edge cases:\n");
    ALLC_TEST_ANY("Expected string", "string format... %s", "varargs for string format");
    
    printf(">> Test error conditions:\n");
    ALLC_TEST_ANY("Expected string", "string format... %s", "varargs for string format");
}
```

**Assertion Macros**: 
- Use `ALLC_TEST_ANY(expected, strfmt, varargs...)` for all test assertions, use `cstr.h` for necessary string conversions
- Provide clear, descriptive messages explaining what is being tested
- Focus on readable test output that helps debugging failures

**Loop Testing with Success Variable Pattern**:
When testing operations that require multiple iterations (stress tests, bulk operations, verification loops), use the success variable pattern for early termination and clear failure reporting:

```c
// Pattern: Initialize success, use in loop condition, accumulate results
bool success = true;
for (int i = 0; success && i < num_items; i++) {
    // Test individual operation
    success &= function_under_test(i) == expected_result;
    
    // Optional: Early break on complex conditions
    if (!(success = complex_condition)) 
        break;
        
    // Optional: Additional validation within loop
    if (additional_check) {
        success &= (additional_condition);
    }
}
ALLC_TEST_BOOL(true, success);
```

**Benefits of this approach**:
- **Early Termination**: Loop stops immediately when first failure occurs
- **Single Assertion**: One test result covers the entire batch operation
- **Clear Intent**: Makes it obvious that all iterations must succeed
- **Performance**: Avoids continuing expensive operations after failure
- **Debugging**: Combined with descriptive loop context, pinpoints failure iteration

**Test Coverage Guidelines**:
1. **Normal Operation**: Test typical usage patterns and expected inputs
2. **Boundary Conditions**: Test limits, empty inputs, and maximum values
3. **Error Handling**: Test invalid inputs and error recovery
4. **Memory Management**: Verify proper allocation/deallocation when applicable
5. **Bulk Operations**: Use success variable pattern for stress tests and verification loops
5. **Edge Cases**: Test corner cases specific to the module's functionality

**Input/Output Testing**: 
- Clearly separate input preparation from expected output verification
- Use meaningful test data that represents real-world usage
- Test both successful operations and failure scenarios

**Performance Testing**: Include stress tests for data structures to verify:
- Large dataset handling
- Memory usage patterns
- Performance characteristics under load

#### Test Execution
- Individual module tests can be run standalone
- Integration tests verify module interactions
- All tests must pass before code integration
- Test macros provide clear failure reporting with context

### Documentation Strategy
- Inline documentation in header files
- Usage examples in test files
- README files for complex modules
- Design documents for architectural decisions

## Usage Patterns

### Basic Usage
```c
#define ALLC_IMPL
#include "allc/strbuf.h"
#include "allc/list.h"

int main() {
    allc_allocator_t allocator = allc_allocator_new_c();
    StrBuf str = allc_strbuf_new(allocator, 64);
    // ... use string buffer
    allc_strbuf_delete(str);
    return 0;
}
```

### Selective Implementation
```c
// Only include specific implementations
#define ALLC_IMPL_STRBUF
#define ALLC_IMPL_LIST
#include "allc/strbuf.h"
#include "allc/list.h"
```

### Custom Allocator
```c
// Use custom memory management
allc_allocator_t my_allocator = /* custom allocator */;
List list = allc_list_new(my_allocator);
```

## Future Considerations

### API Stability
- Major version increments for breaking changes
- Deprecation warnings for API changes
- Backward compatibility within major versions

### Platform Support
- Primary focus on POSIX-compliant systems
- Windows compatibility through abstraction layer
- Embedded system considerations

### Performance Optimization
- Profile-guided optimization for hot paths
- Optional SIMD implementations
- Memory pool allocators for specific use cases

### Community
- Clear contribution guidelines
- Code review process
- Issue tracking and feature requests

## Scope for Complete Standard Library

To serve as a comprehensive alternative to system-provided standard libraries, **allc** aims to provide:

### Core Requirements
1. **Data Structures**: Complete set of fundamental data structures (arrays, lists, hash tables, sets, trees, queues, stacks, heaps)
2. **Algorithms**: Sorting, searching, and manipulation algorithms for all data structures
3. **Mathematical Operations**: Basic arithmetic, trigonometry, statistics, and bit manipulation
4. **String Processing**: Comprehensive string manipulation including Unicode support
5. **Input/Output**: File operations, serialization formats (JSON, CSV), and text encoding
6. **System Integration**: Process management, file system operations, network communications
7. **Memory Management**: Advanced allocators, memory pools, and garbage collection options
8. **Concurrency**: Threading, synchronization primitives, and parallel processing support
9. **Time and Date**: Comprehensive date/time handling with timezone support
10. **Cryptography**: Basic cryptographic functions and secure random number generation

### Differentiation from System Libraries
- **Consistent API**: All modules follow the same naming and error handling conventions
- **Allocator Abstraction**: Every module supports custom memory management
- **Header-Only Distribution**: Easy integration without build system complexity
- **Cross-Platform**: Same API across different operating systems
- **Zero Dependencies**: No external library requirements beyond system calls

### Target Use Cases
- **System Programming**: Low-level applications requiring portable system abstractions
- **Library Development**: Building higher-level libraries with consistent foundations
- **Educational Projects**: Learning C programming with well-documented, readable code
