#define ALLC_IMPL

#include "../dev/os/file.h"
#include "../dev/macro.h"

#include <string.h>
#include <unistd.h>

void test__allc_file_creation_deletion()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    
    printf(">> Test basic file creation:\n");
    allc_file_t file = allc_file_new(allocator, "/tmp/test_file.txt");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(file != NULL));
    ALLC_TEST_ANY("success", "%s", allc_file_status_string(allc_file_get_status(file)));
    ALLC_TEST_ANY("/tmp/test_file.txt", "%s", allc_file_get_path(file));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_file_is_open(file)));
    
    allc_file_delete(file);
    
    printf(">> Test creation with null allocator:\n");
    file = allc_file_new(NULL, "/tmp/test_file.txt");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(file == NULL));
}

void test__allc_file_utility_functions()
{
    printf("\n[%s]\n", __func__);
    
    printf(">> Test status strings:\n");
    ALLC_TEST_ANY("success", "%s", allc_file_status_string(ALLC_FILE_SUCCESS));
    ALLC_TEST_ANY("end of file", "%s", allc_file_status_string(ALLC_FILE_STATUS_EOF));
    ALLC_TEST_ANY("null pointer", "%s", allc_file_status_string(ALLC_FILE_ERROR_NULL_POINTER));
    ALLC_TEST_ANY("open failed", "%s", allc_file_status_string(ALLC_FILE_ERROR_OPEN_FAILED));
    ALLC_TEST_ANY("unknown error", "%s", allc_file_status_string((allc_file_status_e)999));
}

void test__allc_file_open_close()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_file_t file = allc_file_new(allocator, "/tmp/test_file_open.txt");
    
    printf(">> Test opening file for writing:\n");
    allc_file_status_e status = allc_file_open(file, "w");
    ALLC_TEST_ANY("success", "%s", allc_file_status_string(status));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_file_is_open(file)));
    ALLC_TEST_ANY("w", "%s", allc_file_get_mode(file));
    
    printf(">> Test closing file:\n");
    status = allc_file_close(file);
    ALLC_TEST_ANY("success", "%s", allc_file_status_string(status));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_file_is_open(file)));
    
    printf(">> Test opening non-existent file for reading:\n");
    allc_file_t nonexistent = allc_file_new(allocator, "/tmp/nonexistent_file_12345.txt");
    status = allc_file_open(nonexistent, "r");
    ALLC_TEST_ANY("open failed", "%s", allc_file_status_string(status));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_file_is_open(nonexistent)));
    
    allc_file_delete(file);
    allc_file_delete(nonexistent);
    
    // Clean up test file
    unlink("/tmp/test_file_open.txt");
}

void test__allc_file_new_opened()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    
    printf(">> Test creating and opening file in one step:\n");
    allc_file_t file = allc_file_new_opened(allocator, "/tmp/test_new_opened.txt", "w");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(file != NULL));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_file_is_open(file)));
    ALLC_TEST_ANY("success", "%s", allc_file_status_string(allc_file_get_status(file)));
    
    allc_file_delete(file);
    
    printf(">> Test creating with invalid mode:\n");
    file = allc_file_new_opened(allocator, "/tmp/test_new_opened.txt", "invalid");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(file == NULL));
    
    // Clean up test file
    unlink("/tmp/test_new_opened.txt");
}

