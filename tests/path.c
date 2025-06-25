#define ALLC_PATH__IMPL

#include "../dev/path.h"
#include "../dev/macro.h"

void test__new(allc_allocator_t allocator)
{
  Path path;

  path = allc_path_new_from_cwd(allocator);
  ALLC_TEST_ANY("/home/kaarmu/Projects/C/allc", "%s", path->buf);
  allc_path_delete(path);

  path = allc_path_new_from_home(allocator);
  ALLC_TEST_ANY("/home/kaarmu", "%s", path->buf);
  allc_path_delete(path);
}

void test__set(allc_allocator_t allocator)
{
  Path path;

  path = allc_path_new(allocator);
  allc_path_set_cstr(&path, "~/Projects");
  ALLC_TEST_ANY("~/Projects", "%s", path->buf);
  allc_path_set_cstr(&path, "~/Projects/allc");
  ALLC_TEST_ANY("~/Projects/allc", "%s", path->buf);
  allc_path_delete(path);

  path = allc_path_new_from_home(allocator);
  allc_path_set_cstr(&path, "~");
  ALLC_TEST_ANY("~", "%s", path->buf);
  allc_path_delete(path);
}

void test__append(allc_allocator_t allocator)
{
  Path path;

  path = allc_path_new(allocator);
  allc_path_set_cstr(&path, "~/Projects");
  ALLC_TEST_ANY("~/Projects", "%s", path->buf);
  allc_path_append_part_cstr_1(&path, "allc");
  ALLC_TEST_ANY("~/Projects/allc", "%s", path->buf);
  allc_path_append_part_cstr_n(&path, 2, ".cache", "bin");
  ALLC_TEST_ANY("~/Projects/allc/.cache/bin", "%s", path->buf);
  allc_path_delete(path);
}

void test__allc_path_to_canonical_path(allc_allocator_t allocator)
{
  printf("\n[%s]\n", __func__);

  Path path = allc_path_new(allocator);

  // Test 1: Consecutive slashes
  allc_path_set_cstr(&path, "/home//user//projects/");
  path = allc_path_to_canonical_path(path);
  ALLC_TEST_ANY("/home/user/projects", "%s", path->buf);

  // Test 2: Single dot references
  allc_path_set_cstr(&path, "./home/user/./projects/.");
  path = allc_path_to_canonical_path(path);
  ALLC_TEST_ANY("home/user/projects", "%s", path->buf);

  // Test 3: Doprintf("/.: %zu\n", i);uble dot references
  allc_path_set_cstr(&path, "/home/user/../projects");
  path = allc_path_to_canonical_path(path);
  ALLC_TEST_ANY("/home/projects", "%s", path->buf);

  // Test 4: Mixed references
  allc_path_set_cstr(&path, "/home/./user//projects/../bin");
  path = allc_path_to_canonical_path(path);
  ALLC_TEST_ANY("/home/user/bin", "%s", path->buf);

  // Test 5: Absolute path with no changes
  allc_path_set_cstr(&path, "/home/user/projects");
  path = allc_path_to_canonical_path(path);
  ALLC_TEST_ANY("/home/user/projects", "%s", path->buf);

  // Test 6: Relative path with no changes
  allc_path_set_cstr(&path, "home/user/projects");
  path = allc_path_to_canonical_path(path);
  ALLC_TEST_ANY("home/user/projects", "%s", path->buf);

  // Test 7: Edge case - empty path
  allc_path_set_cstr(&path, "");
  path = allc_path_to_canonical_path(path);
  ALLC_TEST_ANY("", "%s", path->buf);

  // Test 8: Edge case - single dot
  allc_path_set_cstr(&path, ".");
  path = allc_path_to_canonical_path(path);
  ALLC_TEST_ANY(".", "%s", path->buf);

  // Test 9: Edge case - double dot
  allc_path_set_cstr(&path, "..");
  path = allc_path_to_canonical_path(path);
  ALLC_TEST_ANY("..", "%s", path->buf);

  allc_path_delete(path);
}

