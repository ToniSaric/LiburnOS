
void *memcpy(void *dest, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *) dest;
    const unsigned char *s = (const unsigned char *) src;

    while (n--)
    {
        *d++ = *s++;
    }

    return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *) dest;
    const unsigned char *s = (const unsigned char *) src;
    if (d == s || n == 0)
    {
        return dest;
    }

    if (d < s)
    {
        while (n--)
        {
            *d++ = *s++;
        }
    }
    else
    {
        d += n;
        s += n;
        while (n--)
        {
            *--d = *--s;
        }
    }


    return dest;
}

void *memset(void *dest, int val, size_t n)
{
    unsigned char *temp = (unsigned char *) dest;
    unsigned char v = (unsigned char) val; // truncate value

    while (n--)
    {
        *temp ++ = v;
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *sc1 = (const unsigned char *) s1;
    const unsigned char *sc2 = (const unsigned char *) s2;
    int diff = 0;

    while(n--)
    {
        int diff = *sc1++ - *sc2++;
        if (diff != 0)
        {
            return diff;
        }
    }

    return 0;
}