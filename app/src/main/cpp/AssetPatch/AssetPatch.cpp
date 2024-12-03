#include "AssetPatch.h"

using namespace std;

void FreeAssetPatch(AssetPatch *a){
    free(a->Data);
    free(a);
}

AssetPatch *CreateAssetPatch(const char *base,const char *filePath) {
    size_t baseLen = strlen(base);
    size_t filePathLen = strlen(filePath);
    char absPath[256];
    memcpy(absPath,base,baseLen);
    memcpy(absPath+baseLen,filePath,filePathLen);
    absPath[baseLen + filePathLen] = '\0';

    LOGD((string("TargetPath=") + string(absPath)).c_str());

    if(!access(absPath,O_RDONLY)){
        FILE *file = fopen(absPath,"r");
        if(file != NULL) {
            // Get file length
            AssetPatch *ret = (AssetPatch *) malloc(sizeof(AssetPatch));
            ret->Filename = (char*)malloc(strlen(filePath) + 1);
            strcpy(ret->Filename,filePath);
            ret->Filename[strlen(filePath)] = '\0';
            if(!fseek(file,0,SEEK_END)) {
                ret->Length = ftell(file);

                if (ret->Length >= 0) {
                    ret->Data = (char *) malloc(ret->Length);
                    fseek(file, 0, SEEK_SET);
                    if (fread(ret->Data, ret->Length, 1, file)) {
                        fclose(file);
                        return ret;
                    }
                }
            }
            // If not successfully, just free and return nullptr.
            FreeAssetPatch(ret);
            fclose(file);
        }
    }
    return nullptr;
}

PathReplace **ParseReplaceContent(const char *content){
    vector<PathReplace *> list;

    std::string c(content);
    c.erase(std::remove(c.begin(), c.end(),'\r'),c.end());
    c.erase(std::remove(c.begin(), c.end(),'\n'),c.end());

    std::stringstream ss(c);

    string line;
    while(std::getline(ss,line,';')){
        char operation = line[0];
        if(operation == '#') continue; //comment should ignore.
        line.erase(0,2);

        vector<string> params;

        stringstream linestream(line);
        string argFeed;

        while(std::getline(linestream,argFeed,',')){
            params.push_back(string(argFeed));
        }

        switch(operation){
            case '0':{
                PathReplace *item = (PathReplace *)malloc(sizeof(PathReplace));

                item->target = (char *)malloc(params.data()[0].size() + 1);
                strcpy(item->target,params.data()[0].c_str());
                item->target[params.data()[0].size()] = '\0';

                item->replace = (char *)malloc(params.data()[1].size() + 1);
                strcpy(item->replace,params.data()[1].c_str());
                item->replace[params.data()[1].size()] = '\0';
                list.push_back(item);
            }
        }
    }

    PathReplace **ret = (PathReplace **)malloc(sizeof(PathReplace*) * (list.size() + 1));

    for(int i=0;i<list.size();i++){
        ret[i] = list[i];
    }
    ret[list.size()] = nullptr;

    return ret;
}

PathReplace **ParseReplaceContentFromFile(const char *path){
    if(!access(path,O_RDONLY)){
        FILE *file = fopen(path,"r");
        if(file != NULL) {
            fseek(file,0,SEEK_END);
            size_t fsize = ftell(file);
            fseek(file,0,SEEK_SET);

            char *content = (char*)malloc(fsize + 1);
            content[fsize] = '\0';
            if(fread(content,fsize,1,file)) {
                auto ret = ParseReplaceContent(content);

                free(content);
                fclose(file);
                return ret;
            }
            free(content);
            fclose(file);
        }
    }
    return nullptr;
}

char *FindPathReplace(PathReplace **list, const char *lookup){
    for(int i=0;list[i] != nullptr;i++) {
        if(!strcmp(list[i]->target,lookup)) return list[i]->replace;
    }
    return nullptr;
}