#include "../Header/Hooker.hpp"

namespace Hooker
{
    /*
        Replace current address in got table,
     */

    void PLTTest(char *modulename)
    {
        ProcMap map = PMap::ParseFromSelf();
        PModule *mod = map.FindModuleByName(modulename);
        if (mod != nullptr)
        {
            FILE *elfFile = fopen(mod->Name, "r");
            if (elfFile != nullptr)
            {
                elf64_hdr elfHeader;
                fseek(elfFile, 0, SEEK_SET);
                fread((void *)&elfHeader, sizeof(elf64_hdr), 1UL, elfFile);

                elf64_shdr *segmentHList = (elf64_shdr *)malloc(sizeof(elf64_shdr) * elfHeader.e_shnum);
                fseek(elfFile, (long)elfHeader.e_shoff, SEEK_SET);
                fread((void *)segmentHList, sizeof(elf64_shdr), elfHeader.e_shnum, elfFile);

                elf64_shdr *ststrtab = nullptr;

                for (size_t i = 1; i <= elfHeader.e_shnum; i++)
                {
                    if (segmentHList[i].sh_type == SHT_STRTAB && segmentHList[i].sh_addr == 0UL)
                    {
                        ststrtab = &(segmentHList[i]);
                        break;
                    }
                }

                if (ststrtab == nullptr)
                {
                    LOGE("No .ststrtab segment located");
                    return;
                }

                for (size_t i = 0; i < elfHeader.e_shnum; i++)
                {
                    elf64_shdr segInfo = segmentHList[i];
                    const char segName[32] = {};

                    fseek(elfFile, (long)(ststrtab->sh_offset + segInfo.sh_name), SEEK_SET);
                    fread((void *)&segName, sizeof(char), 32, elfFile);

                    __android_log_print(ANDROID_LOG_INFO, __LOGTAG, "Seg:%s, VirAddr=%lx, Off=%lx, Size=%lx",
                                        segName, segInfo.sh_addr, segInfo.sh_offset, segInfo.sh_size);
                }

                free(segmentHList);
            }
            fclose(elfFile);
        }
        else
        {
            LOGE("Unable to find loaded module.");
        }
    }

