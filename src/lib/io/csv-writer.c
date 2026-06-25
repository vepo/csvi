#include "io/csv-writer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool csv_writer_field_needs_quote(const char *value, char separator)
{
    if (!value)
    {
        return false;
    }

    if (value[0] == '\0')
    {
        return false;
    }

    if (isspace((unsigned char)value[0]))
    {
        return true;
    }

    size_t len = strlen(value);
    if (isspace((unsigned char)value[len - 1]))
    {
        return true;
    }

    for (size_t i = 0; value[i] != '\0'; ++i)
    {
        char c = value[i];
        if (c == separator || c == '"' || c == '\n' || c == '\r')
        {
            return true;
        }
    }

    return false;
}

int csv_writer_format_field(const char *value, char separator, char *out, size_t out_len)
{
    if (!out || out_len == 0)
    {
        return -1;
    }

    const char *field = value ? value : "";

    if (!csv_writer_field_needs_quote(field, separator))
    {
        if (strlen(field) + 1 > out_len)
        {
            return -1;
        }
        strcpy(out, field);
        return 0;
    }

    size_t needed = 2;
    for (size_t i = 0; field[i] != '\0'; ++i)
    {
        needed += (field[i] == '"') ? 2 : 1;
    }

    if (needed + 1 > out_len)
    {
        return -1;
    }

    size_t pos = 0;
    out[pos++] = '"';
    for (size_t i = 0; field[i] != '\0'; ++i)
    {
        if (field[i] == '"')
        {
            out[pos++] = '"';
        }
        out[pos++] = field[i];
    }
    out[pos++] = '"';
    out[pos] = '\0';
    return 0;
}

static int append_field(char **buf, size_t *len, size_t *cap, const char *field, char separator, bool first_in_row)
{
    char formatted[8192];
    if (csv_writer_format_field(field, separator, formatted, sizeof(formatted)) != 0)
    {
        return -1;
    }

    if (!first_in_row)
    {
        if (*len + 1 >= *cap)
        {
            size_t new_cap = (*cap == 0) ? 4096 : *cap * 2;
            char *grown = realloc(*buf, new_cap);
            if (!grown)
            {
                return -1;
            }
            *buf = grown;
            *cap = new_cap;
        }
        (*buf)[(*len)++] = separator;
        (*buf)[*len] = '\0';
    }

    size_t flen = strlen(formatted);
    while (*len + flen + 1 >= *cap)
    {
        size_t new_cap = (*cap == 0) ? 4096 : *cap * 2;
        char *grown = realloc(*buf, new_cap);
        if (!grown)
        {
            return -1;
        }
        *buf = grown;
        *cap = new_cap;
    }

    memcpy(*buf + *len, formatted, flen);
    *len += flen;
    (*buf)[*len] = '\0';
    return 0;
}

static char *build_file_body(const csv_contents *contents, char separator, char *errbuf, size_t errbuf_len)
{
    if (!contents || contents->columns == 0 || contents->lines == 0)
    {
        if (errbuf && errbuf_len > 0)
        {
            snprintf(errbuf, errbuf_len, "empty csv contents");
        }
        return NULL;
    }

    char *body = NULL;
    size_t len = 0;
    size_t cap = 0;

    for (size_t y = 0; y < contents->lines; ++y)
    {
        for (size_t x = 0; x < contents->columns; ++x)
        {
            csv_token *token = csv_reader_get_token(x, y, contents);
            const char *value = (token && token->data) ? token->data : "";
            if (append_field(&body, &len, &cap, value, separator, x == 0) != 0)
            {
                free(body);
                if (errbuf && errbuf_len > 0)
                {
                    snprintf(errbuf, errbuf_len, "out of memory");
                }
                return NULL;
            }
        }

        if (len + 2 >= cap)
        {
            size_t new_cap = (cap == 0) ? 4096 : cap * 2;
            char *grown = realloc(body, new_cap);
            if (!grown)
            {
                free(body);
                if (errbuf && errbuf_len > 0)
                {
                    snprintf(errbuf, errbuf_len, "out of memory");
                }
                return NULL;
            }
            body = grown;
            cap = new_cap;
        }
        body[len++] = '\n';
        body[len] = '\0';
    }

    return body;
}

int csv_writer_write_file(const char *path,
                          const csv_contents *contents,
                          char separator,
                          char *errbuf,
                          size_t errbuf_len)
{
    if (!path)
    {
        if (errbuf && errbuf_len > 0)
        {
            snprintf(errbuf, errbuf_len, "missing path");
        }
        return -1;
    }

    char *body = build_file_body(contents, separator, errbuf, errbuf_len);
    if (!body)
    {
        return -1;
    }

    char tmp_path[4096];
    snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);

    FILE *fp = fopen(tmp_path, "w");
    if (!fp)
    {
        free(body);
        if (errbuf && errbuf_len > 0)
        {
            snprintf(errbuf, errbuf_len, "could not open '%s' for writing", tmp_path);
        }
        return -1;
    }

    size_t body_len = strlen(body);
    if (fwrite(body, 1, body_len, fp) != body_len)
    {
        fclose(fp);
        unlink(tmp_path);
        free(body);
        if (errbuf && errbuf_len > 0)
        {
            snprintf(errbuf, errbuf_len, "write failed");
        }
        return -1;
    }

    free(body);

    if (fflush(fp) != 0)
    {
        fclose(fp);
        unlink(tmp_path);
        if (errbuf && errbuf_len > 0)
        {
            snprintf(errbuf, errbuf_len, "flush failed");
        }
        return -1;
    }

    int fd = fileno(fp);
    if (fd >= 0)
    {
        fsync(fd);
    }
    fclose(fp);

    if (rename(tmp_path, path) != 0)
    {
        unlink(tmp_path);
        if (errbuf && errbuf_len > 0)
        {
            snprintf(errbuf, errbuf_len, "could not rename '%s' to '%s'", tmp_path, path);
        }
        return -1;
    }

    return 0;
}
