#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>

#include <elf.h>

#include "../../BaseUtils/Header/Array.hpp"
#include "../../BaseUtils/Header/Platform.hpp"

using namespace std;

#ifndef __PROCMAP_H_
#define __PROCMAP_H_

#define MODULE_NAME_INVAILD "__NONAMEMOD__"

typedef struct
{
    void *AddrStart;
    void *AddrEnd;
    bool Perm_Read;
    bool Perm_Write;
    bool Perm_Execute;
    bool Perm_Share;
    unsigned int Offset;
    char DeviceMaster;
    char DeviceSlave;
    unsigned long long inode;
    char *Name;
} PModule;

class ProcMap
{
public:
    ProcMap(char *content); // Parse Content
    bool AddressAvailable(void *Address, bool Read = true, bool Write = false, bool Execute = false, bool Share = false);
    PModule *FindModuleByAddress(void *Address);
    PModule *FindModuleByName(char *Name);
    Array<PModule> GetModuleArray();
    void Free();

private:
    Array<PModule> Modules;
};
namespace PMap
{
    ProcMap ParseFromSelf();
    ProcMap ParseFromFile(char *path);
}

//#include "../Source/ProcMap.cpp"
#endif
