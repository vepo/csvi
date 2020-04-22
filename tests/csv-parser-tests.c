#include <check.h>
#include <stdlib.h>
#include <stdio.h>

#include "csv-reader.h"
#include "mock-token.h"
#include "test-resources.h"

#define assure(v, value, null_value) v != NULL ? value : null_value

void assert_csv_token_eq(csv_token *expected, csv_token *real)
{
    ck_assert_msg((expected == NULL && real == NULL) || (expected != NULL && real != NULL), "One of the tokens is null! expected is null? %s data=%s y=%d x=%d  tk2 is null? %s data=%s y=%d x=%d", assure(expected, "Yes", "No"), assure(expected, expected->data, "NULL"), assure(expected, expected->y, -1), assure(expected, expected->x, -1), assure(expected, "Yes", "No"), assure(real, real->data, "NULL"), assure(real, real->y, -1), assure(real, real->x, -1));
    if (expected && real)
    {
        ck_assert_str_eq(expected->data, real->data);
        ck_assert_uint_eq(expected->x, real->x);
        ck_assert_uint_eq(expected->y, real->y);
        assert_csv_token_eq(expected->next, real->next);
    }
}
void print_csv_token(csv_token *token)
{
    if (token)
    {
        printf("Token (%ld, %ld) %s\n", token->y, token->x, token->data);
        fflush(stdout);
        print_csv_token(token->next);
    }
    else
    {
        printf("END\n");
        fflush(stdout);
    }
}

#define LARGE_CONTENT "qwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnm"
#define LARGE_CONTENT_WITH_QUOTE "   \"qwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnm\"    "

START_TEST(test_csv_reader_simple_csv)
{
    csv_contents *open_file = csv_reader_read_file(test_resource_get("SIMPLE_CONTENTS"));

    // print_csv_token(open_file->first);

    ck_assert_ptr_ne(open_file, NULL);
    ck_assert_uint_eq(open_file->columns, 3);
    ck_assert_uint_eq(open_file->lines, 4);

    csv_token *expected = mock_token(0, 0, "cell 0,0", NULL);
    mock_token(0, 1, "cell 0,1", expected);
    mock_token(0, 2, "cell 0,2", expected);
    mock_token(1, 0, "cell 1,0", expected);
    mock_token(1, 1, "cell 1,1", expected);
    mock_token(1, 2, "cell 1,2", expected);
    mock_token(2, 0, "cell 2,0", expected);
    mock_token(2, 1, "cell 2,1", expected);
    mock_token(2, 2, "cell 2,2", expected);
    mock_token(3, 0, "cell 3,0", expected);
    mock_token(3, 1, "cell 3,1", expected);
    mock_token(3, 2, "cell 3,2", expected);

    // print_csv_token(expected);
    assert_csv_token_eq(expected, open_file->first);
}
END_TEST

START_TEST(test_csv_reader_complex_csv)
{
    csv_contents *open_file = csv_reader_read_file(test_resource_get("COMPLEX_CONTENTS"));

    // print_csv_token(open_file->first);

    ck_assert_ptr_ne(open_file, NULL);
    ck_assert_uint_eq(open_file->columns, 4);
    ck_assert_uint_eq(open_file->lines, 3);
    csv_token *expected = mock_token(0, 0, "cell 0,0", NULL);
    mock_token(0, 1, "value-1", expected);
    mock_token(0, 2, LARGE_CONTENT, expected);
    mock_token(0, 3, "value-1", expected);
    mock_token(1, 0, "cell 1,0", expected);
    mock_token(1, 1, "value-2    ;  ", expected);
    mock_token(1, 2, LARGE_CONTENT, expected);
    mock_token(1, 3, "value-2;", expected);
    mock_token(2, 0, "cell 2,0", expected);
    mock_token(2, 1, "value-3 ; , ;", expected);
    mock_token(2, 2, LARGE_CONTENT_WITH_QUOTE, expected);
    mock_token(2, 3, "value-3", expected);
    // print_csv_token(open_file->first);

    // print_csv_token(expected);
    assert_csv_token_eq(expected, open_file->first);
}
END_TEST

START_TEST(test_csv_line_break_csv)
{
    csv_contents *open_file = csv_reader_read_file(test_resource_get("LINE_BREAK_CONTENTS"));

    // print_csv_token(open_file->first);

    ck_assert_ptr_ne(open_file, NULL);
    ck_assert_uint_eq(open_file->columns, 5);
    ck_assert_uint_eq(open_file->lines, 3);
    csv_token *expected = mock_token(0, 0, "Description", NULL);
    mock_token(0, 1, "value1", expected);
    mock_token(0, 2, "value 2", expected);
    mock_token(0, 3, "value;", expected);
    mock_token(0, 4, "value 5\"", expected);
    mock_token(1, 0, "This is a csv file", expected);
    mock_token(1, 1, "Value with line\nbreak\nbreaking again", expected);
    mock_token(1, 2, "123131", expected);
    mock_token(1, 3, "1231312", expected);
    mock_token(1, 4, "qweqwe", expected);
    mock_token(2, 0, "line 2", expected);
    mock_token(2, 1, "value 1", expected);
    mock_token(2, 2, "value 2", expected);
    mock_token(2, 3, "value 3", expected);
    mock_token(2, 4, "value 4", expected);
    // print_csv_token(open_file->first);

    // print_csv_token(expected);
    assert_csv_token_eq(expected, open_file->first);
}
END_TEST

START_TEST(test_csv_line_end_csv)
{
    csv_contents *open_file = csv_reader_read_file(test_resource_get("LINE_END_CONTENTS"));

    // print_csv_token(open_file->first);

    ck_assert_ptr_ne(open_file, NULL);
    ck_assert_uint_eq(open_file->columns, 3);
    ck_assert_uint_eq(open_file->lines, 3);
    csv_token *expected = mock_token(0, 0, "cell 0,0", NULL);
    mock_token(0, 1, "cell 0,1", expected);
    mock_token(0, 2, "cell 0,2", expected);
    mock_token(1, 0, "cell 1,0", expected);
    mock_token(1, 1, "cell 1,1", expected);
    mock_token(1, 2, "cell 1,2", expected);
    mock_token(2, 0, "cell 2,0", expected);
    mock_token(2, 1, "cell 2,1", expected);
    mock_token(2, 2, "cell 2,2", expected);

    // print_csv_token(expected);
    assert_csv_token_eq(expected, open_file->first);
}
END_TEST

Suite *csv_parser_test_suite(void)
{
    Suite *s = suite_create("CSV Parser");
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_csv_reader_simple_csv);
    tcase_add_test(tc_core, test_csv_reader_complex_csv);
    tcase_add_test(tc_core, test_csv_line_break_csv);
    tcase_add_test(tc_core, test_csv_line_end_csv);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = csv_parser_test_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_log(sr, "test-output.log");
    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}