void test__allc_file_write_read()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    const char *test_data = "Hello, File System!";
    const char *filename = "/tmp/test_write_read.txt";
    
    printf(">> Test writing to file:\n");
    allc_file_t file = allc_file_new_opened(allocator, filename, "w");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(file != NULL));
    
    allc_size_t bytes_written = 0;
    allc_file_status_e status = allc_file_write(file, strlen(test_data), test_data, &bytes_written);
    ALLC_TEST_ANY("success", "%s", allc_file_status_string(status));
    ALLC_TEST_ANY("19", "%zu", bytes_written);
    
    allc_file_delete(file);
    
    printf(">> Test reading from file:\n");
    file = allc_file_new_opened(allocator, filename, "r");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(file != NULL));
    
    char buffer[100] = {0};
    allc_size_t bytes_read = 0;
    status = allc_file_read(file, sizeof(buffer) - 1, buffer, &bytes_read);
    ALLC_TEST_ANY("success", "%s", allc_file_status_string(status));
    ALLC_TEST_ANY("19", "%zu", bytes_read);
    ALLC_TEST_ANY("Hello, File System!", "%s", buffer);
    
    printf(">> Test reading beyond EOF:\n");
    status = allc_file_read(file, sizeof(buffer) - 1, buffer, &bytes_read);
    ALLC_TEST_ANY("end of file", "%s", allc_file_status_string(status));
    ALLC_TEST_ANY("0", "%zu", bytes_read);
    
    allc_file_delete(file);
    
    // Clean up test file
    unlink(filename);
}

void test__allc_file_seek_tell()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    const char *test_data = "0123456789ABCDEF";
    const char *filename = "/tmp/test_seek_tell.txt";
    
    // Create test file
    allc_file_t file = allc_file_new_opened(allocator, filename, "w");
    allc_file_write(file, strlen(test_data), test_data, NULL);
    allc_file_close(file);
    
    // Reopen for reading and testing
    allc_file_open(file, "r");
    
    printf(">> Test file position operations:\n");
    off_t position;
    allc_file_status_e status = allc_file_tell(file, &position);
    ALLC_TEST_ANY("success", "%s", allc_file_status_string(status));
    ALLC_TEST_ANY("0", "%ld", position);
    
    printf(">> Test seeking to middle of file:\n");
    status = allc_file_seek(file, 5, SEEK_SET);
    ALLC_TEST_ANY("success", "%s", allc_file_status_string(status));
    
    status = allc_file_tell(file, &position);
    ALLC_TEST_ANY("success", "%s", allc_file_status_string(status));
    ALLC_TEST_ANY("5", "%ld", position);
    
    // Read from new position
    char buffer[10] = {0};
    allc_size_t bytes_read;
    allc_file_read(file, 5, buffer, &bytes_read);
    ALLC_TEST_ANY("56789", "%s", buffer);
    
    allc_file_delete(file);
    
    // Clean up test file
    unlink(filename);
}

void test__allc_file_size()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    const char *test_data = "This is a test file for size measurement.";
    const char *filename = "/tmp/test_file_size.txt";
    
    // Create test file
    allc_file_t file = allc_file_new_opened(allocator, filename, "w");
    allc_file_write(file, strlen(test_data), test_data, NULL);
    
    printf(">> Test getting file size:\n");
    off_t size;
    allc_file_status_e status = allc_file_size(file, &size);
    ALLC_TEST_ANY("success", "%s", allc_file_status_string(status));
    ALLC_TEST_ANY("42", "%ld", size);
    
    allc_file_delete(file);
    
    // Clean up test file
    unlink(filename);
}

void test__allc_file_modes()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    const char *filename = "/tmp/test_file_modes.txt";
    
    printf(">> Test different file modes:\n");
    
    // Test write mode
    allc_file_t file = allc_file_new_opened(allocator, filename, "w");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(file != NULL));
    allc_file_write(file, 5, "test1", NULL);
    allc_file_delete(file);
    
    // Test append mode
    file = allc_file_new_opened(allocator, filename, "a");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(file != NULL));
    allc_file_write(file, 5, "test2", NULL);
    allc_file_delete(file);
    
    // Test read mode and verify append worked
    file = allc_file_new_opened(allocator, filename, "r");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(file != NULL));
    
    char buffer[20] = {0};
    allc_size_t bytes_read;
    allc_file_read(file, sizeof(buffer) - 1, buffer, &bytes_read);
    ALLC_TEST_ANY("test1test2", "%s", buffer);
    
    allc_file_delete(file);
    
    printf(">> Test binary modes:\n");
    file = allc_file_new(allocator, filename);
    allc_file_status_e status = allc_file_open(file, "wb");
    ALLC_TEST_ANY("success", "%s", allc_file_status_string(status));
    allc_file_delete(file);
    
    printf(">> Test invalid mode:\n");
    file = allc_file_new(allocator, filename);
    status = allc_file_open(file, "invalid_mode");
    ALLC_TEST_ANY("invalid mode", "%s", allc_file_status_string(status));
    allc_file_delete(file);
    
    // Clean up test file
    unlink(filename);
}