void test__allc_path_count_parts(allc_allocator_t allocator)
{
  printf("\n[%s]\n", __func__);

  Path path = allc_path_new(allocator);

  // Test 1: Absolute path
  allc_path_set_cstr(&path, "/home/user/projects");
  ALLC_TEST_ANY("3", "%zu", allc_path_count_parts(&path));

  // Test 2: Relative path
  allc_path_set_cstr(&path, "home/user/projects");
  ALLC_TEST_ANY("3", "%zu", allc_path_count_parts(&path));

  // Test 3: Consecutive slashes
  allc_path_set_cstr(&path, "/home//user//projects/");
  ALLC_TEST_ANY("3", "%zu", allc_path_count_parts(&path)); // Even consecutive slashes are counted

  // Test 4: Path without slashes
  allc_path_set_cstr(&path, "projects");
  ALLC_TEST_ANY("1", "%zu", allc_path_count_parts(&path));

  // Test 5: Edge case - empty path
  allc_path_set_cstr(&path, "");
  ALLC_TEST_ANY("0", "%zu", allc_path_count_parts(&path));

  // Test 6: Single slash
  allc_path_set_cstr(&path, "/");
  ALLC_TEST_ANY("0", "%zu", allc_path_count_parts(&path));

  // Test 7: Multiple slashes only
  allc_path_set_cstr(&path, "////");
  ALLC_TEST_ANY("0", "%zu", allc_path_count_parts(&path));

  allc_path_delete(path);
}

void test__allc_path_to_dirname_path(allc_allocator_t allocator)
{
  printf("\n[%s]\n", __func__);

  Path path = allc_path_new(allocator);

  // Test 1: Absolute path
  allc_path_set_cstr(&path, "/home/user/projects/file.txt");
  path = allc_path_to_dirname_path(path);
  ALLC_TEST_ANY("/home/user/projects", "%s", path->buf);

  // Test 2: Relative path
  allc_path_set_cstr(&path, "user/projects/file.txt");
  path = allc_path_to_dirname_path(path);
  ALLC_TEST_ANY("user/projects", "%s", path->buf);

  // Test 3: Path without directories (just filename)
  allc_path_set_cstr(&path, "file.txt");
  path = allc_path_to_dirname_path(path);
  ALLC_TEST_ANY(".", "%s", path->buf); // Typically, dirname returns "." for such cases

  // Test 4: Edge case - empty path
  allc_path_set_cstr(&path, "");
  path = allc_path_to_dirname_path(path);
  ALLC_TEST_ANY(".", "%s", path->buf); // Assuming it returns "." for empty paths

  // Test 5: Path that is just a directory
  allc_path_set_cstr(&path, "/home/user/projects/");
  path = allc_path_to_dirname_path(path);
  ALLC_TEST_ANY("/home/user/projects", "%s", path->buf);

  // Test 6: Single directory without slash
  allc_path_set_cstr(&path, "projects");
  path = allc_path_to_dirname_path(path);
  ALLC_TEST_ANY(".", "%s", path->buf);

  // Test 7: Root directory
  allc_path_set_cstr(&path, "/");
  path = allc_path_to_dirname_path(path);
  ALLC_TEST_ANY("/", "%s", path->buf);

  allc_path_delete(path);
}

