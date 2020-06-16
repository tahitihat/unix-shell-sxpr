#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "ivec.h"
#include "tokens.h"


typedef int (*cpred)(int); 

void
copy_next_token(char* text, char* tokn, cpred pred)
{
    int ii = 0;
    while (text[ii] && pred(text[ii])) {
        tokn[ii] = text[ii];
        ii++;
    }

    tokn[ii] = 0;
}

int
isparen(int cc)
{
    return cc == '(' || cc == ')';
}

int
isquote(int cc)
{
    return cc == '"';
}

int
istext(int cc)
{
    return !isparen(cc) && !ispunct(cc) && !isspace(cc);
}

char*
copy_quote(char* text, char* buf)
{
    int ii;
    for (ii = 0; text[ii] && !isquote(text[ii]); ++ii) {
        buf[ii] = text[ii];
    }
    buf[ii] = 0;
}

ivec*
tokens(char* text)
{
    char tok[80];
    ivec* ys = make_ivec();

    cpred preds[] = { ispunct, istext };

    int ii = 0;
    while (ii < strlen(text)) {
        if (isspace(text[ii])) {
            ii++;
            continue;
        }

        if (isparen(text[ii])) {
            tok[0] = text[ii];
            tok[1] = 0;
            ii += 1;
            add_element(ys, tok);
            continue;
        }

        if (isquote(text[ii])) {
            copy_quote(text + ii + 1, tok);
            add_element(ys, tok);
            ii += strlen(tok);
            ii += 2;
            continue;
        }

	int jj;
        for (jj = 0; jj < 2; ++jj) {
            if (preds[jj](text[ii])) {
                copy_next_token(text + ii, tok, preds[jj]);
                add_element(ys, tok);
                break;
            }
        }

        ii += strlen(tok);
    }

    return ys;
}

