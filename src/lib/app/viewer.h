#ifndef CSVI_VIEWER_H_
#define CSVI_VIEWER_H_

#include <stddef.h>

typedef struct csvi_viewer csvi_viewer_t;

typedef struct csvi_viewer_options
{
    char separator;
} csvi_viewer_options_t;

csvi_viewer_t *csvi_viewer_create(const csvi_viewer_options_t *opts);
int csvi_viewer_open(csvi_viewer_t *viewer, const char *path);
int csvi_viewer_run(csvi_viewer_t *viewer);
void csvi_viewer_destroy(csvi_viewer_t *viewer);

#ifdef CSVI_TEST
#include "layout/types.h"
#include "io/csv-reader.h"

const coordinates_t *csvi_viewer_selected_cell(const csvi_viewer_t *viewer);
void csvi_viewer_exec_go_to_last_line(csvi_viewer_t *viewer);
void csvi_viewer_exec_go_to_last_column(csvi_viewer_t *viewer);
csvi_viewer_t *csvi_viewer_create_with_file(const csv_contents *file);
#endif

#endif
