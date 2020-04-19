#include <check.h>
#include <stdlib.h>
#include <stdio.h>

#include "matrix-config.h"

START_TEST(test_matrix_initialization)
{
    matrix_config_t *config = matrix_config_initialize(10, 10);
    for (size_t index = 0; index < 10; ++index)
    {
        ck_assert_int_eq(0, config->column_width[index]);
        ck_assert_int_eq(0, config->line_height[index]);
    }
    matrix_config_dispose(config);
}
END_TEST

Suite *matrix_test_suite(void)
{
    Suite *s = suite_create("Matrix");
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_matrix_initialization);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = matrix_test_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_log(sr, "matrix-test-output.log");
    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}