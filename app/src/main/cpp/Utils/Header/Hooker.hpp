#include <iostream>
#include <sys/mman.h>
#include <elf.h>
#include "ProcMap.hpp"

#ifndef __HOOKER_HPP_
#define __HOOKER_HPP_

#define PAGE_SIZE getpagesize()
#define PAGE_MASK (~(PAGE_SIZE - 1))
#define PAGE_START(addr) ((addr) & PAGE_MASK)
#define PAGE_END(addr) (PAGE_START(addr) + PAGE_SIZE)

namespace Hooker
{
    /*
        Replace current address in got table,
     */
    void PLTTest(char *modulename);
    void *HookGotPltByName(char *modulename, char *funcName, void **replace);
    void *HookGotPlt(char *modulename, uint64_t funcOffset, void **replace);
    void *HookGotPlt(uint64_t addr, void **replace);
    void set_mprotect(uint64_t addr, int value);
    void copymem(void *dest, void *src, size_t len);
}
#endif