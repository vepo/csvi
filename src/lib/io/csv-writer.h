#ifndef CSVI_CSV_WRITER_H_
#define CSVI_CSV_WRITER_H_

#include <stdbool.h>
#include <stddef.h>

#include "io/csv-reader.h"

bool csv_writer_field_needs_quote(const char *value, char separator);
int csv_writer_format_field(const char *value, char separator, char *out, size_t out_len);
int csv_writer_write_file(const char *path,
                          const csv_contents *contents,
                          char separator,
                          char *errbuf,
                          size_t errbuf_len);

#endif
