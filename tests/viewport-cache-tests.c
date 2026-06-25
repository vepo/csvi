#include <check.h>
#include <stdlib.h>
#include <string.h>

#include "layout/viewport-cache.h"
#include "mock-token.h"
#include "io/csv-reader.h"

static csv_contents *make_uniform_file(size_t lines, size_t columns, const char *cell_text)
{
    csv_contents *contents = calloc(1, sizeof(csv_contents));
    if (!contents)
    {
        return NULL;
    }

    contents->lines = lines;
    contents->columns = columns;
    contents->index = calloc(lines * columns, sizeof(csv_token *));
    if (!contents->index)
    {
        free(contents);
        return NULL;
    }

    for (size_t y = 0; y < lines; ++y)
    {
        for (size_t x = 0; x < columns; ++x)
        {
            contents->index[y * columns + x] = mock_token(y, x, (char *)cell_text, NULL);
        }
    }

    for (size_t i = 0; i + 1 < lines * columns; ++i)
    {
        contents->index[i]->next = contents->index[i + 1];
    }
    contents->first = contents->index[0];

    return contents;
}

START_TEST(test_viewport_cache_fit_narrow_grid)
{
    matrix_properties_t props = {
        .cell_padding_top = 0,
        .cell_padding_right = 1,
        .cell_padding_bottom = 0,
        .cell_padding_left = 1,
        .margin_top = 0,
        .margin_right = 0,
        .margin_bottom = 0,
        .margin_left = 0};

    csv_contents *file = make_uniform_file(4, 6, "abc");
    viewport_cache_t *cache = viewport_cache_build(file);
    ck_assert_ptr_ne(cache, NULL);

    coordinates_t top_cell = {.x = 0, .y = 0};
    screen_size_t grid_px = {.width = 10, .height = 10};
    screen_size_t visible = {.width = 1, .height = 1};

    viewport_cache_get_most_expanded(cache, &top_cell, &grid_px, &props, file->columns, file->lines, &visible);
    ck_assert_int_eq(2, visible.width);
    ck_assert_int_gt(visible.height, 0);

    viewport_cache_dispose(cache);
    csv_contents_dispose(file);
}
END_TEST

START_TEST(test_viewport_cache_fit_wider_grid)
{
    matrix_properties_t props = {
        .cell_padding_top = 0,
        .cell_padding_right = 1,
        .cell_padding_bottom = 0,
        .cell_padding_left = 1,
        .margin_top = 0,
        .margin_right = 0,
        .margin_bottom = 0,
        .margin_left = 0};

    csv_contents *file = make_uniform_file(4, 6, "abc");
    viewport_cache_t *cache = viewport_cache_build(file);
    ck_assert_ptr_ne(cache, NULL);

    coordinates_t top_cell = {.x = 0, .y = 0};
    screen_size_t grid_px = {.width = 15, .height = 10};
    screen_size_t visible = {.width = 1, .height = 1};

    viewport_cache_get_most_expanded(cache, &top_cell, &grid_px, &props, file->columns, file->lines, &visible);
    ck_assert_int_eq(3, visible.width);

    top_cell.x = 1;
    visible.width = 1;
    visible.height = 1;
    viewport_cache_get_most_expanded(cache, &top_cell, &grid_px, &props, file->columns, file->lines, &visible);
    ck_assert_int_eq(3, visible.width);

    viewport_cache_dispose(cache);
    csv_contents_dispose(file);
}
END_TEST

START_TEST(test_viewport_cache_update_cell_width)
{
    csv_contents *file = make_uniform_file(2, 2, "abc");
    viewport_cache_t *cache = viewport_cache_build(file);
    ck_assert_ptr_ne(cache, NULL);
    ck_assert_uint_eq(viewport_cache_col_width(cache, 0), 3);

    ck_assert_int_eq(csv_reader_set_cell(file, 0, 0, "much longer text"), 0);
    viewport_cache_update_cell(cache, file, 0, 0);
    ck_assert_uint_eq(viewport_cache_col_width(cache, 0), strlen("much longer text"));

    ck_assert_int_eq(csv_reader_set_cell(file, 0, 0, "x"), 0);
    viewport_cache_update_cell(cache, file, 0, 0);
    ck_assert_uint_eq(viewport_cache_col_width(cache, 0), 3);

    viewport_cache_dispose(cache);
    csv_contents_dispose(file);
}
END_TEST

Suite *viewport_cache_suite(void)
{
    Suite *s = suite_create("ViewportCache");
    TCase *tc = tcase_create("fit");
    tcase_add_test(tc, test_viewport_cache_fit_narrow_grid);
    tcase_add_test(tc, test_viewport_cache_fit_wider_grid);
    tcase_add_test(tc, test_viewport_cache_update_cell_width);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = viewport_cache_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_log(sr, "viewport-cache-output.log");
    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
