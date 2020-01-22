#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "actions.h"
#include "csv-reader.h"
#include "matrix-presentation.h"

void up()
{
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "No file to read...");
        exit(1);
    }
    csv_reader_read_file(argv[1]);
    matrix_presentation_init();
    matrix_presentation_configure_handler(UP, &up);
    matrix_presentation_handle();
    matrix_presentation_exit();
    return 0;
}
