#include <check.h>
#include <string.h>

#include "io/buffer-reader.h"

START_TEST(test_buffer_reader_missing_file)
{
    char errbuf[256];
    buffer_reader_t *reader = buffer_reader_open("/nonexistent/csvi-test-file.csv", errbuf, sizeof(errbuf));
    ck_assert_ptr_eq(reader, NULL);
    ck_assert(strstr(errbuf, "nonexistent") != NULL || strstr(errbuf, "could not open") != NULL);
}
END_TEST

Suite *buffer_reader_suite(void)
{
    Suite *s = suite_create("buffer_reader");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_buffer_reader_missing_file);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = buffer_reader_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? 0 : 1;
}
