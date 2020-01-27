#include <check.h>
#include <stdlib.h>
#include <stdio.h>

#include "csv-reader.h"

csv_token *mock_token(size_t y, size_t x, char *data, csv_token *next)
{
    csv_token *token = (csv_token *)malloc(sizeof(csv_token));
    token->data = data;
    token->y = y;
    token->x = x;
    token->next = next;
    return token;
}

#define assure(v, value, null_value) v != NULL ? value : null_value

void assert_csv_token_eq(csv_token *tk1, csv_token *tk2)
{
    ck_assert_msg((tk1 == NULL && tk2 == NULL) || (tk1 != NULL && tk2 != NULL), "One of the tokens is null! tk1 is null? %s data=%s y=%d x=%d  tk2 is null? %s data=%s y=%d x=%d", assure(tk1, "Yes", "No"), assure(tk1, tk1->data, "NULL"), assure(tk1, tk1->y, -1), assure(tk1, tk1->x, -1), assure(tk1, "Yes", "No"), assure(tk2, tk2->data, "NULL"), assure(tk2, tk2->y, -1), assure(tk2, tk2->x, -1));
    if (tk1 && tk2)
    {
        ck_assert_str_eq(tk1->data, tk2->data);
        ck_assert_uint_eq(tk1->x, tk2->x);
        ck_assert_uint_eq(tk1->y, tk2->y);
        assert_csv_token_eq(tk1->next, tk2->next);
    }
}
void print_csv_token(csv_token *token)
{
    if (token)
    {
        printf("Token (%ld, %ld) %s\n", token->y, token->x, token->data);
        print_csv_token(token->next);
    }
    else
    {
        printf("END\n");
    }
    fflush(stdout);
}

#define LARGE_CONTENT "qwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnm"
#define LARGE_CONTENT_WITH_QUOTE "   \"qwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnm\"    "

START_TEST(test_csv_reader_simple_csv)
{
    csv_contents *open_file = csv_reader_read_file("simple.csv");
    ck_assert_ptr_ne(open_file, NULL);
    ck_assert_uint_eq(open_file->columns, 3);
    ck_assert_uint_eq(open_file->lines, 3);

    csv_token *expected = mock_token(0, 0, "cell 0,0",
                                     mock_token(0, 1, "value-1",
                                                mock_token(0, 2, LARGE_CONTENT,
                                                           mock_token(0, 3, "value-1",
                                                                      mock_token(1, 0, "cell 1,0",
                                                                                 mock_token(1, 1, "value-2    ;  ",
                                                                                            mock_token(1, 2, LARGE_CONTENT,
                                                                                                       mock_token(1, 3, "value-2;",
                                                                                                                  mock_token(2, 0, "cell 2,0",
                                                                                                                             mock_token(2, 1, "value-3 ; , ;",
                                                                                                                                        mock_token(2, 2, LARGE_CONTENT_WITH_QUOTE,
                                                                                                                                                   mock_token(2, 3, "value-3", NULL))))))))))));
    // print_csv_token(open_file->first);
    // print_csv_token(expected);
    assert_csv_token_eq(open_file->first, expected);
}
END_TEST

START_TEST(test_csv_reader_complex_csv)
{
    csv_contents *open_file = csv_reader_read_file("complex.csv");
    ck_assert_ptr_ne(open_file, NULL);
    ck_assert_uint_eq(open_file->columns, 4);
    ck_assert_uint_eq(open_file->lines, 4);
    csv_token *expected = mock_token(0, 0, "cell 0,0",
                                     mock_token(0, 1, "cell 0,1",
                                                mock_token(0, 2, "cell 0,2",
                                                           mock_token(1, 0, "cell 1,0",
                                                                      mock_token(1, 1, "cell 1,1",
                                                                                 mock_token(1, 2, "cell 1,2",
                                                                                            mock_token(2, 0, "cell 2,0",
                                                                                                       mock_token(2, 1, "cell 2,1",
                                                                                                                  mock_token(2, 2, "cell 2,2",
                                                                                                                             mock_token(3, 0, "cell 3,0",
                                                                                                                                        mock_token(3, 1, "cell 3,1",
                                                                                                                                                   mock_token(3, 2, "cell 3,2", NULL))))))))))));
    // print_csv_token(open_file->first);
    // print_csv_token(expected);
    assert_csv_token_eq(open_file->first, expected);
}
END_TEST


Suite *csv_parser_test_suite(void)
{
    Suite *s = suite_create("CSV Parser");
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_csv_reader_simple_csv);
    tcase_add_test(tc_core, test_csv_reader_complex_csv);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = csv_parser_test_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}