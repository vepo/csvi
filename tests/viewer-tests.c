#include <check.h>
#include <stdlib.h>

#include "app/viewer.h"
#include "io/csv-reader.h"
#include "mock-token.h"

static csv_contents *make_test_file(size_t lines, size_t columns)
{
    csv_contents *contents = calloc(1, sizeof(csv_contents));
    contents->lines = lines;
    contents->columns = columns;
    contents->first = mock_token(0, 0, "a", NULL);
    return contents;
}

START_TEST(test_go_to_last_line)
{
    csv_contents *file = make_test_file(10, 5);
    csvi_viewer_t *viewer = csvi_viewer_create_with_file(file);
    ck_assert_ptr_ne(viewer, NULL);

    csvi_viewer_exec_go_to_last_line(viewer);
    ck_assert_uint_eq(csvi_viewer_selected_cell(viewer)->y, 9);

    csvi_viewer_destroy(viewer);
    csv_contents_dispose(file);
}
END_TEST

START_TEST(test_go_to_last_column)
{
    csv_contents *file = make_test_file(10, 5);
    csvi_viewer_t *viewer = csvi_viewer_create_with_file(file);
    ck_assert_ptr_ne(viewer, NULL);

    csvi_viewer_exec_go_to_last_column(viewer);
    ck_assert_uint_eq(csvi_viewer_selected_cell(viewer)->x, 4);

    csvi_viewer_destroy(viewer);
    csv_contents_dispose(file);
}
END_TEST

START_TEST(test_commit_cell_sets_modified)
{
    csv_contents *file = make_test_file(2, 2);
    file->index = calloc(4, sizeof(csv_token *));
    file->index[0] = mock_token(0, 0, "old", NULL);
    file->first = file->index[0];

    csvi_viewer_t *viewer = csvi_viewer_create_with_file(file);
    ck_assert_ptr_ne(viewer, NULL);
    ck_assert(!csvi_viewer_file_modified(viewer));

    ck_assert_int_eq(csvi_viewer_test_commit_at(viewer, 0, 0, "new"), 0);
    ck_assert(csvi_viewer_file_modified(viewer));

    csv_token *token = csv_reader_get_token(0, 0, file);
    ck_assert_str_eq(token->data, "new");

    csvi_viewer_destroy(viewer);
    csv_contents_dispose(file);
}
END_TEST

Suite *viewer_suite(void)
{
    Suite *s = suite_create("viewer");
    TCase *tc = tcase_create("executors");
    tcase_add_test(tc, test_go_to_last_line);
    tcase_add_test(tc, test_go_to_last_column);
    tcase_add_test(tc, test_commit_cell_sets_modified);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = viewer_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? 0 : 1;
}
