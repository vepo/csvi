#include <check.h>
#include <stdlib.h>

#include "csv-reader.h"

START_TEST(test_csv_reader_simple_csv)
{
    csv_contents *open_file = csv_reader_read_file("simple.csv");
    ck_assert_ptr_ne(open_file, NULL);
}
END_TEST

Suite *test_suite(void)
{
    Suite *s = suite_create("test");
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_csv_reader_simple_csv);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = test_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}