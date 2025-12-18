#include <stdio.h>
#include "get_element.h"

int main(void) {
    for (int i = 0; i < 5; i++) {
        char *s = getElement(i);
        if (s != NULL)
            printf("Element %d: %s\n", i, s);
    }
    return 0;
}
