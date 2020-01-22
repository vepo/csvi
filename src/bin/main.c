#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "csv-reader.h"
#include "matrix-presentation.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "No file to read...");
        exit(1);
    }
    csv_reader_read_file(argv[1]);
    matrix_presentation_init();
    sleep(10);
    matrix_presentation_exit();
    return 0;
}
