# AGENTS.md - OS Abstraction Layer

## Module Context

This directory contains **operating system abstraction headers** that provide cross-platform interfaces for system-level operations. These modules isolate platform-specific code and provide consistent APIs across different operating systems.

## Architecture Goals

### Abstraction Strategy
- **Consistent Interface** - Same API across all platforms
- **Platform Detection** - Use preprocessor to select appropriate implementation
- **Graceful Degradation** - Provide fallbacks when features unavailable
- **Minimal Dependencies** - Rely primarily on standard C libraries

### Platform Support Priority
1. **POSIX-compliant systems** (Linux, macOS, BSD) - Primary target
2. **Windows** - Secondary through abstraction layer
3. **Embedded systems** - Tertiary with minimal feature set

## Module Breakdown

### Core System Modules

#### `process.h` - Process Management
- **Purpose**: Process creation, execution, and monitoring
- **Key Functions**: 
  - `allc_process_spawn()` - Create new process
  - `allc_process_wait()` - Wait for process completion
  - `allc_process_kill()` - Terminate process
- **Platform Considerations**: fork/exec vs CreateProcess
- **Dependencies**: Basic types, error handling

#### `file.h` - File Operations
- **Purpose**: File I/O operations beyond standard C library
- **Key Functions**:
  - File attribute queries
  - File copying and moving
  - Atomic file operations
- **Platform Considerations**: Different file attribute systems
- **Dependencies**: Path handling, error codes

#### `directory.h` - Directory Operations
- **Purpose**: Directory traversal and manipulation
- **Key Functions**:
  - Directory creation/removal
  - Directory listing and iteration
  - Recursive operations
- **Platform Considerations**: Path separators, permission models
- **Dependencies**: Path handling, memory allocation

#### `memory.h` - Memory Management Primitives
- **Purpose**: Low-level memory operations
- **Key Functions**:
  - Memory mapping
  - Shared memory
  - Memory protection
- **Platform Considerations**: Virtual memory systems
- **Dependencies**: Core allocator system

### System Information Modules

#### `time.h` - Time and Date
- **Purpose**: Time measurement and formatting
- **Key Functions**:
  - High-resolution timers
  - Date/time formatting
  - Time zone handling
- **Platform Considerations**: Different timer APIs
- **Dependencies**: String formatting

#### `environ.h` - Environment Variables
- **Purpose**: Environment variable access and manipulation
- **Key Functions**:
  - Get/set environment variables
  - Environment variable enumeration
  - Path variable parsing
- **Platform Considerations**: Different environment models
- **Dependencies**: String handling

#### `network.h` - Network Operations
- **Purpose**: Basic network communication
- **Key Functions**:
  - Socket creation and management
  - Network address resolution
  - Basic HTTP/TCP operations
- **Platform Considerations**: Winsock vs BSD sockets
- **Dependencies**: String handling, memory management

### Enhanced System Integration

#### `memory.h` - Advanced Memory Management
- **Purpose**: System-level memory operations beyond malloc/free
- **Key Functions**:
  - Memory mapping and unmapping
  - Shared memory operations
  - Memory protection and permissions
  - Virtual memory statistics
- **Platform Considerations**: mmap vs VirtualAlloc, memory protection flags
- **Dependencies**: Allocator abstraction, error handling

#### `network.h` - Network Operations
- **Purpose**: Cross-platform network communications
- **Key Functions**:
  - Socket creation and management
  - TCP/UDP client and server operations
  - DNS resolution
  - Network interface enumeration
- **Platform Considerations**: Winsock vs Berkeley sockets
- **Dependencies**: String handling, error codes, threading

#### `time.h` - Time and System Clock
- **Purpose**: High-resolution timing and system clock operations
- **Key Functions**:
  - High-resolution timestamps
  - System uptime and boot time
  - CPU time measurement
  - Sleep and timer operations
- **Platform Considerations**: Different time APIs and resolutions
- **Dependencies**: Basic types, mathematical operations

#### `environ.h` - Environment Variable Access
- **Purpose**: Environment variable manipulation
- **Key Functions**:
  - Environment variable get/set/unset
  - Environment variable enumeration
  - Path environment parsing
- **Platform Considerations**: Different environment variable formats
- **Dependencies**: String handling, dynamic arrays

### Security and Permissions

#### `security.h` - Security Context Operations (Planned)
- **Purpose**: User and group management, permissions
- **Key Functions**:
  - User and group ID operations
  - Permission checking and modification
  - Security context switching
