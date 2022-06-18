#define ARR_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

int strnjoin(const char *first, const char *sec, char *dest, int destsize);
void strnrpl(const char *src, const char *old, const char *new, char *dest,
             int destsize);
