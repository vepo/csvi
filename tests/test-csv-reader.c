#include <check.h>
#include <stdlib.h>
#include <unistd.h>

#include "csv-reader.h"
#include "test-resources.h"

START_TEST(test_csv_reader_open)
{
    csv_metadata_t *metadata = csv_reader_open(test_resource_get("SIMPLE_CONTENTS"));
    ck_assert_ptr_ne(NULL, metadata);
    ck_assert_ptr_ne(NULL, metadata->file_handler);
    ck_assert_int_eq(CSV_STATUS_READ, metadata->status);
    ck_assert_int_eq(true, metadata->can_write);
    ck_assert_int_eq(true, metadata->can_read);
    //ck_assert_int_eq(3, metadata->columns);
    //ck_assert_int_eq(4, metadata->lines);
}
END_TEST

START_TEST(test_csv_reader_open_file_name)
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("path: %s\n", cwd);
    fflush(stdout);
#if defined(WIN32) || defined(_WIN32)
    char *path = ".\\test.csv";
#else
    char *path = "./test.csv";
#endif
    csv_metadata_t *metadata = csv_reader_open(path);
    ck_assert_ptr_ne(NULL, metadata);
    ck_assert_str_eq(cwd, metadata->directory);
    ck_assert_str_eq("test.csv", metadata->filename);
    ck_assert_int_eq(CSV_STATUS_FILE_NOT_FOUND, metadata->status);
    ck_assert_int_eq(0, metadata->columns);
    ck_assert_int_eq(0, metadata->lines);
}
END_TEST

Suite *matrix_test_suite(void)
{
    Suite *s = suite_create("CSV Reader");
    TCase *tc_core = tcase_create("File Handler");

    tcase_add_test(tc_core, test_csv_reader_open);
    tcase_add_test(tc_core, test_csv_reader_open_file_name);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = matrix_test_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_log(sr, "test-output-csv-reader.log");
    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}