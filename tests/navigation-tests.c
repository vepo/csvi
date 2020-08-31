#include <check.h>
#include <stdlib.h>
#include <stdio.h>

#include "navigation.h"

START_TEST(test_navigation_up)
{
    // nothing
    coordinates_t top_cell = {
        .x = 0,
        .y = 0};
    coordinates_t cursor_position = {
        .x = 0,
        .y = 0};
    screen_size_t screen_size = {
        .width = 10,
        .height = 10};

    ck_assert_int_eq(BEEP, navigate_up(&top_cell, &cursor_position, &screen_size, 10, 10));
    ck_assert_int_eq(0, cursor_position.y);
    ck_assert_int_eq(0, top_cell.y);

    cursor_position.y = 1;
    ck_assert_int_eq(CURSOR_UPDATED, navigate_up(&top_cell, &cursor_position, &screen_size, 10, 10));
    ck_assert_int_eq(0, cursor_position.y);
    ck_assert_int_eq(0, top_cell.y);

    cursor_position.y = 1;
    top_cell.y = 1;
    ck_assert_int_eq(CURSOR_UPDATED, navigate_up(&top_cell, &cursor_position, &screen_size, 10, 10));
    ck_assert_int_eq(0, cursor_position.y);
    ck_assert_int_eq(0, top_cell.y);
}
END_TEST

Suite *matrix_test_suite(void)
{
    Suite *s = suite_create("Navigation");
    TCase *tc_core = tcase_create("Actions");

    tcase_add_test(tc_core, test_navigation_up);

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