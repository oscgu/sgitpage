#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define THROW_SIZE_ERR(req)                                                   \
        {                                                                     \
                fprintf(stderr, "Dest must hold atleast '%ld' chars\n", req); \
                exit(1);                                                      \
        }

void
strnjoin(const char *first, const char *sec, char *dest, int destsize)
{
        unsigned long firstlen = strlen(first);
        unsigned long seclen = strlen(sec);

        if (firstlen + seclen + 1 > destsize) {
                THROW_SIZE_ERR(firstlen + seclen + 1);
        }

        strncpy(dest, first, destsize);
        int rest = destsize - firstlen - 1;
        strncat(dest, sec, rest);
        dest[firstlen + seclen] = '\0';
}

void
strnrpl(const char *src, const char *old, const char *new, char *dest,
        int destsize)
{
        if (destsize < (strlen(src) - strlen(old) + strlen(new) + 1)) {
                THROW_SIZE_ERR(strlen(src) - strlen(old) + strlen(new) + 1);
        }
        /*
         * Get pos of substring
         */
        char *pos = strstr(src, old);

        /*
         * Copy from source till substring addr is hit
         */
        for (; src < pos; dest++, src++) {
                *dest = *src;
        }

        /*
         * Move pointer behind old str
         */
        for (int i = 0; i < strlen(old); i++) {
                src++;
        }

        /*
         * Copy new string in
         */
        unsigned long newsize = strlen(new);
        for (int i = 0; i < newsize; i++, dest++, new ++) {
                *dest = *new;
        }

        /*
         * Copy rest in
         */
        unsigned int srclen = strlen(src);
        for (int i = 0; i < srclen; i++, dest++, src++) {
                *dest = *src;
        }
        *(++dest) = '\0';
}
