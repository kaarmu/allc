#define ALLC_DIRECTORY__IMPL
#define ALLC_FILE__IMPL


#include "src/os/file.h"
#include "src/os/directory.h"

#include <stdio.h>

int main() {
    allc_allocator_t allocator = allc_allocator_c();

    // Test File functionality
    File file = allc_file_new_opened(allocator, "test.txt", "w");
    if (!file || file->status != ALLC_FILE_SUCCESS) {
        printf("Failed to open test.txt for writing.\n");
        return 1;
    }

    char data[] = "Hello, World!";
    allc_file_write(&file, sizeof(data) - 1, data);

    if (file->status != ALLC_FILE_SUCCESS) {
        printf("Failed to write to test.txt.\n");
        return 1;
    }

    allc_file_close(&file);
    if (file->status != ALLC_FILE_SUCCESS) {
        printf("Failed to close test.txt.\n");
        return 1;
    }

    allc_file_delete(file);

    // Test Directory functionality
    Directory dir = allc_directory_new(allocator, ".");

    allc_directory_open(&dir);
    if (dir->status != ALLC_DIRECTORY_SUCCESS) {
        printf("Failed to open current directory.\n");
        return 1;
    }

    while (dir->status == ALLC_DIRECTORY_SUCCESS) {
    	dir->status = allc_directory_read(&dir);
        printf("Read\n");
	if (dir->status == ALLC_DIRECTORY_SUCCESS) {
            printf("[%d] Found entry: %s\n", dir->status, dir->entry->d_name);
	} else {
            printf("[%d] Error\n", dir->status);
 	}
    }

    if (dir->status != ALLC_DIRECTORY_SUCCESS) {
        printf("Failed reading directory entries.\n");
        return 1;
    }
    printf("Found all entries.\n");

    allc_directory_close(&dir);
    if (dir->status != ALLC_DIRECTORY_SUCCESS) {
        printf("Failed to close directory.\n");
        return 1;
    }
    printf("Closed directory.\n");

    allc_directory_delete(dir);

    printf("All tests passed!\n");
    return 0;
}

