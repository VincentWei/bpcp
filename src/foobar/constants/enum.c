#include <stddef.h>

typedef enum {
    IDM_FOO = 0,
    IDM_BAR,
    IDM_BAZ,
    IDM_QUX,
    IDM_FOOBAR,
} method_k;

const char* get_method_name(method_k method)
{
    switch (method) {
    case IDM_FOO:
        return "foo";
    case IDM_BAR:
        return "bar";
    case IDM_BAZ:
        return "baz";
    case IDM_QUX:
        return "qux";
    }

    return NULL;
}
