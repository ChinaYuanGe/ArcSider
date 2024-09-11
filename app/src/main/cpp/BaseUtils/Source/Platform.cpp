#include "../Header/Platform.hpp"

namespace Platform
{
    bool Is64Bit()
    {
        return sizeof(void *) == 8;
    }

    bool IsLittleEndian()
    {
        int value = 0x01;
        return *((char *)&value) == 0x01;
    }
}