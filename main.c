#include <stdio.h>

#define ALLC_IMPL
#include "src/cstr.h"
#include "src/path.h"
#include "src/os/process.h"

int main() {

    allc_allocator_t a = allc_allocator_new_c();

    Path cwd = allc_path_new_from_cwd(a);
    printf("cwd: (abs) %s, %s\n", 
        allc_cstr_repr_bool(allc_path_is_absolute(&cwd)),
        cwd->buf);
    allc_path_delete(cwd);

    Path p0 = allc_path_new_from_cstr(a, "foo/bar");
    printf("p0: (abs) %s, %s\n", 
        allc_cstr_repr_bool(allc_path_is_absolute(&p0)),
        p0->buf);

    p0 = allc_path_to_absolute_path(p0);
    printf("p0: %s\n", p0->buf);

    allc_path_delete(p0);

    Path p1 = allc_path_new_from_cwd(a);
    printf("p1: %s\n", p1->buf);

    Path p2 = allc_path_new_from_home(a);
    printf("p2: %s\n", p2->buf);
    printf("p1 == p2: %s\n",
           allc_cstr_repr_bool(allc_path_is_equal(&p1, &p2)));

    allc_path_delete(p2);
    allc_path_delete(p1);
    
    char * argv[] = { "/bin/echo", "echo", "Hello, wordl!", NULL };
    int pid = allc_process_spawn_wait("/bin/echo", argv);

    printf("pid: %d\n", pid);

}
