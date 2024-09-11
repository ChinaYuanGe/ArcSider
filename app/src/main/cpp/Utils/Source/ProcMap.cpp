#include "../Header/ProcMap.hpp"

ProcMap::ProcMap(char *content)
{
    char *lineStart = content;
    char *lineEnd = strchr(lineStart, '\n');
    // Array is {so lib's inode number, count}
    // 'count' should start in 0.
    Array<size_t *> inodeCounter;
    size_t doRowCount = 0;
    do
    {
        doRowCount++;
        PModule *Mod = (PModule *)malloc(sizeof(PModule));

        char *Endptr = nullptr;
        if (Platform::Is64Bit())
        {
            Mod->AddrStart = (void *)strtoull(lineStart, &Endptr, 16);
            Mod->AddrEnd = (void *)strtoull(strchr(lineStart, '-') + 1, &Endptr, 16);
        }
        else // Well, no one will use 16bit platform, right?
        {
            Mod->AddrStart = (void *)strtoul(lineStart, &Endptr, 16);
            Mod->AddrEnd = (void *)strtoul(strchr(lineStart, '-') + 1, &Endptr, 16);
        }

        char *PermStrOffset = strchr(lineStart, ' ') + 1;
        Mod->Perm_Read = PermStrOffset[0] == 'r';
        Mod->Perm_Write = PermStrOffset[1] == 'w';
        Mod->Perm_Execute = PermStrOffset[2] == 'x';
        Mod->Perm_Share = PermStrOffset[3] == 's';

        char *OffsetStrOffset = strchr(PermStrOffset, ' ') + 1;
        Mod->Offset = (unsigned int)strtoul(OffsetStrOffset, &Endptr, 16);

        char *DeviceStrOffset = strchr(OffsetStrOffset, ' ') + 1;
        Mod->DeviceMaster = (char)strtoul(DeviceStrOffset, &Endptr, 16);
        Mod->DeviceSlave = (char)strtoul(strchr(DeviceStrOffset, ':') + 1, &Endptr, 16);

        char *inodeStrOffset = strchr(DeviceStrOffset, ' ') + 1;
        Mod->inode = strtoull(inodeStrOffset, &Endptr, 10);

        size_t *pick = (size_t *)malloc(sizeof(size_t) * 2);
        pick[0] = Mod->inode;
        pick[1] = 0;
        size_t *lookupInodeCountData = nullptr;
        if (Mod->inode > 0)
        {
            if (inodeCounter.Size() > 0)
            {
                if (Mod->inode == inodeCounter.First()[0]) // It means exists
                {
                    inodeCounter.First()[1]++;
                    lookupInodeCountData = inodeCounter.First();
                    free(pick);
                }
                else if (Mod->inode == inodeCounter.Last()[0]) // It means exists
                {
                    inodeCounter.Last()[1]++;
                    lookupInodeCountData = inodeCounter.Last();
                    free(pick);
                }

                else if (Mod->inode < inodeCounter.First()[0])
                {
                    inodeCounter.Insert(0, &pick);
                }
                else if (Mod->inode > inodeCounter.Last()[0])
                {
                    inodeCounter.Push(&pick);
                }
                else // linear search for most matches
                {
                    /*size_t startptr = 0;
                    size_t endptr = inodeCounter.Size();
                    size_t splitsize = endptr - startptr;
                    size_t pickindex = 0;
                    do
                    {
                        splitsize = endptr - startptr;
                        pickindex = startptr + (splitsize / 2);
                        size_t *pickup = inodeCounter[pickindex];
                        if (pickup[0] == Mod->inode)
                        {
                            break;
                        }
                        else
                        {
                            if (Mod->inode < pickup[0])
                            {
                                endptr = pickindex;
                            }
                            else
                            {
                                startptr = pickindex;
                            }
                        }
                    } while (splitsize > 1);

                    if (inodeCounter[pickindex][0] == Mod->inode)
                    {
                        inodeCounter[pickindex][1]++;
                        lookupInodeCountData = inodeCounter[pickindex];
                        free(pick);
                    }
                    else
                    {
                        inodeCounter.Insert(pickindex, &pick);
                    }*/

                    size_t insertIndex = 0;
                    for (int i = 0; i < inodeCounter.Size(); i++)
                    {
                        if (inodeCounter[i][0] == Mod->inode)
                        {
                            inodeCounter[i][1]++;
                            insertIndex = 0;
                            lookupInodeCountData = inodeCounter[i];
                            break;
                        }
                        else
                        {
                            if (inodeCounter[i][0] < Mod->inode)
                            {
                                insertIndex = i + 1;
                            }
                        }
                    }
                    if (insertIndex > 0)
                    {
                        inodeCounter.Insert(insertIndex, &pick);
                    }
                    else
                    {
                        free(pick);
                    }
                }
            }
            else
            {
                inodeCounter.Push(&pick);
            }
        }
        else
        {
            free(pick);
        }
        char *NameStrOffset = strchr(inodeStrOffset, '/');
        if (NameStrOffset != nullptr && NameStrOffset < lineEnd)
        {
            size_t NameLen = lineEnd - NameStrOffset;
            if (lookupInodeCountData != nullptr)
            {
                char appendNumberStr[8];
                sprintf(appendNumberStr, ".%d", lookupInodeCountData[1]);
                appendNumberStr[7] = '\0';
                int appendNumberStrLen = strlen(appendNumberStr);

                Mod->Name = (char *)malloc(NameLen + appendNumberStrLen + 1);
                memcpy(Mod->Name, NameStrOffset, NameLen);
                memcpy((void *)(Mod->Name + NameLen), (void *)appendNumberStr, appendNumberStrLen);
                Mod->Name[NameLen + appendNumberStrLen] = '\0';
            }
            else
            {
                Mod->Name = (char *)malloc(NameLen + 1);
                memcpy(Mod->Name, NameStrOffset, NameLen);
                Mod->Name[NameLen] = '\0';
            }
        }
        else
        {
            NameStrOffset = strchr(inodeStrOffset, '[');
            if (NameStrOffset != nullptr && NameStrOffset < lineEnd)
            {
                size_t NameLen = lineEnd - NameStrOffset;
                Mod->Name = (char *)malloc(NameLen + 1);
                memcpy(Mod->Name, NameStrOffset, NameLen);
                Mod->Name[NameLen] = '\0';
            }
            else
            {
                Mod->Name = (char *)&MODULE_NAME_INVAILD;
            }
        }
        this->Modules.Push(Mod);
        free(Mod);

    } while ((lineStart = lineEnd + 1, lineEnd = strchr(lineStart, '\n'), lineEnd != nullptr));
    for (size_t i = 0; i < inodeCounter.Size(); i++)
    {
        free(inodeCounter[i]);
    }
    inodeCounter.Clear();
}
bool ProcMap::AddressAvailable(void *Address, bool Read, bool Write, bool Execute, bool Share)
{
    PModule *Mod = FindModuleByAddress(Address);
    if (Mod == nullptr)
    {
        return false;
    }
}
PModule *ProcMap::FindModuleByAddress(void *Address)
{
    if (Address < this->Modules.First().AddrStart || Address > this->Modules.Last().AddrEnd)
    {
        return nullptr;
    }

    // split search
    size_t startptr = 0;
    size_t endptr = this->Modules.Size();
    long long range = endptr - startptr;
    do
    {
        range = endptr - startptr;
        size_t pickindex = startptr + range / 2;
        // printf("pick = %d\n", pickindex);
        PModule *mod = this->Modules.GetPointer(pickindex);
        // printf("%lx-%lx %s\n", mod->AddrStart, mod->AddrEnd, mod->Name);
        if (Address >= mod->AddrStart && Address < mod->AddrEnd)
        {
            return mod;
        }
        else
        {
            if (Address > mod->AddrEnd)
            {
                startptr = pickindex;
            }
            else
            {
                endptr = pickindex;
            }
        }
    } while (range > 1);
}