- **Platform Considerations**: POSIX permissions vs Windows ACLs
- **Dependencies**: String handling, error codes

### Advanced System Features

#### `ipc.h` - Inter-Process Communication (Planned)
- **Purpose**: Process communication mechanisms
- **Key Functions**:
  - Pipes and named pipes
  - Message queues
  - Semaphores and shared memory
- **Platform Considerations**: POSIX IPC vs Windows named objects
- **Dependencies**: Process management, memory operations

#### `hardware.h` - Hardware Information (Planned)
- **Purpose**: System hardware information
- **Key Functions**:
  - CPU information and capabilities
  - Memory configuration
  - Storage device enumeration
- **Platform Considerations**: Different hardware query mechanisms
- **Dependencies**: String handling, mathematical operations

## Implementation Patterns

### Platform Detection
```c
#ifdef _WIN32
    // Windows-specific implementation
#elif defined(__unix__) || defined(__APPLE__)
    // POSIX-compliant implementation
#else
    #error "Unsupported platform"
#endif
```

### Error Handling Strategy
```c
typedef enum {
    ALLC_OS_SUCCESS = 0,
    ALLC_OS_ERROR_PERMISSION,
    ALLC_OS_ERROR_NOT_FOUND,
    ALLC_OS_ERROR_NO_MEMORY,
    // ... platform-specific errors mapped to common codes
} allc_os_error_t;
```

### Resource Management
```c
// RAII-style resource handling
typedef struct {
    allc_allocator_t allocator;
    void *platform_handle;
    allc_os_error_t last_error;
} allc_os_resource_t;

allc_os_resource_t allc_os_resource_new(allc_allocator_t allocator);
void allc_os_resource_delete(allc_os_resource_t *self);
```

## Development Guidelines

### When Implementing OS Modules

1. **Start with POSIX** - Implement POSIX version first as it's most common
2. **Abstract Early** - Don't expose platform-specific types in public API
3. **Error Mapping** - Map platform errors to common error codes
4. **Test Thoroughly** - Test on multiple platforms if possible
5. **Document Limitations** - Clearly document platform-specific behaviors

### Platform-Specific Code Organization
```c
// In header file - common interface
allc_os_error_t allc_process_spawn(const char *executable, char *argv[]);

// In implementation section
#ifdef ALLC_OS_IMPL

#ifdef _WIN32
static allc_os_error_t spawn_windows(const char *executable, char *argv[]) {
    // Windows implementation
}
#endif

#ifdef __unix__
static allc_os_error_t spawn_posix(const char *executable, char *argv[]) {
    // POSIX implementation
}
#endif

allc_os_error_t allc_process_spawn(const char *executable, char *argv[]) {
#ifdef _WIN32
    return spawn_windows(executable, argv);
#elif defined(__unix__)
    return spawn_posix(executable, argv);
#else
    return ALLC_OS_ERROR_UNSUPPORTED;
#endif
}

#endif // ALLC_OS_IMPL
```

### Testing Strategy
- **Unit Tests** - Test each function with known inputs
- **Integration Tests** - Test cross-module interactions
- **Platform Tests** - Verify behavior on different platforms
- **Error Tests** - Test error conditions and recovery

### Dependencies Management
- **Minimize Dependencies** - Avoid platform-specific libraries when possible
- **Feature Detection** - Use compile-time checks for optional features
- **Graceful Degradation** - Provide limited functionality when features unavailable

## Security Considerations

### Process Operations
- Validate all executable paths
- Sanitize command-line arguments
- Handle process privileges carefully

### File Operations
- Validate all file paths (prevent directory traversal)
- Handle file permissions properly
- Use secure temporary file creation

### Network Operations
- Validate network addresses and ports
- Handle connection timeouts
- Implement proper certificate validation for HTTPS

## Performance Considerations

### System Call Overhead
- Batch operations when possible
- Cache frequently accessed information
- Use appropriate buffer sizes

### Memory Management
- Minimize system memory allocations
- Use memory mapping for large files
- Implement proper cleanup on errors

## Future Expansion

### Additional Modules
- **thread.h** - Threading and synchronization primitives
- **ipc.h** - Inter-process communication
- **device.h** - Device enumeration and access
- **security.h** - Security context and permissions

### Platform Support
- Plan for additional platforms as needed
- Consider embedded system constraints
- Evaluate mobile platform support
