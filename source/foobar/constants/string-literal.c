#include <assert.h>

#define NAME "foo"
#define EXT  ".bar"

static const char* name = "bar";

int main(void)
{
    char buf1[] = "bar";
    char buf2[10] = "foobar";
    char buf3[4] = "foobar";
    const char *name1 = "foobar";
    const char *name2 = "foobar";

    /* Which assertion will fail? */
    assert(sizeof(NAME) == 4);
    assert(sizeof(buf1) == 4);
    assert(sizeof(buf2) == 10);
    assert(sizeof(buf3) == 4);
    assert(name1 == name2);
    assert(sizeof(name) == 4);
    assert(sizeof(EXT) == 4);

    return 0;
}