    void *HookGotPltByName(char *modulename, char *funcName, void **replace)
    {
        void *originFun = nullptr;
        ProcMap map = PMap::ParseFromSelf();
        PModule *mod = map.FindModuleByName(modulename);
        if (mod != nullptr)
        {
            //__android_log_print(ANDROID_LOG_INFO, __LOGTAG, "Seeked module Path:%s", mod->Name);
            FILE *elfFile = fopen(mod->Name, "r");
            if (elfFile != NULL)
            {
                elf64_hdr elfHeader;
                fseek(elfFile, 0, SEEK_SET);
                fread((void *)&elfHeader, sizeof(elf64_hdr), 1UL, elfFile);
                // Logger::I("e_shnum:%d", (unsigned int)elfHeader.e_shnum);
                elf64_shdr *segmentHList = (elf64_shdr *)malloc(sizeof(elf64_shdr) * elfHeader.e_shnum);
                fseek(elfFile, (long)elfHeader.e_shoff, SEEK_SET);
                fread((void *)segmentHList, sizeof(elf64_shdr), elfHeader.e_shnum, elfFile);

                elf64_shdr *ststrtab = nullptr;

                for (size_t i = 1; i <= elfHeader.e_shnum; i++)
                {
                    if (segmentHList[i].sh_type == SHT_STRTAB && segmentHList[i].sh_addr == 0UL)
                    {
                        ststrtab = &(segmentHList[i]);
                        break;
                    }
                }

                if (ststrtab == nullptr) // .ststrtab not found
                {
                    return nullptr;
                }

                Elf64_Off strtabStart = ststrtab->sh_offset;

                elf64_shdr *segment_relaplt = nullptr;
                elf64_shdr *segment_dynsym = nullptr;
                elf64_shdr *segment_dynstr = nullptr;

                for (size_t i = 0; i < elfHeader.e_shnum; i++)
                {
                    elf64_shdr segInfo = segmentHList[i];
                    char segName[32] = {0x00};

                    //__android_log_print(ANDROID_LOG_INFO, __LOGTAG, "Target Pick:%lx", (long)(ststrtab->sh_offset + segInfo.sh_name));

                    fseek(elfFile, (long)(ststrtab->sh_offset + segInfo.sh_name), SEEK_SET);
                    fread((void *)&segName, sizeof(char), 32, elfFile);

                    //__android_log_print(ANDROID_LOG_INFO, __LOGTAG, "Readed segName:%s", segName);

                    if (!strcmp(segName, ".rela.plt"))
                        segment_relaplt = &(segmentHList[i]);
                    else if (!strcmp(segName, ".dynsym"))
                        segment_dynsym = &(segmentHList[i]);
                    else if (!strcmp(segName, ".dynstr"))
                        segment_dynstr = &(segmentHList[i]);
                }

                if (segment_relaplt == nullptr || segment_dynsym == nullptr || segment_dynstr == nullptr)
                {
                    Logger::E("Unable to locate needed segments. returning");
                    return nullptr;
                }

                // Start hooking

                elf64_sym *symbolBase = (elf64_sym *)((uint64_t)mod->AddrStart + segment_dynsym->sh_addr);
                char *dynstrBase = (char *)((uint64_t)mod->AddrStart + segment_dynstr->sh_addr);

                uint64_t *gotAddr = nullptr;

                //__android_log_print(ANDROID_LOG_INFO, __LOGTAG, ".rela.plt datacount = %ld", (segment_relaplt->sh_size / sizeof(elf64_rela)));

                for (size_t i = 0; i < (segment_relaplt->sh_size / sizeof(elf64_rela)); i++)
                {
                    // LOGD("TEST");
                    //__android_log_print(ANDROID_LOG_INFO, __LOGTAG, "indx=%ld", i);

                    //__android_log_print(ANDROID_LOG_INFO, __LOGTAG, "targetOff=%lx", segment_relaplt->sh_offset + (i * sizeof(elf64_rela)));
                    elf64_rela redirectInf = *((elf64_rela *)((uint64_t)mod->AddrStart + segment_relaplt->sh_offset + (i * sizeof(elf64_rela))));
                    size_t symbolIndx = (redirectInf.r_info >> 32);
                    // LOGD("SymIndx");
                    if (symbolIndx > 0)
                    {
                        // LOGD("BeginLookupGotAddr");
                        elf64_sym symbolInfo = symbolBase[symbolIndx];

                        char *stName = (dynstrBase + symbolInfo.st_name);
                        //__android_log_print(ANDROID_LOG_INFO, __LOGTAG, "Pickup symbol:%s [symbolAddr=%ld]", stName, symbolIndx);
                        if (!strcmp(funcName, stName)) // Result Matched, Set got address to hook
                        {
                            // LOGD("MATCH!");
                            gotAddr = (uint64_t *)((uint64_t)mod->AddrStart + (uint64_t)(redirectInf.r_offset));
                            break;
                        }
                    }
                }
                // LOGD("TEST2");
                if (gotAddr != nullptr)
                {
                    mprotect((void *)PAGE_START((uint64_t)gotAddr), PAGE_SIZE, PROT_READ | PROT_WRITE);
                    //__android_log_print(ANDROID_LOG_WARN, __LOGTAG, "GOT Pick:%lx", gotAddr);
                    // mprotect((void *)PAGE_START((uint64_t)gotAddr), PAGE_SIZE, PROT_READ | PROT_WRITE);
                    originFun = (void *)*gotAddr;

                    //__android_log_print(ANDROID_LOG_WARN, __LOGTAG, "GOT replace:%lx", replace);
                    *gotAddr = (uint64_t)replace;
                    //__android_log_print(ANDROID_LOG_INFO, __LOGTAG, "Got Hooked target: %s => %lx", funcName, replace);
                }
                else
                {
                    LOGE("Failed to get Got address, ignore hooking.");
                }
                free(segmentHList);
            }
            fclose(elfFile);
            map.Free();
            return originFun;
        }
        else
            return nullptr;
    }
    void *HookGotPlt(char *modulename, uint64_t funcOffset, void **replace)
    {
        ProcMap map = PMap::ParseFromSelf();
        PModule *mod = map.FindModuleByName(modulename);
        if (mod != nullptr)
        {
            uint64_t addr = (uint64_t)mod->AddrStart + funcOffset;

            mprotect((void *)PAGE_START(addr), PAGE_SIZE, PROT_READ | PROT_WRITE);

            // replace the function address
            void *origin = *(void **)addr;
            *(void ***)addr = replace;

            // clear instruction cache
            //__builtin___clear_cache((void *)PAGE_START(addr), (void *)PAGE_END(addr));

            map.Free();
            return origin;
        }
        else
        {
            map.Free();
            return nullptr;
        }
    }
    void *HookGotPlt(uint64_t addr, void **replace)
    {
        __android_log_print(ANDROID_LOG_WARN, __LOGTAG, "target address = %lx", addr);
        // LOGI("mprotect setup.");
        mprotect((void *)PAGE_START(addr), PAGE_SIZE, PROT_READ | PROT_WRITE);
        // LOGI("START HOOK.");
        void *origin = *(void **)addr;
        //__android_log_print(ANDROID_LOG_WARN, __LOGTAG, "origin function address = %lx", origin);
        //__android_log_print(ANDROID_LOG_WARN, __LOGTAG, "replacing: %lx", replace);
        // replace the function address
        *(void ***)addr = replace;
        // LOGI("END HOOK.");
        return origin;
    }
    void set_mprotect(uint64_t addr, int value)
    {
        mprotect((void *)PAGE_START(addr), PAGE_SIZE, value);
    }

    void copymem(void *dest, void *src, size_t len)
    {
        for (size_t i = 0; i < len; i++)
        {
            ((char *)dest)[i] = ((char *)src)[i];
        }
    }
}