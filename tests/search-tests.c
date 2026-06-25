#include <check.h>
#include <stdlib.h>
#include <string.h>

#include "app/search.h"
#include "mock-token.h"

static csv_contents *make_search_file(void)
{
    csv_contents *contents = calloc(1, sizeof(csv_contents));
    contents->columns = 2;
    contents->lines = 2;
    contents->index = calloc(4, sizeof(csv_token *));
    contents->index[0] = mock_token(0, 0, "alpha", NULL);
    contents->index[1] = mock_token(0, 1, "beta", NULL);
    contents->index[2] = mock_token(1, 0, "alphabet", NULL);
    contents->index[3] = mock_token(1, 1, "gamma", NULL);
    return contents;
}

START_TEST(test_search_find_and_next)
{
    csv_contents *file = make_search_file();
    csvi_search_t *search = csvi_search_create(file);
    ck_assert(csvi_search_find(search, "alpha"));
    ck_assert_uint_eq(csvi_search_match_count(search), 2);

    coordinates_t match;
    ck_assert(csvi_search_next(search, &match));
    ck_assert_uint_eq(match.y, 0);
    ck_assert_uint_eq(match.x, 0);

    ck_assert(csvi_search_next(search, &match));
    ck_assert_uint_eq(match.y, 1);
    ck_assert_uint_eq(match.x, 0);

    csvi_search_dispose(search);
    csv_contents_dispose(file);
}
END_TEST

Suite *search_suite(void)
{
    Suite *s = suite_create("search");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_search_find_and_next);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = search_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? 0 : 1;
}
