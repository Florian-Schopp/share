#include <stdio.h>
#include <iostream>
#include "../src/ClipxApi.h"
int main(void)
{
    MHandle handle = ClipX_Connect("213.157.30.50");
    std::cout << "Connected" << handle << std::endl;
    char res[100];
    ClipX_SDORead(handle, 0x4280, 2, res, 100);
    ClipX_Disconnect(handle);
    std::cout
        << "Done" << std::endl;
    return 0;
}