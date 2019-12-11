#include "csv-reader.h"
#include "buffer-reader.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

csv_contents *csv_reader_read_file(char *path)
{
    csv_contents *contents = (csv_contents *)malloc(sizeof(csv_contents));
    contents->lines = 0;
    contents->columns = 0;
    contents->first = NULL;
    csv_token *current_token = NULL;
    buffer_reader *reader = buffer_reader_open(path);

    size_t read = 0;
    bool is_escaped = false;
    while (buffer_reader_acquire(reader))
    {
        while (buffer_reader_has_data(reader))
        {
            if (current_token == NULL)
            {
                current_token = (csv_token *)malloc(sizeof(csv_token));
                contents->first = current_token;
            }
            for (; buffer_reader_available(reader) > 0 && isspace(buffer_reader_current_char(reader, 1)); buffer_reader_commit(reader, 1))
                ;

            is_escaped = buffer_reader_current_char(reader, 1) == '"';
            if (is_escaped)
            {
                size_t token_end = 2;
                size_t scaped_counter = 0;
                bool in_scaped_char = false;
                while (buffer_reader_available(reader) > token_end + 1 && !in_scaped_char && buffer_reader_current_char(reader, token_end + 1) != '"')
                {
                    token_end++;
                    if (!in_scaped_char && buffer_reader_current_char(reader, token_end) == '"')
                    {
                        in_scaped_char = true;
                        scaped_counter++;
                    }
                    else
                    {
                        in_scaped_char = false;
                    }
                }
                for (; buffer_reader_available(reader) > token_end + 2 && !(buffer_reader_current_char(reader, token_end + 1) == '"' && buffer_reader_current_char(reader, token_end + 2) != '"'); token_end++)
                    ;
                char *token = (char *)malloc(sizeof(char) * (token_end - 1 - scaped_counter));
                in_scaped_char = false;
                scaped_counter = 0;
                for (int bufferPos = 2, tokenPos = 0; bufferPos <= token_end; bufferPos++)
                {

                    if (!in_scaped_char && buffer_reader_current_char(reader, bufferPos) == '"')
                    {
                        in_scaped_char = true;
                        scaped_counter++;
                    }
                    else
                    {
                        in_scaped_char = false;
                        token[tokenPos++] = buffer_reader_current_char(reader, bufferPos);
                    }
                }
                token[token_end - 1 - scaped_counter] = '\0';
                printf("Token: %s\n", token);
                fflush(stdout);
                current_token->data = token;
                current_token->next = (csv_token *)malloc(sizeof(csv_token));
                current_token = current_token->next;
                token_end++;
                for (; buffer_reader_available(reader) > token_end && isspace(buffer_reader_current_char(reader, token_end)); ++token_end)
                    ;
                for (; buffer_reader_available(reader) > token_end && buffer_reader_current_char(reader, token_end) != ';' && buffer_reader_current_char(reader, token_end) != '\n' && buffer_reader_current_char(reader, token_end) != EOF; ++token_end)
                    ;
                buffer_reader_commit(reader, token_end);
                token_end = token_end + 1;
            }
            else
            {
                size_t token_end = 0;
                for (; buffer_reader_available(reader) > token_end + 1 && buffer_reader_current_char(reader, token_end + 1) != ';' && buffer_reader_current_char(reader, token_end + 1) != '\n' && buffer_reader_current_char(reader, token_end) != EOF; token_end++)
                    ;
                char *token = (char *)malloc(sizeof(char) * token_end);
                buffer_reader_current_copy(reader, 1, token_end, token);
                printf("Token: %s\n", token);
                fflush(stdout);
                current_token->data = token;
                current_token->next = (csv_token *)malloc(sizeof(csv_token));
                current_token = current_token->next;
                buffer_reader_commit(reader, token_end + 1);
            }
            printf("-----------------\n");
        }
    }
    buffer_reader_release(reader);
    printf("file read!\n");
    return contents;
}