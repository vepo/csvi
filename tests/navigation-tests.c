#include <check.h>
#include <stdlib.h>
#include <stdio.h>

#include "matrix-config.h"
#include "mock-token.h"


Suite *matrix_test_suite(void)
{
    Suite *s = suite_create("Navigation");
    TCase *tc_core = tcase_create("Actions");


    suite_add_tcase(s, tc_core);
    return s;
}


int main(void)
{
    int number_failed;
    Suite *s = matrix_test_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_log(sr, "navigation-output.log");
    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}