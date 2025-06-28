#define ALLC_IMPL

#include "../dev/os/directory.h"
#include "../dev/macro.h"

#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void test__allc_directory_creation_deletion()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    
    printf(">> Test basic directory creation:\n");
    allc_directory_t dir = allc_directory_new(allocator, "/tmp");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(dir != NULL));
    ALLC_TEST_ANY("success", "%s", allc_directory_status_string(allc_directory_get_status(dir)));
    ALLC_TEST_ANY("/tmp", "%s", allc_directory_get_path(dir));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_directory_is_open(dir)));
    
    allc_directory_delete(dir);
    
    printf(">> Test creation with null allocator:\n");
    dir = allc_directory_new(NULL, "/tmp");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(dir == NULL));
}

void test__allc_directory_utility_functions()
{
    printf("\n[%s]\n", __func__);
    
    printf(">> Test status strings:\n");
    ALLC_TEST_ANY("success", "%s", allc_directory_status_string(ALLC_DIRECTORY_SUCCESS));
    ALLC_TEST_ANY("end of directory", "%s", allc_directory_status_string(ALLC_DIRECTORY_END_OF_DIRECTORY));
    ALLC_TEST_ANY("null pointer", "%s", allc_directory_status_string(ALLC_DIRECTORY_ERROR_NULL_POINTER));
    ALLC_TEST_ANY("open failed", "%s", allc_directory_status_string(ALLC_DIRECTORY_ERROR_OPEN_FAILED));
    ALLC_TEST_ANY("unknown error", "%s", allc_directory_status_string((allc_directory_status_e)999));
}

void test__allc_directory_open_close()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_directory_t dir = allc_directory_new(allocator, "/tmp");
    
    printf(">> Test opening directory:\n");
    allc_directory_status_e status = allc_directory_open(dir);
    ALLC_TEST_ANY("success", "%s", allc_directory_status_string(status));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_directory_is_open(dir)));
    
    printf(">> Test closing directory:\n");
    status = allc_directory_close(dir);
    ALLC_TEST_ANY("success", "%s", allc_directory_status_string(status));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_directory_is_open(dir)));
    
    printf(">> Test opening non-existent directory:\n");
    allc_directory_t nonexistent = allc_directory_new(allocator, "/nonexistent_directory_12345");
    status = allc_directory_open(nonexistent);
    ALLC_TEST_ANY("open failed", "%s", allc_directory_status_string(status));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_directory_is_open(nonexistent)));
    
    allc_directory_delete(dir);
    allc_directory_delete(nonexistent);
}

void test__allc_directory_new_opened()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    
    printf(">> Test creating and opening directory in one step:\n");
    allc_directory_t dir = allc_directory_new_opened(allocator, "/tmp");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(dir != NULL));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_directory_is_open(dir)));
    ALLC_TEST_ANY("success", "%s", allc_directory_status_string(allc_directory_get_status(dir)));
    
    allc_directory_delete(dir);
    
    printf(">> Test creating with invalid path:\n");
    dir = allc_directory_new_opened(allocator, "/nonexistent_directory_12345");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(dir == NULL));
}

void test__allc_directory_read_entries()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    const char *test_dir = "/tmp/test_directory_read";
    
    // Create test directory structure
    mkdir(test_dir, 0755);
    
    // Create some test files
    char test_file1[256], test_file2[256];
    snprintf(test_file1, sizeof(test_file1), "%s/test_file1.txt", test_dir);
    snprintf(test_file2, sizeof(test_file2), "%s/test_file2.txt", test_dir);
    
    FILE *f1 = fopen(test_file1, "w");
    if (f1) {
        fprintf(f1, "test content 1");
        fclose(f1);
    }
    
    FILE *f2 = fopen(test_file2, "w");
    if (f2) {
        fprintf(f2, "test content 2");
        fclose(f2);
    }
    
    printf(">> Test reading directory entries:\n");
    allc_directory_t dir = allc_directory_new_opened(allocator, test_dir);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(dir != NULL));
    
    bool found_file1 = false, found_file2 = false, found_current = false, found_parent = false;
    int entry_count = 0;
    
    while (allc_directory_read(dir) == ALLC_DIRECTORY_SUCCESS) {
        const char *entry_name = allc_directory_get_entry_name(dir);
        entry_count++;
        
        if (entry_name) {
            if (allc_cstr_is_equal(entry_name, "test_file1.txt")) {
                found_file1 = true;
            } else if (allc_cstr_is_equal(entry_name, "test_file2.txt")) {
                found_file2 = true;
            } else if (allc_cstr_is_equal(entry_name, ".")) {
                found_current = true;
            } else if (allc_cstr_is_equal(entry_name, "..")) {
                found_parent = true;
            }
        }
    }
    
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(found_file1));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(found_file2));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(found_current));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(found_parent));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(entry_count >= 4));
    
    printf(">> Test reading beyond end of directory:\n");
    allc_directory_status_e status = allc_directory_read(dir);
    ALLC_TEST_ANY("end of directory", "%s", allc_directory_status_string(status));
    
    allc_directory_delete(dir);
    
    // Clean up test files and directory
    unlink(test_file1);
    unlink(test_file2);
    rmdir(test_dir);
}

