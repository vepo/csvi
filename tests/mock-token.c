#include "mock-token.h"

csv_token *mock_token(size_t y, size_t x, char *data, csv_token *start)
{
    csv_token *token = (csv_token *)malloc(sizeof(csv_token));
    token->data = data;
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
