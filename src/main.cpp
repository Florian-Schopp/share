#include <stdio.h>
#include "ClipxApi.h"

int main(void)
{
    puts("This is a shared library test...");
    ClipX_Connect("");
    return 0;
}