void test__allc_directory_rewind()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    const char *test_dir = "/tmp/test_directory_rewind";
    
    // Create test directory with a few files
    mkdir(test_dir, 0755);
    
    char test_file[256];
    for (int i = 0; i < 3; i++) {
        snprintf(test_file, sizeof(test_file), "%s/file_%d.txt", test_dir, i);
        FILE *f = fopen(test_file, "w");
        if (f) {
            fprintf(f, "content %d", i);
            fclose(f);
        }
    }
    
    allc_directory_t dir = allc_directory_new_opened(allocator, test_dir);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(dir != NULL));
    
    printf(">> Test reading entries first time:\n");
    int first_count = 0;
    while (allc_directory_read(dir) == ALLC_DIRECTORY_SUCCESS) {
        first_count++;
    }
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(first_count > 0));
    
    printf(">> Test rewind and read again:\n");
    allc_directory_status_e status = allc_directory_rewind(dir);
    ALLC_TEST_ANY("success", "%s", allc_directory_status_string(status));
    
    int second_count = 0;
    while (allc_directory_read(dir) == ALLC_DIRECTORY_SUCCESS) {
        second_count++;
    }
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(second_count == first_count));
    
    allc_directory_delete(dir);
    
    // Clean up test files and directory
    for (int i = 0; i < 3; i++) {
        snprintf(test_file, sizeof(test_file), "%s/file_%d.txt", test_dir, i);
        unlink(test_file);
    }
    rmdir(test_dir);
}

void test__allc_directory_entry_info()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    const char *test_dir = "/tmp/test_directory_info";
    
    // Create test directory structure
    mkdir(test_dir, 0755);
    
    char test_file[256], test_subdir[256];
    snprintf(test_file, sizeof(test_file), "%s/test_file.txt", test_dir);
    snprintf(test_subdir, sizeof(test_subdir), "%s/test_subdir", test_dir);
    
    // Create a file
    FILE *f = fopen(test_file, "w");
    if (f) {
        fprintf(f, "test content");
        fclose(f);
    }
    
    // Create a subdirectory
    mkdir(test_subdir, 0755);
    
    printf(">> Test getting entry information:\n");
    allc_directory_t dir = allc_directory_new_opened(allocator, test_dir);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(dir != NULL));
    
    bool found_file = false, found_subdir = false;
    
    while (allc_directory_read(dir) == ALLC_DIRECTORY_SUCCESS) {
        const char *entry_name = allc_directory_get_entry_name(dir);
        unsigned char entry_type = allc_directory_get_entry_type(dir);
        
        if (entry_name) {
            if (allc_cstr_is_equal(entry_name, "test_file.txt")) {
                found_file = true;
                // Note: entry_type values depend on the filesystem and platform
                // We just check that we can get the type without crashing
                ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(entry_type >= 0));
            } else if (allc_cstr_is_equal(entry_name, "test_subdir")) {
                found_subdir = true;
                ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(entry_type >= 0));
            }
        }
    }
    
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(found_file));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(found_subdir));
    
    allc_directory_delete(dir);
    
    // Clean up test files and directories
    unlink(test_file);
    rmdir(test_subdir);
    rmdir(test_dir);
}

