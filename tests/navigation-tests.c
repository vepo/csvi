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

    ck_assert_int_eq(BEEP, navigate_up(&top_cell, &cursor_position));
    ck_assert_int_eq(0, cursor_position.y);
    ck_assert_int_eq(0, top_cell.y);

    cursor_position.y = 1;
    ck_assert_int_eq(CURSOR_UPDATED, navigate_up(&top_cell, &cursor_position));
    ck_assert_int_eq(0, cursor_position.y);
    ck_assert_int_eq(0, top_cell.y);

    cursor_position.y = 1;
    top_cell.y = 1;
    ck_assert_int_eq(CURSOR_UPDATED, navigate_up(&top_cell, &cursor_position));
    ck_assert_int_eq(0, cursor_position.y);
    ck_assert_int_eq(0, top_cell.y);
}
END_TEST

START_TEST(test_navigation_down)
{
    // nothing
    coordinates_t top_cell = {
        .x = 0,
        .y = 0};
    coordinates_t cursor_position = {
        .x = 0,
        .y = 9};
    screen_size_t screen_size = {
        .width = 10,
        .height = 10};

    ck_assert_int_eq(BEEP, navigate_down(&top_cell, &cursor_position, &screen_size, 10));
    ck_assert_int_eq(9, cursor_position.y);
    ck_assert_int_eq(0, top_cell.y);

    ck_assert_int_eq(CURSOR_UPDATED, navigate_down(&top_cell, &cursor_position, &screen_size, 20));
    ck_assert_int_eq(10, cursor_position.y);
    ck_assert_int_eq(1, top_cell.y);

    cursor_position.y = 5;
    top_cell.y = 0;
    ck_assert_int_eq(CURSOR_UPDATED, navigate_down(&top_cell, &cursor_position, &screen_size, 10));
    ck_assert_int_eq(6, cursor_position.y);
    ck_assert_int_eq(0, top_cell.y);
}
END_TEST

START_TEST(test_navigation_left)
{
    // nothing
    coordinates_t top_cell = {
        .x = 0,
        .y = 0};
    coordinates_t cursor_position = {
        .x = 0,
        .y = 0};

    ck_assert_int_eq(BEEP, navigate_left(&top_cell, &cursor_position));
    ck_assert_int_eq(0, cursor_position.x);
    ck_assert_int_eq(0, top_cell.x);

    cursor_position.x = 1;
    ck_assert_int_eq(CURSOR_UPDATED, navigate_left(&top_cell, &cursor_position));
    ck_assert_int_eq(0, cursor_position.x);
    ck_assert_int_eq(0, top_cell.x);

    cursor_position.x = 1;
    top_cell.x = 1;
    ck_assert_int_eq(CURSOR_UPDATED, navigate_left(&top_cell, &cursor_position));
    ck_assert_int_eq(0, cursor_position.x);
    ck_assert_int_eq(0, top_cell.x);
}
END_TEST

START_TEST(test_navigation_right)
{
    // nothing
    coordinates_t top_cell = {
        .x = 0,
        .y = 0};
    coordinates_t cursor_position = {
        .x = 9,
        .y = 0};
    screen_size_t screen_size = {
        .width = 10,
        .height = 10};

    ck_assert_int_eq(BEEP, navigate_right(&top_cell, &cursor_position, &screen_size, 10));
    ck_assert_int_eq(9, cursor_position.x);
    ck_assert_int_eq(0, top_cell.x);

    ck_assert_int_eq(CURSOR_UPDATED, navigate_right(&top_cell, &cursor_position, &screen_size, 20));
    ck_assert_int_eq(10, cursor_position.x);
    ck_assert_int_eq(1, top_cell.x);

    cursor_position.x = 19;
    top_cell.x = 10;
    ck_assert_int_eq(BEEP, navigate_right(&top_cell, &cursor_position, &screen_size, 20));
    ck_assert_int_eq(19, cursor_position.x);
    ck_assert_int_eq(10, top_cell.x);
}
END_TEST

