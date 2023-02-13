#include <stdio.h>
#include <iostream>
#include "../src/lib/ClipX.hpp"
int main(void)
{
    ClipX *dev = new ClipX();
    std::cout << "Connecting" << std::endl;
    dev->Connect("213.157.30.50");
    dev->Disconnect();
    std::cout << "Done" << std::endl;
    return 0;
}