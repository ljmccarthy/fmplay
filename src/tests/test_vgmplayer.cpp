#include <iostream>
#include "vgmplayer.hpp"

int main()
{
    {
        VgmPlayer player("test.vgm");
    }
    std::cout << "OK" << std::endl;
    return 0;
}