START_TEST(test_navigation_page_up)
{
    // nothing
    coordinates_t top_cell = {
        .x = 3,
        .y = 0};
    coordinates_t cursor_position = {
        .x = 3,
        .y = 0};
    screen_size_t screen_size = {
        .width = 10,
        .height = 10};

    ck_assert_int_eq(BEEP, navigate_page_up(&top_cell, &cursor_position, &screen_size));
    ck_assert_int_eq(0, cursor_position.y);
    ck_assert_int_eq(0, top_cell.y);

    cursor_position.y = 9;
    ck_assert_int_eq(CURSOR_UPDATED, navigate_page_up(&top_cell, &cursor_position, &screen_size));
    ck_assert_int_eq(0, cursor_position.y);
    ck_assert_int_eq(0, top_cell.y);

    cursor_position.y = 10;
    top_cell.y = 1;
    ck_assert_int_eq(CURSOR_UPDATED, navigate_page_up(&top_cell, &cursor_position, &screen_size));
    ck_assert_int_eq(0, cursor_position.y);
    ck_assert_int_eq(0, top_cell.y);

    cursor_position.y = 11;
    top_cell.y = 11;
    ck_assert_int_eq(CURSOR_UPDATED, navigate_page_up(&top_cell, &cursor_position, &screen_size));
    ck_assert_int_eq(1, cursor_position.y);
    ck_assert_int_eq(1, top_cell.y);
}
END_TEST

START_TEST(test_navigation_page_down)
{
    // nothing
    coordinates_t top_cell = {
        .x = 0,
        .y = 0};
    coordinates_t cursor_position = {
        .x = 0,
        .y = 9};
    screen_size_t screen_size = {
        .width = 10,
        .height = 10};

    ck_assert_int_eq(BEEP, navigate_page_down(&top_cell, &cursor_position, &screen_size, 10));
    ck_assert_int_eq(9, cursor_position.y);
    ck_assert_int_eq(0, top_cell.y);

    ck_assert_int_eq(CURSOR_UPDATED, navigate_page_down(&top_cell, &cursor_position, &screen_size, 20));
    ck_assert_int_eq(19, cursor_position.y);
    ck_assert_int_eq(10, top_cell.y);

    cursor_position.y = 9;
    top_cell.y = 0;
    ck_assert_int_eq(CURSOR_UPDATED, navigate_page_down(&top_cell, &cursor_position, &screen_size, 11));
    ck_assert_int_eq(10, cursor_position.y);
    ck_assert_int_eq(10, top_cell.y);
}
END_TEST

START_TEST(test_navigation_home)
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

    ck_assert_int_eq(BEEP, navigate_page_previous(&top_cell, &cursor_position, &screen_size));
    ck_assert_int_eq(0, cursor_position.x);
    ck_assert_int_eq(0, top_cell.x);

    cursor_position.x = 10;
    top_cell.x = 10;
    ck_assert_int_eq(CURSOR_UPDATED, navigate_page_previous(&top_cell, &cursor_position, &screen_size));
    ck_assert_int_eq(0, cursor_position.x);
    ck_assert_int_eq(0, top_cell.x);

    cursor_position.x = 3;
    top_cell.x = 0;
    ck_assert_int_eq(CURSOR_UPDATED, navigate_page_previous(&top_cell, &cursor_position, &screen_size));
    ck_assert_int_eq(0, cursor_position.x);
    ck_assert_int_eq(0, top_cell.x);
}
END_TEST

Suite *matrix_test_suite(void)
{
    Suite *s = suite_create("Navigation");
    TCase *tc_core = tcase_create("Actions");

    tcase_add_test(tc_core, test_navigation_up);
    tcase_add_test(tc_core, test_navigation_down);
    tcase_add_test(tc_core, test_navigation_left);
    tcase_add_test(tc_core, test_navigation_right);
    tcase_add_test(tc_core, test_navigation_page_up);
    tcase_add_test(tc_core, test_navigation_page_down);
    tcase_add_test(tc_core, test_navigation_home);

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