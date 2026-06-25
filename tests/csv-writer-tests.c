#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "io/csv-reader.h"
#include "io/csv-writer.h"
#include "test-resources.h"

START_TEST(test_writer_needs_quote)
{
    ck_assert(!csv_writer_field_needs_quote("hello", ';'));
    ck_assert(csv_writer_field_needs_quote("hello;world", ';'));
    ck_assert(csv_writer_field_needs_quote("hello\"world", ';'));
    ck_assert(csv_writer_field_needs_quote("hello\nworld", ';'));
    ck_assert(csv_writer_field_needs_quote(" padded", ';'));
    ck_assert(csv_writer_field_needs_quote("padded ", ';'));
}

START_TEST(test_writer_format_field)
{
    char out[128];

    ck_assert_int_eq(csv_writer_format_field("plain", ';', out, sizeof(out)), 0);
    ck_assert_str_eq(out, "plain");

    ck_assert_int_eq(csv_writer_format_field("a;b", ';', out, sizeof(out)), 0);
    ck_assert_str_eq(out, "\"a;b\"");

    ck_assert_int_eq(csv_writer_format_field("say \"hi\"", ';', out, sizeof(out)), 0);
    ck_assert_str_eq(out, "\"say \"\"hi\"\"\"");

    ck_assert_int_eq(csv_writer_format_field("line\nbreak", ';', out, sizeof(out)), 0);
    ck_assert_str_eq(out, "\"line\nbreak\"");
}

START_TEST(test_writer_empty_cells)
{
    csv_contents *contents = calloc(1, sizeof(csv_contents));
    contents->lines = 2;
    contents->columns = 3;
    contents->index = calloc(6, sizeof(csv_token *));

    csv_token *t00 = calloc(1, sizeof(csv_token));
    t00->data = strdup("a");
    t00->x = 0;
    t00->y = 0;
    contents->index[0] = t00;
    contents->first = t00;

    csv_token *t02 = calloc(1, sizeof(csv_token));
    t02->data = strdup("c");
    t02->x = 2;
    t02->y = 0;
    t02->next = NULL;
    t00->next = t02;
    contents->index[2] = t02;

    char *path = test_resource_get("SIMPLE_CONTENTS");
    char errbuf[256];
    ck_assert_int_eq(csv_writer_write_file(path, contents, ';', errbuf, sizeof(errbuf)), 0);

    csv_contents *reloaded = csv_reader_read_file(path, errbuf, sizeof(errbuf));
    ck_assert_ptr_ne(reloaded, NULL);
    ck_assert_uint_eq(reloaded->lines, 2);
    ck_assert_uint_eq(reloaded->columns, 3);

    csv_token *mid = csv_reader_get_token(1, 0, reloaded);
    ck_assert_ptr_ne(mid, NULL);
    ck_assert_str_eq(mid->data, "");

    csv_contents_dispose(reloaded);
    csv_contents_dispose(contents);
    unlink(path);
    free(path);
}

START_TEST(test_set_cell_existing_and_new)
{
    char *path = test_resource_get("SIMPLE_CONTENTS");
    csv_contents *contents = csv_reader_read_file(path, NULL, 0);
    ck_assert_ptr_ne(contents, NULL);

    ck_assert_int_eq(csv_reader_set_cell(contents, 0, 0, "updated"), 0);
    csv_token *token = csv_reader_get_token(0, 0, contents);
    ck_assert_str_eq(token->data, "updated");

    ck_assert_int_eq(csv_reader_set_cell(contents, 1, 0, "new"), 0);
    token = csv_reader_get_token(1, 0, contents);
    ck_assert_ptr_ne(token, NULL);
    ck_assert_str_eq(token->data, "new");

    ck_assert_int_eq(csv_reader_set_cell(contents, contents->columns, 0, "bad"), -1);

    csv_contents_dispose(contents);
    unlink(path);
    free(path);
}

START_TEST(test_writer_round_trip)
{
    char *path = test_resource_get("SIMPLE_CONTENTS");
    char *out_path = test_resource_get("SIMPLE_CONTENTS");
    csv_contents *contents = csv_reader_read_file(path, NULL, 0);
    ck_assert_ptr_ne(contents, NULL);

    ck_assert_int_eq(csv_reader_set_cell(contents, 1, 1, "changed;value"), 0);

    char errbuf[256];
    ck_assert_int_eq(csv_writer_write_file(out_path, contents, ';', errbuf, sizeof(errbuf)), 0);

    csv_contents *reloaded = csv_reader_read_file(out_path, errbuf, sizeof(errbuf));
    ck_assert_ptr_ne(reloaded, NULL);

    csv_token *token = csv_reader_get_token(1, 1, reloaded);
    ck_assert_ptr_ne(token, NULL);
    ck_assert_str_eq(token->data, "changed;value");

    csv_contents_dispose(contents);
    csv_contents_dispose(reloaded);
    unlink(path);
    unlink(out_path);
    free(path);
    free(out_path);
}

Suite *csv_writer_suite(void)
{
    Suite *s = suite_create("csv_writer");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_writer_needs_quote);
    tcase_add_test(tc, test_writer_format_field);
    tcase_add_test(tc, test_writer_empty_cells);
    tcase_add_test(tc, test_set_cell_existing_and_new);
    tcase_add_test(tc, test_writer_round_trip);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int failed;
    Suite *s = csv_writer_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? 0 : 1;
}
