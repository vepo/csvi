#include <check.h>
#include <stdlib.h>
#include <stdio.h>

#include "matrix-config.h"
#include "mock-token.h"

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

START_TEST(test_matrix_expansion)
{
    matrix_config_t *config = matrix_config_initialize(10, 10);
    screen_config_t windows_configuration = {.width = 120, .height = 80};
    screen_config_t presentation_configuration = {.width = 1, .height = 1};

    csv_token *contents = mock_token(0, 0, "cell 0,0", NULL);
    mock_token(0, 1, "cell 0,1", contents);
    mock_token(0, 2, "cell 0,2", contents);
    mock_token(1, 0, "cell 1,0", contents);
    mock_token(1, 1, "cell 1,1", contents);
    mock_token(1, 2, "cell 1,2", contents);
    mock_token(2, 0, "cell 2,0", contents);
    mock_token(2, 1, "cell 2,1", contents);
    mock_token(2, 2, "cell 2,2", contents);
    mock_token(3, 0, "cell 3,0", contents);
    mock_token(3, 1, "cell 3,1", contents);
    mock_token(3, 2, "cell 3,2", contents);

    matrix_config_get_most_expanded(&windows_configuration, contents, 4, 3, &presentation_configuration);
    csv_token_dispose(contents);
    matrix_config_dispose(config);
}
END_TEST

Suite *matrix_test_suite(void)
{
    Suite *s = suite_create("Matrix");
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_matrix_initialization);
    tcase_add_test(tc_core, test_matrix_expansion);

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