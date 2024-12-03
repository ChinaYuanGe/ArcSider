#include <android/asset_manager.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <sstream>
#include <string>
#include "../AndroidUtils/Logger.hpp"

#ifndef __ASSETPATCH_H_
#define __ASSETPATCH_H_

typedef struct{
    char *Filename = nullptr;
    char *Data = nullptr;
    off64_t Length = 0;
    off_t Pos = 0;
    AAsset *Identify = nullptr;
} AssetPatch;

typedef struct{
    char *target;
    char *replace;
} PathReplace;

void FreeAssetPatch(AssetPatch *a);
AssetPatch *CreateAssetPatch(const char *base,const char *filePath);

PathReplace **ParseReplaceContent(const char *content);
PathReplace **ParseReplaceContentFromFile(const char *path);
char *FindPathReplace(PathReplace **list, const char *lookup);

//#include "AssetPatch.cpp"
#endif