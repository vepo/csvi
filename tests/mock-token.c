#include "mock-token.h"

#include <stdlib.h>
#include <string.h>

csv_token *mock_token(size_t y, size_t x, char *data, csv_token *start)
{
    csv_token *token = (csv_token *)malloc(sizeof(csv_token));
    char *token_data = strdup(data);
    if (!token || !token_data)
    {
        free(token);
        free(token_data);
        return NULL;
    }

    token->data = token_data;
    token->y = y;
    token->x = x;
    token->next = NULL;

    if (start)
    {
        csv_token *last = start;
        while (last->next)
        {
            last = last->next;
        }
        last->next = token;
    }
    return token;
}
