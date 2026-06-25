#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "app/viewer.h"
#include "common/errors.h"
#include "common/log.h"

static void usage(FILE *out)
{
    fprintf(out, "csvi - CSV Viewer %s\n\n", PACKAGE_VERSION);
    fprintf(out, "Usage: csvi [options] file\n\n");
    fprintf(out, "Options:\n");
    fprintf(out, "  --separator    -s     Cell separator (default ;)\n");
    fprintf(out, "  --color=MODE          auto|never|always (default auto)\n");
    fprintf(out, "  --grid                Draw column separators\n");
    fprintf(out, "  --header              Freeze header row\n");
    fprintf(out, "  --verbose      -V     Verbose logging to stderr\n");
    fprintf(out, "  --help         -h     Print this message\n");
    fprintf(out, "  --version      -v     Print version\n\n");
    fprintf(out, "Report issues to: %s\n", PACKAGE_BUGREPORT);
}

static csvi_color_mode_t parse_color_mode(const char *value)
{
    if (strcmp(value, "always") == 0)
    {
        return CSVI_COLOR_ALWAYS;
    }
    if (strcmp(value, "never") == 0)
    {
        return CSVI_COLOR_NEVER;
    }
    return CSVI_COLOR_AUTO;
}

int main(int argc, char *argv[])
{
    static struct option long_options[] = {
        {"separator", required_argument, NULL, 's'},
        {"color", required_argument, NULL, 'C'},
        {"grid", no_argument, NULL, 'G'},
        {"header", no_argument, NULL, 'H'},
        {"verbose", no_argument, NULL, 'V'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}};

    csvi_viewer_options_t opts = {
        .separator = ';',
        .color_mode = CSVI_COLOR_AUTO,
        .grid = false,
        .header = false,
        .file_path = NULL};

    int option_index = 0;
    int c;

    while ((c = getopt_long(argc, argv, "s:C:GHVvh", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 's':
            if (optarg[0] == '\0' || optarg[1] != '\0')
            {
                csvi_log_error("separator must be a single character\n");
                return CSVI_EXIT_USAGE;
            }
            opts.separator = optarg[0];
            break;
        case 'C':
            opts.color_mode = parse_color_mode(optarg);
            break;
        case 'G':
            opts.grid = true;
            break;
        case 'H':
            opts.header = true;
            break;
        case 'V':
            csvi_log_set_verbose(true);
            break;
        case 'v':
            printf("csvi version %s\n", PACKAGE_VERSION);
            return CSVI_EXIT_OK;
        case 'h':
            usage(stdout);
            return CSVI_EXIT_OK;
        case '?':
            csvi_log_error("unknown option\n");
            usage(stderr);
            return CSVI_EXIT_USAGE;
        default:
            usage(stderr);
            return CSVI_EXIT_USAGE;
        }
    }

    if (optind >= argc)
    {
        csvi_log_error("no file to read\n");
        usage(stderr);
        return CSVI_EXIT_USAGE;
    }

    opts.file_path = argv[optind];

    csvi_viewer_t *viewer = csvi_viewer_create(&opts);
    if (!viewer)
    {
        csvi_log_error("out of memory\n");
        return CSVI_EXIT_ERR;
    }

    int rc = csvi_viewer_open(viewer, argv[optind]);
    if (rc != CSVI_EXIT_OK)
    {
        csvi_viewer_destroy(viewer);
        return rc;
    }

    rc = csvi_viewer_run(viewer);
    csvi_viewer_destroy(viewer);
    return rc;
}
