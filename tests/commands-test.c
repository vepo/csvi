#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "commands.h"

int test_commands_counter_exit = 0;
int test_commands_counter_go_to_column = 0;
int test_commands_counter_go_to_line = 0;
int test_commands_counter_go_to_cell = 0;
int test_commands_counter_go_to_first_line = 0;
int test_commands_counter_go_to_last_line = 0;
int test_commands_counter_go_to_first_column = 0;
int test_commands_counter_go_to_last_column = 0;
int test_commands_counter_show_error = 0;
int test_commands_last_exit_code = -1;
int test_commands_last_column = -1;
int test_commands_last_line = -1;
char *test_commands_last_shown_error_message = NULL;

void test_commands_mock_reset()
{
    test_commands_counter_exit = 0;
    test_commands_counter_go_to_column = 0;
    test_commands_counter_go_to_line = 0;
    test_commands_counter_go_to_cell = 0;
    test_commands_counter_go_to_first_line = 0;
    test_commands_counter_go_to_last_line = 0;
    test_commands_counter_go_to_first_column = 0;
    test_commands_counter_go_to_last_column = 0;
    test_commands_counter_show_error = 0;
    test_commands_last_exit_code = -1;
    test_commands_last_column = -1;
    test_commands_last_line = -1;
    if (test_commands_last_shown_error_message)
    {
        free(test_commands_last_shown_error_message);
    }
    test_commands_last_shown_error_message = NULL;
}

void test_commands_mocked_exit(int exit_code)
{
    test_commands_counter_exit++;
    test_commands_last_exit_code = exit_code;
}

void test_commands_mock_go_to_column(size_t colunm)
{
    test_commands_counter_go_to_column++;
    test_commands_last_column = colunm;
}

void test_commands_mock_go_to_line(size_t line)
{
    test_commands_counter_go_to_line++;
    test_commands_last_line = line;
}

void test_commands_mock_go_to_first_line()
{
    test_commands_counter_go_to_first_line++;
}

void test_commands_mock_go_to_last_line()
{
    test_commands_counter_go_to_last_line++;
}

void test_commands_mock_go_to_first_column()
{
    test_commands_counter_go_to_first_column++;
}

void test_commands_mock_go_to_last_column()
{
    test_commands_counter_go_to_last_column++;
}

void test_commands_mock_go_to_cell(size_t column, size_t line)
{
    test_commands_counter_go_to_cell++;
    test_commands_last_line = line;
    test_commands_last_column = column;
}

void test_commands_mock_show_error(char *error_message)
{
    test_commands_counter_show_error++;
    size_t message_length = strlen(error_message) + 1;
    test_commands_last_shown_error_message = (char *)malloc((message_length) * sizeof(char));
    strncpy(test_commands_last_shown_error_message, error_message, message_length);
}

START_TEST(commands_exit)
{
    test_commands_mock_reset();
    command_executors_t executors = {
        .exit = &test_commands_mocked_exit,
        .go_to_line = &test_commands_mock_go_to_line,
        .go_to_column = &test_commands_mock_go_to_column,
        .go_to_cell = &test_commands_mock_go_to_cell,
        .go_to_first_line = &test_commands_mock_go_to_first_line,
        .go_to_last_line = &test_commands_mock_go_to_last_line,
        .go_to_first_column = &test_commands_mock_go_to_first_column,
        .go_to_last_column = &test_commands_mock_go_to_last_column,
        .show_error = &test_commands_mock_show_error};
    commands_init(&executors);
    commands_execute(":q");
    ck_assert_uint_eq(test_commands_counter_exit, 1);
    ck_assert_uint_eq(test_commands_last_exit_code, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_column, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_cell, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_first_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_last_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_first_column, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_last_column, 0);
    ck_assert_uint_eq(test_commands_counter_show_error, 0);
    commands_shutdown();
}
END_TEST

START_TEST(commands_go_to_line)
{
    test_commands_mock_reset();
    command_executors_t executors = {
        .exit = &test_commands_mocked_exit,
        .go_to_line = &test_commands_mock_go_to_line,
        .go_to_column = &test_commands_mock_go_to_column,
        .go_to_cell = &test_commands_mock_go_to_cell,
        .go_to_first_line = &test_commands_mock_go_to_first_line,
        .go_to_last_line = &test_commands_mock_go_to_last_line,
        .go_to_first_column = &test_commands_mock_go_to_first_column,
        .go_to_last_column = &test_commands_mock_go_to_last_column,
        .show_error = &test_commands_mock_show_error};
    commands_init(&executors);
    commands_execute(":57");
    ck_assert_uint_eq(test_commands_counter_go_to_line, 1);
    ck_assert_uint_eq(test_commands_last_line, 56);
    ck_assert_uint_eq(test_commands_counter_exit, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_column, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_cell, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_first_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_last_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_first_column, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_last_column, 0);
    ck_assert_uint_eq(test_commands_counter_show_error, 0);
    commands_shutdown();
}
END_TEST