Array<PModule> ProcMap::GetModuleArray()
{
    return this->Modules;
}

PModule *ProcMap::FindModuleByName(char *name)
{
    Array<PModule> mods = this->GetModuleArray();
    for (size_t i = 0; i < mods.Size(); i++)
    {
        if (strstr(mods.GetPointer(i)->Name, name) != nullptr)
            return mods.GetPointer(i);
    }
    return nullptr;
}

void ProcMap::Free()
{
    for (size_t i = 0; i < this->Modules.Size(); i++)
    {
        PModule Mod = this->Modules[i];
        if (Mod.Name != (char *)&MODULE_NAME_INVAILD && Mod.Name != nullptr)
            free(Mod.Name);
    }
    this->Modules.Clear();
}
namespace PMap
{
    ProcMap ParseFromSelf()
    {
        return ParseFromFile("/proc/self/maps");
    }
    ProcMap ParseFromFile(char *path)
    {
        int fd = open(path, O_RDONLY | O_SYNC);
        int bufferSize = 1024;
        Array<char *> ReadedContentList;
        char *buffer = nullptr;
        size_t bufferReaded = 0;
        size_t totalRead = 0;
        do
        {
            buffer = (char *)malloc(bufferSize);
            bufferReaded = read(fd, buffer, bufferSize);
            totalRead += bufferReaded;
            ReadedContentList.Push(&buffer);
        } while (bufferReaded > 0);
        char *content = (char *)malloc(totalRead + 1);
        content[totalRead] = '\0';
        for (size_t i = 0; i < totalRead; i += bufferSize)
        {
            size_t index = i / bufferSize;
            size_t takeCount = totalRead - i < bufferSize ? totalRead - i : bufferSize;
            memcpy((void *)(content + (index * bufferSize)), (void *)ReadedContentList[index], takeCount);
        }
        for (size_t i = 0; i < ReadedContentList.Size(); i++)
        {
            free(ReadedContentList[i]);
        }
        ReadedContentList.Clear();
        ProcMap pmap(content);

        // write to file debug...

        /*if (access("/data/data/moe.oly.crc/files/mapexport.txt", F_OK) != 0)
        {
            LOGD("WRITE mapexport");
            FILE *f = fopen("/data/data/moe.oly.crc/files/mapexport.txt", "w");
            fwrite(content, totalRead + 1, 1, f);
            fclose(f);
            LOGD("Dumped libarchook.so's reading /proc/maps to /data/data/moe.oly.crc/files/mapexport.txt");
        }*/

        free(content);

        close(fd);

        return pmap;
    }
}