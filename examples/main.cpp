#include <stdio.h>
#include <iostream>
#include "../src/ClipxApi.h"
int main(void)
{
    MHandle handle = ClipX_Connect("213.157.30.50");
    ClipX_Disconnect(handle);
    std::cout
        << "Done" << std::endl;
    return 0;
}