START_TEST(commands_go_to_column)
{
    test_commands_mock_reset();
    command_executors_t executors = {
        .exit = &test_commands_mocked_exit,
        .go_to_line = &test_commands_mock_go_to_line,
        .go_to_column = &test_commands_mock_go_to_column,
        .go_to_cell = &test_commands_mock_go_to_cell,
        .go_to_first_line = &test_commands_mock_go_to_first_line,
        .go_to_last_line = &test_commands_mock_go_to_last_line,
        .go_to_first_column = &test_commands_mock_go_to_first_column,
        .go_to_last_column = &test_commands_mock_go_to_last_column,
        .show_error = &test_commands_mock_show_error};
    commands_init(&executors);
    commands_execute(":c457");
    ck_assert_uint_eq(test_commands_counter_go_to_column, 1);
    ck_assert_uint_eq(test_commands_last_column, 456);
    ck_assert_uint_eq(test_commands_counter_exit, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_cell, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_first_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_last_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_first_column, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_last_column, 0);
    ck_assert_uint_eq(test_commands_counter_show_error, 0);
    commands_shutdown();
}
END_TEST

START_TEST(commands_go_to_cell)
{
    test_commands_mock_reset();
    command_executors_t executors = {
        .exit = &test_commands_mocked_exit,
        .go_to_line = &test_commands_mock_go_to_line,
        .go_to_column = &test_commands_mock_go_to_column,
        .go_to_cell = &test_commands_mock_go_to_cell,
        .go_to_first_line = &test_commands_mock_go_to_first_line,
        .go_to_last_line = &test_commands_mock_go_to_last_line,
        .go_to_first_column = &test_commands_mock_go_to_first_column,
        .go_to_last_column = &test_commands_mock_go_to_last_column,
        .show_error = &test_commands_mock_show_error};
    commands_init(&executors);
    commands_execute(":57x32");
    ck_assert_uint_eq(test_commands_counter_go_to_cell, 1);
    ck_assert_uint_eq(test_commands_last_column, 56);
    ck_assert_uint_eq(test_commands_last_line, 31);
    ck_assert_uint_eq(test_commands_counter_exit, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_column, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_first_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_last_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_first_column, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_last_column, 0);
    ck_assert_uint_eq(test_commands_counter_show_error, 0);
    commands_shutdown();
}
END_TEST

START_TEST(commands_invalid)
{
    test_commands_mock_reset();
    command_executors_t executors = {
        .exit = &test_commands_mocked_exit,
        .go_to_line = &test_commands_mock_go_to_line,
        .go_to_column = &test_commands_mock_go_to_column,
        .go_to_cell = &test_commands_mock_go_to_cell,
        .go_to_first_line = &test_commands_mock_go_to_first_line,
        .go_to_last_line = &test_commands_mock_go_to_last_line,
        .go_to_first_column = &test_commands_mock_go_to_first_column,
        .go_to_last_column = &test_commands_mock_go_to_last_column,
        .show_error = &test_commands_mock_show_error};
    commands_init(&executors);
    commands_execute(":aaaaa");
    ck_assert_uint_eq(test_commands_counter_show_error, 1);
    ck_assert_str_eq(test_commands_last_shown_error_message, "Unknown command: :aaaaa");
    ck_assert_uint_eq(test_commands_counter_go_to_column, 0);
    ck_assert_uint_eq(test_commands_counter_exit, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_cell, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_first_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_last_line, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_first_column, 0);
    ck_assert_uint_eq(test_commands_counter_go_to_last_column, 0);
    commands_shutdown();
}
END_TEST

Suite *matrix_test_suite(void)
{
    Suite *s = suite_create("Navigation");
    TCase *tc_core = tcase_create("Actions");

    tcase_add_test(tc_core, commands_exit);
    tcase_add_test(tc_core, commands_go_to_line);
    tcase_add_test(tc_core, commands_go_to_column);
    tcase_add_test(tc_core, commands_go_to_cell);
    tcase_add_test(tc_core, commands_invalid);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = matrix_test_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_log(sr, "output-commands.log");
    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}