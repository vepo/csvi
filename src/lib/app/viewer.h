#ifndef CSVI_VIEWER_H_
#define CSVI_VIEWER_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct csvi_viewer csvi_viewer_t;

typedef enum csvi_color_mode
{
    CSVI_COLOR_AUTO = 0,
    CSVI_COLOR_NEVER = 1,
    CSVI_COLOR_ALWAYS = 2
} csvi_color_mode_t;

typedef struct csvi_viewer_options
{
    char separator;
    csvi_color_mode_t color_mode;
    bool grid;
    bool header;
    const char *file_path;
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
