#include <check.h>
#include <stdlib.h>
#include <string.h>

#include "cmd/commands.h"

static int counter_exit;
static int counter_line;
static int counter_col;
static int counter_cell;
static int counter_first_line;
static int counter_last_line;
static int counter_first_col;
static int counter_last_col;
static int counter_error;
static int counter_save;
static int counter_exit_force;
static int last_save_result;
static int last_line;
static int last_col;
static char last_error[256];

static void reset_counters(void)
{
    counter_exit = 0;
    counter_line = 0;
    counter_col = 0;
    counter_cell = 0;
    counter_first_line = 0;
    counter_last_line = 0;
    counter_first_col = 0;
    counter_last_col = 0;
    counter_error = 0;
    counter_save = 0;
    counter_exit_force = 0;
    last_save_result = 0;
    last_line = -1;
    last_col = -1;
    last_error[0] = '\0';
}

static void mock_exit(int code)
{
    (void)code;
    counter_exit++;
}

static void mock_line(size_t line)
{
    counter_line++;
    last_line = (int)line;
}

static void mock_col(size_t col)
{
    counter_col++;
    last_col = (int)col;
}

static void mock_cell(size_t col, size_t line)
{
    counter_cell++;
    last_col = (int)col;
    last_line = (int)line;
}

static void mock_first_line(void) { counter_first_line++; }
static void mock_last_line(void) { counter_last_line++; }
static void mock_first_col(void) { counter_first_col++; }
static void mock_last_col(void) { counter_last_col++; }

static void mock_error(char *msg)
{
    counter_error++;
    strncpy(last_error, msg, sizeof(last_error) - 1);
}

static int mock_save(void)
{
    counter_save++;
    return last_save_result;
}

static void mock_exit_force(int code)
{
    (void)code;
    counter_exit_force++;
}

static command_executors_t mock_executors(void)
{
    command_executors_t executors = {
        .go_to_line = mock_line,
        .go_to_column = mock_col,
        .go_to_cell = mock_cell,
        .go_to_first_line = mock_first_line,
        .go_to_last_line = mock_last_line,
        .go_to_first_column = mock_first_col,
        .go_to_last_column = mock_last_col,
        .save_file = mock_save,
        .show_error = mock_error,
        .exit = mock_exit,
        .exit_force = mock_exit_force};
    return executors;
}

START_TEST(test_cell_command_quit)
{
    reset_counters();
    command_executors_t executors = mock_executors();
    commands_init(&executors);
    commands_execute(":quit");
    ck_assert_int_eq(counter_exit, 1);
    commands_shutdown();
}
END_TEST

START_TEST(test_cell_command_line)
{
    reset_counters();
    command_executors_t executors = mock_executors();
    commands_init(&executors);
    commands_execute(":line 57");
    ck_assert_int_eq(counter_line, 1);
    ck_assert_int_eq(last_line, 56);
    commands_shutdown();
}
END_TEST

START_TEST(test_cell_command_col)
{
    reset_counters();
    command_executors_t executors = mock_executors();
    commands_init(&executors);
    commands_execute(":col 4");
    ck_assert_int_eq(counter_col, 1);
    ck_assert_int_eq(last_col, 3);
    commands_shutdown();
}
END_TEST

START_TEST(test_cell_command_cell)
{
    reset_counters();
    command_executors_t executors = mock_executors();
    commands_init(&executors);
    commands_execute(":cell 57,32");
    ck_assert_int_eq(counter_cell, 1);
    ck_assert_int_eq(last_line, 56);
    ck_assert_int_eq(last_col, 31);
    commands_shutdown();
}
END_TEST

START_TEST(test_cell_command_top_bottom)
{
    reset_counters();
    command_executors_t executors = mock_executors();
    commands_init(&executors);
    commands_execute(":top");
    ck_assert_int_eq(counter_first_line, 1);
    commands_execute(":bottom");
    ck_assert_int_eq(counter_last_line, 1);
    commands_shutdown();
}
END_TEST

START_TEST(test_cell_command_invalid)
{
    reset_counters();
    command_executors_t executors = mock_executors();
    commands_init(&executors);
    commands_execute(":nope");
    ck_assert_int_eq(counter_error, 1);
    commands_shutdown();
}
END_TEST

START_TEST(test_cell_command_write)
{
    reset_counters();
    command_executors_t executors = mock_executors();
    commands_init(&executors);
    commands_execute(":w");
    ck_assert_int_eq(counter_save, 1);
    commands_shutdown();
}
END_TEST

START_TEST(test_cell_command_wq)
{
    reset_counters();
    last_save_result = 0;
    command_executors_t executors = mock_executors();
    commands_init(&executors);
    commands_execute(":wq");
    ck_assert_int_eq(counter_save, 1);
    ck_assert_int_eq(counter_exit, 1);
    commands_shutdown();
}
END_TEST

START_TEST(test_cell_command_wq_save_fail)
{
    reset_counters();
    last_save_result = -1;
    command_executors_t executors = mock_executors();
    commands_init(&executors);
    commands_execute(":wq");
    ck_assert_int_eq(counter_save, 1);
    ck_assert_int_eq(counter_exit, 0);
    commands_shutdown();
}
END_TEST

START_TEST(test_cell_command_quit_force)
{
    reset_counters();
    command_executors_t executors = mock_executors();
    commands_init(&executors);
    commands_execute(":q!");
    ck_assert_int_eq(counter_exit_force, 1);
    commands_shutdown();
}
END_TEST

Suite *cell_commands_suite(void)
{
    Suite *s = suite_create("cell_commands");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_cell_command_quit);
    tcase_add_test(tc, test_cell_command_line);
    tcase_add_test(tc, test_cell_command_col);
    tcase_add_test(tc, test_cell_command_cell);
    tcase_add_test(tc, test_cell_command_top_bottom);
    tcase_add_test(tc, test_cell_command_invalid);
    tcase_add_test(tc, test_cell_command_write);
    tcase_add_test(tc, test_cell_command_wq);
    tcase_add_test(tc, test_cell_command_wq_save_fail);
    tcase_add_test(tc, test_cell_command_quit_force);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = cell_commands_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? 0 : 1;
}
