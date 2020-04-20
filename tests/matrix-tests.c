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

/************************************************
 * |                40                    |
 * ┌──────────────────────────────────────┐ ---
 * │ cell 0,0 | cell 0,1 | cell 0,2 |     │
 * │ cell 1,0 | cell 1,1 | cell 1,2 |     │
 * │ cell 2,0 | cell 2,1 | cell 2,2 |     │
 * │ cell 3,0 | cell 3,1 | cell 3,2 |     │
 * │                                      │ 10
 * │                                      │
 * │                                      │
 * │                                      │
 * │                                      │
 * └──────────────────────────────────────┘ ---
 **/
START_TEST(test_matrix_expansion_lower)
{
    matrix_properties_t m_properties = {.cell_padding_top = 0,
                                        .cell_padding_right = 2,
                                        .cell_padding_bottom = 0,
                                        .cell_padding_left = 1,
                                        .margin_top = 1,
                                        .margin_right = 1,
                                        .margin_bottom = 2,
                                        .margin_left = 1};
    matrix_config_t *config = matrix_config_initialize(3, 4);
    screen_config_t windows_configuration = {.width = 40, .height = 10};
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

    matrix_config_get_most_expanded(&windows_configuration, &m_properties, contents, 3, 4, &presentation_configuration);
    ck_assert_int_eq(3, presentation_configuration.width);
    ck_assert_int_eq(4, presentation_configuration.height);
    csv_token_dispose(contents);
    matrix_config_dispose(config);
}
END_TEST

/************************************************
 * |                40                    |
 * ┌──────────────────────────────────────┐ ---
 * │ cell 0,0 | cell 0,1 | cell 0,2 |     │
 * │ cell 1,0 | cell 1,1 | cell 1,2 |     │
 * │ cell 2,0 | cell 2,1 | cell 2,2 |     │ 5
 * │ cell 3,0 | cell 3,1 | cell 3,2 |     │
 * │ cell 4,0 | cell 4,1 | cell 4,2 |     │
 * └──────────────────────────────────────┘ ---
 **/
START_TEST(test_matrix_expansion_bigger)
{
    matrix_properties_t m_properties = {.cell_padding_top = 0,
                                        .cell_padding_right = 2,
                                        .cell_padding_bottom = 0,
                                        .cell_padding_left = 1,
                                        .margin_top = 1,
                                        .margin_right = 1,
                                        .margin_bottom = 2,
                                        .margin_left = 1};
    matrix_config_t *config = matrix_config_initialize(4, 6);
    screen_config_t windows_configuration = {.width = 40, .height = 8};
    screen_config_t presentation_configuration = {.width = 1, .height = 1};

    csv_token *contents = mock_token(0, 0, "cell 0,0", NULL);
    mock_token(0, 1, "cell 0,1", contents);
    mock_token(0, 2, "cell 0,2", contents);
    mock_token(0, 3, "cell 0,3", contents);
    mock_token(1, 0, "cell 1,0", contents);
    mock_token(1, 1, "cell 1,1", contents);
    mock_token(1, 2, "cell 1,2", contents);
    mock_token(1, 3, "cell 1,3", contents);
    mock_token(2, 0, "cell 2,0", contents);
    mock_token(2, 1, "cell 2,1", contents);
    mock_token(2, 2, "cell 2,2", contents);
    mock_token(2, 3, "cell 2,3", contents);
    mock_token(3, 0, "cell 3,0", contents);
    mock_token(3, 1, "cell 3,1", contents);
    mock_token(3, 2, "cell 3,2", contents);
    mock_token(3, 3, "cell 3,3", contents);
    mock_token(4, 0, "cell 4,0", contents);
    mock_token(4, 1, "cell 4,1", contents);
    mock_token(4, 2, "cell 4,2", contents);
    mock_token(4, 3, "cell 4,3", contents);
    mock_token(5, 0, "cell 5,0", contents);
    mock_token(5, 1, "cell 5,1", contents);
    mock_token(5, 2, "cell 5,2", contents);
    mock_token(5, 3, "cell 5,3", contents);

    matrix_config_get_most_expanded(&windows_configuration, &m_properties, contents, 4, 6, &presentation_configuration);
    fflush(stdout);

    ck_assert_int_eq(3, presentation_configuration.width);
    ck_assert_int_eq(5, presentation_configuration.height);
    csv_token_dispose(contents);
    matrix_config_dispose(config);
}
END_TEST

Suite *matrix_test_suite(void)
{
    Suite *s = suite_create("Matrix");
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_matrix_initialization);
    tcase_add_test(tc_core, test_matrix_expansion_lower);
    tcase_add_test(tc_core, test_matrix_expansion_bigger);

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