void test__allc_path_extend_path(allc_allocator_t allocator)
{
  printf("\n[%s]\n", __func__);

  Path p1 = allc_path_new(allocator), p2 = allc_path_new(allocator);

  // Test 1: Extend absolute path with relative path
  allc_path_set_cstr(&p1, "/home/user");
  allc_path_set_cstr(&p2, "projects/file.txt");
  allc_path_extend_path(&p1, &p2);
  ALLC_TEST_ANY("/home/user/projects/file.txt", "%s", p1->buf);

  // Test 2: Extend relative path with relative path
  allc_path_set_cstr(&p1, "user");
  allc_path_set_cstr(&p2, "projects/file.txt");
  allc_path_extend_path(&p1, &p2);
  ALLC_TEST_ANY("user/projects/file.txt", "%s", p1->buf);

  // Test 3: Do nothing when it is impossible to extend
  allc_path_set_cstr(&p1, "/home/user");
  allc_path_set_cstr(&p2, "/projects/file.txt");
  allc_path_extend_path(&p1, &p2);
  ALLC_TEST_ANY("/home/user", "%s", p1->buf);

  // Test 4: Edge case - extend empty path with path
  allc_path_set_cstr(&p1, "");
  allc_path_set_cstr(&p2, "projects/file.txt");
  allc_path_extend_path(&p1, &p2);
  ALLC_TEST_ANY("projects/file.txt", "%s", p1->buf);

  // Test 5: Edge case - extend path with empty path
  allc_path_set_cstr(&p1, "/home/user");
  allc_path_set_cstr(&p2, "");
  allc_path_extend_path(&p1, &p2);
  ALLC_TEST_ANY("/home/user", "%s", p1->buf);

  allc_path_delete(p1);
  allc_path_delete(p2);
}

void test__allc_path_to_basename_strbuf(allc_allocator_t allocator)
{
  printf("\n[%s]\n", __func__);

  Path p = allc_path_new(allocator);

  allc_path_set_cstr(&p, "/home/user/file.txt");
  StrBuf base = allc_path_to_basename_strbuf(p);
  ALLC_TEST_ANY("file.txt", "%s", base->buf);
  allc_strbuf_delete(base);

  allc_path_set_cstr(&p, "/home/user/");
  base = allc_path_to_basename_strbuf(p);
  ALLC_TEST_ANY("user", "%s", base->buf);
  allc_strbuf_delete(base);

  allc_path_set_cstr(&p, "plainname");
  base = allc_path_to_basename_strbuf(p);
  ALLC_TEST_ANY("plainname", "%s", base->buf);
  allc_strbuf_delete(base);

  allc_path_delete(p);
}

void test__allc_path_to_suffix_strbuf(allc_allocator_t allocator)
{
  printf("\n[%s]\n", __func__);

  Path p = allc_path_new(allocator);

  allc_path_set_cstr(&p, "/tmp/archive.tar.gz");
  StrBuf suf = allc_path_to_suffix_strbuf(p);
  ALLC_TEST_ANY("gz", "%s", suf->buf);
  allc_strbuf_delete(suf);

  allc_path_set_cstr(&p, "noext");
  suf = allc_path_to_suffix_strbuf(p);
  ALLC_TEST_ANY("", "%s", suf->buf);
  allc_strbuf_delete(suf);

  allc_path_set_cstr(&p, ".hidden");
  suf = allc_path_to_suffix_strbuf(p);
  ALLC_TEST_ANY("", "%s", suf->buf);
  allc_strbuf_delete(suf);

  allc_path_delete(p);
}

void test__allc_path_to_absolute_path(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    Path path = allc_path_new(allocator);

    // Absolute path remains unchanged
    allc_path_set_cstr(&path, "/home/user/projects");
    path = allc_path_to_absolute_path(path);
    ALLC_TEST_ANY("/home/user/projects", "%s", path->buf);

    // Relative path becomes absolute
    allc_path_set_cstr(&path, "user/projects");
    path = allc_path_to_absolute_path(path);
    ALLC_TEST_ANY("/current/dir/user/projects", "%s", path->buf);


    // Edge case - empty path
    // Assuming it returns the base path or current directory.
    allc_path_set_cstr(&path, "");
    path = allc_path_to_absolute_path(path);
    ALLC_TEST_ANY("/current/dir", "%s", path->buf);

    allc_path_delete(path);
}


int main()
{
  allc_allocator_t allocator = allc_allocator_c();

  test__new(allocator);
  test__set(allocator);
  test__append(allocator);
  test__allc_path_to_canonical_path(allocator);
  test__allc_path_count_parts(allocator);
  test__allc_path_to_dirname_path(allocator);
  test__allc_path_extend_path(allocator);
  test__allc_path_to_basename_strbuf(allocator);
  test__allc_path_to_suffix_strbuf(allocator);
  test__allc_path_to_absolute_path(allocator);
}
