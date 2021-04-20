#include "epsilon.h"
#include <stdio.h>
#include <stdint.h>

int main() {
    char *str = "Foo";
    uint32_t hash = FNV1a_hash(str, sizeof(str));
    printf("FNV1a_hash(\"%s\") -> %u.\n", str, hash);
    return 0;
}
