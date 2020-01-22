#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "actions.h"
#include "csv-reader.h"
#include "matrix-presentation.h"

csv_contents *open_file = NULL;
size_t pos_x = 0;
size_t pos_y = 0;

void up()
{
    if (open_file)
    {
        //        open_file->
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "No file to read...");
        exit(1);
    }
    open_file = csv_reader_read_file(argv[1]);
    matrix_presentation_init();
    matrix_presentation_configure_handler(UP, &up);
    matrix_presentation_handle();
    matrix_presentation_exit();
    return 0;
}