void test__allc_file_error_conditions()
{
    printf("\n[%s]\n", __func__);
    
    printf(">> Test null pointer errors:\n");
    ALLC_TEST_ANY("null pointer", "%s", allc_file_status_string(allc_file_get_status(NULL)));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_file_get_path(NULL) == NULL));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_file_is_open(NULL)));
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_file_t file = allc_file_new(allocator, "/tmp/test_errors.txt");
    
    printf(">> Test operations on closed file:\n");
    char buffer[10];
    allc_size_t bytes;
    ALLC_TEST_ANY("bad state", "%s", allc_file_status_string(allc_file_read(file, 10, buffer, &bytes)));
    ALLC_TEST_ANY("bad state", "%s", allc_file_status_string(allc_file_write(file, 5, "test", &bytes)));
    ALLC_TEST_ANY("bad state", "%s", allc_file_status_string(allc_file_close(file)));
    
    printf(">> Test double open:\n");
    allc_file_open(file, "w");
    ALLC_TEST_ANY("bad state", "%s", allc_file_status_string(allc_file_open(file, "r")));
    
    allc_file_delete(file);
    
    // Clean up test file
    unlink("/tmp/test_errors.txt");
}

void test__allc_file_comprehensive()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    const char *filename = "/tmp/test_comprehensive.txt";
    
    printf(">> Testing complete file lifecycle:\n");
    
    bool success = true;
    
    // Create and write multiple test files
    for (int i = 0; success && i < 5; i++) {
        char test_filename[100];
        snprintf(test_filename, sizeof(test_filename), "%s_%d", filename, i);
        
        allc_file_t file = allc_file_new_opened(allocator, test_filename, "w");
        success &= (file != NULL);
        
        if (success) {
            char test_data[100];
            snprintf(test_data, sizeof(test_data), "Test data for file %d", i);
            
            allc_size_t bytes_written;
            allc_file_status_e status = allc_file_write(file, strlen(test_data), test_data, &bytes_written);
            success &= (status == ALLC_FILE_SUCCESS);
            success &= (bytes_written == strlen(test_data));
            
            allc_file_delete(file);
        }
        
        if (success) {
            // Read back and verify
            allc_file_t read_file = allc_file_new_opened(allocator, test_filename, "r");
            success &= (read_file != NULL);
            
            if (success) {
                char buffer[100] = {0};
                allc_size_t bytes_read;
                allc_file_status_e status = allc_file_read(read_file, sizeof(buffer) - 1, buffer, &bytes_read);
                success &= (status == ALLC_FILE_SUCCESS);
                
                char expected[100];
                snprintf(expected, sizeof(expected), "Test data for file %d", i);
                success &= allc_cstr_is_equal(buffer, expected);
                
                allc_file_delete(read_file);
            }
        }
        
        // Clean up
        unlink(test_filename);
    }
    
    ALLC_TEST_BOOL(true, success);
    printf(">> All comprehensive tests completed successfully\n");
}

int main()
{
    printf("=== File Module Tests ===\n");
    
    test__allc_file_creation_deletion();
    test__allc_file_utility_functions();
    test__allc_file_open_close();
    test__allc_file_new_opened();
    test__allc_file_write_read();
    test__allc_file_seek_tell();
    test__allc_file_size();
    test__allc_file_modes();
    test__allc_file_error_conditions();
    test__allc_file_comprehensive();
    
    printf("\n=== All File Tests Completed ===\n");
    return 0;
}
