/**
 *  Memory view.
 *
 *  Description
 *  ===========
 *  << What is this module about? >>
 *
 *  Options
 *  =======
 *  << What compilation options is available? >>
 *
 *  Content
 *  =======
 *  << What types/subroutines does this module provide? >>
 *
**/

#include <sys/types.h>

// NOTE: This is just proof-of-concept/idea
struct AllCView {
    size_t size;
    const unsigned char data[];
    // void *data;
};