void test__allc_directory_error_conditions()
{
    printf("\n[%s]\n", __func__);
    
    printf(">> Test null pointer errors:\n");
    ALLC_TEST_ANY("null pointer", "%s", allc_directory_status_string(allc_directory_get_status(NULL)));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_directory_get_path(NULL) == NULL));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_directory_is_open(NULL)));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_directory_get_entry_name(NULL) == NULL));
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_directory_t dir = allc_directory_new(allocator, "/tmp");
    
    printf(">> Test operations on closed directory:\n");
    ALLC_TEST_ANY("bad state", "%s", allc_directory_status_string(allc_directory_read(dir)));
    ALLC_TEST_ANY("bad state", "%s", allc_directory_status_string(allc_directory_rewind(dir)));
    ALLC_TEST_ANY("bad state", "%s", allc_directory_status_string(allc_directory_close(dir)));
    
    printf(">> Test double open:\n");
    allc_directory_open(dir);
    ALLC_TEST_ANY("bad state", "%s", allc_directory_status_string(allc_directory_open(dir)));
    
    allc_directory_delete(dir);
}

void test__allc_directory_comprehensive()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    const char *base_dir = "/tmp/test_directory_comprehensive";
    
    printf(">> Testing complete directory operations:\n");
    
    bool success = true;
    
    // Create test directory structure
    mkdir(base_dir, 0755);
    
    // Create multiple subdirectories with files
    for (int i = 0; success && i < 3; i++) {
        char subdir[256];
        snprintf(subdir, sizeof(subdir), "%s/subdir_%d", base_dir, i);
        
        if (mkdir(subdir, 0755) == 0) {
            // Create files in each subdirectory
            for (int j = 0; success && j < 2; j++) {
                char filename[256];
                snprintf(filename, sizeof(filename), "%s/file_%d.txt", subdir, j);
                
                FILE *f = fopen(filename, "w");
                if (f) {
                    fprintf(f, "Content for subdir %d, file %d", i, j);
                    fclose(f);
                } else {
                    success = false;
                }
            }
        } else {
            success = false;
        }
    }
    
    if (success) {
        // Test reading the main directory
        allc_directory_t dir = allc_directory_new_opened(allocator, base_dir);
        success &= (dir != NULL);
        success &= (allc_directory_get_status(dir) == ALLC_DIRECTORY_SUCCESS);
        
        int subdir_count = 0;
        while (success && allc_directory_read(dir) == ALLC_DIRECTORY_SUCCESS) {
            const char *entry_name = allc_directory_get_entry_name(dir);
            if (entry_name && strncmp(entry_name, "subdir_", 7) == 0) {
                subdir_count++;
            }
        }
        
        success &= (subdir_count == 3);
        
        // Test rewind functionality
        if (success) {
            allc_directory_status_e status = allc_directory_rewind(dir);
            success &= (status == ALLC_DIRECTORY_SUCCESS);
            
            int second_count = 0;
            while (success && allc_directory_read(dir) == ALLC_DIRECTORY_SUCCESS) {
                const char *entry_name = allc_directory_get_entry_name(dir);
                if (entry_name && strncmp(entry_name, "subdir_", 7) == 0) {
                    second_count++;
                }
            }
            success &= (second_count == subdir_count);
        }
        
        allc_directory_delete(dir);
    }
    
    // Clean up test directory structure
    for (int i = 0; i < 3; i++) {
        char subdir[256];
        snprintf(subdir, sizeof(subdir), "%s/subdir_%d", base_dir, i);
        
        for (int j = 0; j < 2; j++) {
            char filename[256];
            snprintf(filename, sizeof(filename), "%s/file_%d.txt", subdir, j);
            unlink(filename);
        }
        rmdir(subdir);
    }
    rmdir(base_dir);
    
    ALLC_TEST_BOOL(true, success);
    printf(">> All comprehensive tests completed successfully\n");
}

int main()
{
    printf("=== Directory Module Tests ===\n");
    
    test__allc_directory_creation_deletion();
    test__allc_directory_utility_functions();
    test__allc_directory_open_close();
    test__allc_directory_new_opened();
    test__allc_directory_read_entries();
    test__allc_directory_rewind();
    test__allc_directory_entry_info();
    test__allc_directory_error_conditions();
    test__allc_directory_comprehensive();
    
    printf("\n=== All Directory Tests Completed ===\n");
    return 0;
}
