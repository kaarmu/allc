# AGENTS.md - Test Suite

## Testing Context

This directory contains **unit tests** for all allc modules. Each test file corresponds to a specific module and validates its functionality, error handling, and edge cases.

## Testing Philosophy

### Comprehensive Coverage
- **Function Coverage** - Test all public functions
- **Branch Coverage** - Test all code paths including error conditions
- **Edge Cases** - Test boundary conditions and unusual inputs
- **Memory Management** - Verify no memory leaks or corruption

### Test Organization
- One test file per module (`module.c` tests `../dev/module.h`)
- Test functions named `test__allc_module_function()`
- Tests grouped by functionality within each file
- Clear section headers using `printf()` for readability
- Follow the pattern: normal cases, edge cases, error conditions

## Test Infrastructure

### Testing Macros (from `macro.h`)
```c
// Primary testing macro - test any value with format string
ALLC_TEST_ANY(expected_string, format, actual_value)

// Debug value printing
ALLC_DEBUG_VALUE(format, value)
```

### Test Structure Pattern
```c
#define ALLC_IMPL

#include "../dev/cstr.h"
#include "../dev/macro.h"

void test__allc_module_function() {
    printf("\n[%s]\n", __func__);
    
    // Test normal cases
    ALLC_TEST_ANY("expected_output", "%s", actual_result);
    
    // Test edge cases  
    ALLC_TEST_ANY("edge_case_output", "%d", edge_result);
    
    // Test error conditions
    ALLC_TEST_ANY("error_output", "%s", error_result);
}

int main() {
    test__allc_module_function();
    // ... other test calls
    return 0;
}
```
    printf("\n[%s]\n", __func__);
    
    // Setup
    allc_allocator_t allocator = allc_allocator_new_c();
    ModuleType instance = allc_module_new(allocator);
    
    // Test basic functionality
    result = allc_module_function(instance, test_input);
    ALLC_TEST_ANY("expected_output", "%s", result);
    
    // Cleanup
    allc_module_delete(instance);
}

void test__allc_module_function_edge_cases() {
    printf("\n[%s]\n", __func__);
    
    // Test NULL inputs
    result = allc_module_function(NULL, test_input);
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(result == expected_error));
    
    // Test empty inputs
    // Test boundary conditions
    // etc.
}

int main() {
    test__allc_module_function_basic();
    test__allc_module_function_edge_cases();
    return 0;
}
```

## Current Test Files

### `cstr.c` - C String Utilities
- **Functions Tested**: String inspection, manipulation, searching
- **Focus Areas**: 
  - String classification (digit, alpha, blank, etc.)
  - String manipulation (strip, replace, shift)
  - String searching (find character, find substring)
- **Edge Cases**: Empty strings, NULL pointers, boundary conditions

### `list.c` - Doubly Linked List
- **Functions Tested**: List creation, modification, traversal
- **Focus Areas**:
  - List construction and destruction
  - Element insertion and removal
  - List traversal and access
- **Edge Cases**: Empty lists, single elements, memory management

### `path.c` - File System Path Handling
- **Functions Tested**: Path creation, manipulation, resolution
- **Focus Areas**:
  - Path normalization and resolution
  - Cross-platform path handling
  - Working directory operations
- **Edge Cases**: Relative paths, path separators, invalid paths

### `strbuf.c` - Dynamic String Buffer
- **Functions Tested**: Buffer creation, modification, growth
- **Focus Areas**:
  - Buffer growth and capacity management
  - String operations (append, insert, format)
  - Memory management
- **Edge Cases**: Buffer overflow, format string edge cases

## Planned Test Files

### Data Structure Tests
- **`array.c`** - Dynamic array resizing, slicing, element access
- **`hashmap.c`** - Hash table operations, collision handling, resizing
- **`set.c`** - Set operations, membership testing, iteration
- **`queue.c`** - FIFO/LIFO operations, ring buffer behavior
- **`heap.c`** - Priority queue operations, heap properties
- **`tree.c`** - Tree construction, traversal, balancing

### Mathematical Tests
- **`math.c`** - Mathematical functions, precision, edge cases
- **`random.c`** - Random number generation, distribution testing
- **`crypto.c`** - Hash function correctness, encoding/decoding

### I/O and Serialization Tests
- **`json.c`** - JSON parsing/generation, malformed input handling
- **`csv.c`** - CSV parsing with various delimiters and escaping
- **`config.c`** - Configuration file parsing (INI, TOML)
- **`encoding.c`** - Text encoding/decoding, URL encoding
- **`unicode.c`** - Unicode validation, normalization, conversion

### Concurrency Tests
- **`thread.c`** - Thread creation, joining, synchronization
- **`sync.c`** - Mutex, condition variables, atomics
- **`threadpool.c`** - Work distribution, thread pool sizing

### Time and Date Tests
- **`datetime.c`** - Date parsing, formatting, arithmetic, timezones

## Testing Standards for New Modules

### Required Test Categories
1. **Constructor/Destructor Tests**
   - Verify proper initialization
   - Test memory allocation/deallocation
   - Validate cleanup behavior

2. **Core Functionality Tests**
   - Test primary use cases
   - Verify expected return values
   - Test function interactions

3. **Error Condition Tests**
   - NULL pointer handling
   - Invalid parameter validation
   - Out-of-memory conditions
   - Boundary condition failures

4. **Memory Management Tests**
   - No memory leaks (use valgrind or similar)
   - Proper allocator usage
   - Double-free protection

5. **Performance Regression Tests**
   - Basic performance benchmarks
   - Complexity validation for data structures
   - Memory usage validation

### Test Naming Conventions
- Test functions: `test__allc_module_function_scenario()`
- Test files: `module.c` (matches `../dev/module.h`)
- Test categories: `_basic`, `_edge_cases`, `_error_conditions`, `_performance`

### Test Data Organization
- Use representative test data sets
- Include edge cases (empty, single element, maximum size)
- Test with both valid and invalid inputs
- Use consistent test data across similar functions
````
