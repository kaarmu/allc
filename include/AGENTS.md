# AGENTS.md - Stable Public Headers

## Directory Purpose

This directory is intended to contain **stable, public headers** for the allc project. Currently empty, it will house mature modules that have been promoted from the `dev/` directory.

## Promotion Criteria

### From Development to Stable
A module can be promoted from `dev/` to `include/` when it meets these criteria:

#### API Stability
- ✅ **Complete Interface** - All planned functions implemented
- ✅ **Consistent Naming** - Follows project naming conventions
- ✅ **Stable ABI** - Binary interface won't change in minor versions
- ✅ **Comprehensive Documentation** - All functions documented with examples

#### Quality Assurance
- ✅ **Full Test Coverage** - All functions tested with edge cases
- ✅ **Memory Safety** - No memory leaks or buffer overflows
- ✅ **Error Handling** - Proper error codes and logging
- ✅ **Platform Testing** - Verified on multiple platforms

#### Performance
- ✅ **Benchmarked** - Performance characteristics documented
- ✅ **Optimized** - No obvious performance bottlenecks
- ✅ **Scalable** - Handles reasonable input sizes efficiently

#### Integration
- ✅ **Dependency Stability** - All dependencies are also stable
- ✅ **Build System** - Proper Makefile integration
- ✅ **Example Usage** - Working examples in documentation

## Stable Module Organization

### Expected Structure (Future)
```
include/
├── allc/
│   ├── core/
│   │   ├── types.h          # Fundamental types
│   │   ├── allocator.h      # Memory management
│   │   ├── cstr.h           # C string utilities
│   │   └── macro.h          # Utility macros
│   ├── containers/
│   │   ├── list.h           # Doubly linked list
│   │   ├── strbuf.h         # Dynamic string buffer
│   │   └── array.h          # Dynamic array (future)
│   ├── system/
│   │   ├── path.h           # File system paths
│   │   ├── logger.h         # Logging system
│   │   └── os/              # OS abstraction
│   └── tools/
│       ├── builder.h        # Build system utilities
│       └── regex.h          # Regular expressions
├── allc.h                   # Single header that includes everything
└── README.md                # Usage documentation
```

### Versioning Strategy
- **Major Version** - Breaking API changes
- **Minor Version** - New features, backward compatible
- **Patch Version** - Bug fixes, no API changes

## Usage Patterns for Stable Headers

### Single Header Inclusion
```c
#define ALLC_IMPL
#include "allc.h"  // Includes everything

int main() {
    // Use any allc functionality
    return 0;
}
```

### Selective Inclusion
```c
#define ALLC_IMPL_CORE
#define ALLC_IMPL_CONTAINERS
#include "allc/core/types.h"
#include "allc/containers/list.h"

int main() {
    // Use only selected modules
    return 0;
}
```

### Module-Specific Usage
```c
#define ALLC_IMPL_STRBUF
#include "allc/containers/strbuf.h"

int main() {
    // Use only string buffer functionality
    return 0;
}
```

## Migration Process

### From dev/ to include/
1. **Final API Review** - Ensure API is complete and stable
2. **Comprehensive Testing** - Run full test suite
3. **Documentation Update** - Finalize all documentation
4. **Performance Validation** - Confirm performance characteristics
5. **Integration Testing** - Test with other stable modules
6. **Copy to include/** - Move header to appropriate location
7. **Update Build System** - Modify Makefile for new location
8. **Version Tagging** - Tag release with new stable module

### Backward Compatibility
- **Deprecation Process** - Mark old APIs as deprecated before removal
- **Migration Guide** - Provide clear upgrade instructions
- **Compatibility Headers** - Provide compatibility wrappers if needed

## Distribution Strategy

### Header-Only Distribution
- **Single File** - Each module in a single header file
- **Self-Contained** - Minimal external dependencies
- **Easy Integration** - Simple copy-and-include usage

### Package Distribution
- **Archive Releases** - Tarball/zip with all stable headers
- **Git Tags** - Tagged releases for specific versions
- **Documentation** - Comprehensive usage examples

## Quality Assurance for Stable Headers

### Automated Testing
- **Continuous Integration** - Automated testing on multiple platforms
- **Regression Testing** - Ensure changes don't break existing functionality
- **Performance Monitoring** - Track performance over time

### Code Review Process
- **Peer Review** - All changes reviewed by project maintainers
- **API Review** - Special attention to public interface changes
- **Documentation Review** - Ensure documentation stays current

### User Feedback Integration
- **Issue Tracking** - Systematic bug reporting and resolution
- **Feature Requests** - Community input on new features
- **Usage Analytics** - Understanding how modules are used

## Future Considerations

### Expansion Plans
- **Additional Modules** - More functionality as it matures
- **Platform Support** - Broader platform compatibility
- **Performance Optimization** - Continuous performance improvements

### Community
- **Contribution Guidelines** - Clear process for external contributions
- **Maintenance Policy** - Long-term support commitments
- **Release Schedule** - Regular, predictable releases

### Compatibility
- **Language Standards** - Support for newer C standards
- **Compiler Support** - Compatibility with major compilers
- **Platform Evolution** - Adaptation to new platforms

## Current Status

⚠️ **No stable headers yet** - All modules are still in development phase in `dev/` directory.

### Next Candidates for Promotion
1. **types.h** - Fundamental and stable
2. **cstr.h** - Mature string utilities
3. **macro.h** - Stable utility macros
4. **allocator.h** - Core memory management (needs testing)

### Modules Needing More Work
1. **strbuf.h** - Implementation gaps
2. **list.h** - Needs more testing
3. **path.h** - Cross-platform testing needed
4. **logger.h** - API refinement needed
5. **os/** modules - Platform testing required
