#include <jni.h>
#include "Utils/Header/ProcMap.hpp"
#include <sys/mman.h>
#include <dlfcn.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <regex>

#include "AndroidUtils/Logger.hpp"
#include "Utils/Header/Hooker.hpp"
#include "GMConfig.h"
#include "AssetPatch/AssetPatch.h"

static bool fopenReplaceActivated = false;

//static char *AssetSearchBase = "/data/user/0/moe.low.arc/asset_patch/root/";
//static char *bundlePath = "/data/user/0/moe.low.arc/files/cb/active/";
//static char *PathReplaceListFile = "/data/user/0/moe.low.arc/asset_patch/path.txt";
const char *AssetSearchBaseRelPath = "/asset_patch/root/";
static char AssetSearchBase[128];

const char *bundlePathRelPath = "/files/cb/active/";
static char bundlePath[128];

const char *PathReplaceListFileRelPath = "/asset_patch/path.txt";
static char PathReplaceListFile[128];

PathReplace **PathReplaceList;

vector<AssetPatch *> AssetPatchList;
AssetPatch* FindPatch(AAsset *id){
    for(int i=0;i<AssetPatchList.size();i++){
        if(AssetPatchList[i]->Identify == id) return AssetPatchList[i];
    }
    return nullptr;
}
AssetPatch* FindPatch(const char *file){
    for(int i=0;i<AssetPatchList.size();i++){
        if(!strcmp(AssetPatchList[i]->Filename,file)) return AssetPatchList[i];
    }
    return nullptr;
}

AAsset* rAAssetManager_open(AAssetManager *mgr, const char *filename, int mode) {
    // Global Enable gater
    if (!Config[CFG_GlobalEnable]) return AAssetManager_open(mgr,filename,mode);

    // The first trigger for fopen match, which cannot effect bundle self checking.
    if (!fopenReplaceActivated && !strcmp(filename, "img/shutter_r.png"))
        fopenReplaceActivated = true;

    //return AAssetManager_open(mgr,filename,mode);
    //LOGD((string("AAssetManager opened: ") + string(filename)).c_str());

    // Override always first.
    AssetPatch *seek = FindPatch(filename);
    if (seek == nullptr) {
        seek = CreateAssetPatch(AssetSearchBase, filename);
        if(seek != nullptr) {
            AssetPatchList.push_back(seek);
        }
    }
    if (seek != nullptr){// If Still nullptr, give up data override.
        seek->Pos = 0;
        seek->Identify = AAssetManager_open(mgr,filename,mode);
        return seek->Identify;
    }

    if (PathReplaceList != nullptr) {
        char *Replace = FindPathReplace(PathReplaceList, filename);
        if (Replace != nullptr) {
            return AAssetManager_open(mgr, Replace, mode);
        }
    }

    // Nothing to do, return origin.
    return AAssetManager_open(mgr, filename, mode);
}
off_t rAAsset_getLength(AAsset* asset){
    if(!Config[CFG_GlobalEnable]) return AAsset_getLength(asset);

    AssetPatch *seeked = FindPatch(asset);
    if(seeked != nullptr){
        return seeked->Length;
    }
    else return AAsset_getLength(asset);
}

int rAAsset_read(AAsset* asset, void* buf, size_t count){
    if(!Config[CFG_GlobalEnable]) return AAsset_read(asset,buf,count);

    AssetPatch *seeked = FindPatch(asset);
    if(seeked != nullptr){
        if(seeked->Pos + 1 >= (seeked->Length)) {
            return 0;
        }
        else {
            off_t beforeReadPos = seeked->Pos;
            seeked->Pos =
                    (seeked->Pos + count) > seeked->Length ? seeked->Length - 1 : seeked->Pos +
                                                                                  count;
            int realReadCount = seeked->Pos - beforeReadPos;
            memcpy(buf, seeked->Data + beforeReadPos, realReadCount);
            return realReadCount;
        }
    }
    else return AAsset_read(asset,buf,count);
}
void rAAsset_close(AAsset* asset){
    AssetPatch *seeked = FindPatch(asset);
    if(seeked != nullptr){
        seeked->Pos = 0;
        seeked->Identify = nullptr;
    }
    AAsset_close(asset);
}

FILE *rfopen(const char *path, const char *mode)
{
    if(!Config[CFG_GlobalEnable]) return fopen(path,mode);

    //return ofopen(path,mode);
    //LOGD((string("fopen CURRENT checking path=")+string(path)).c_str());
    if(fopenReplaceActivated && strstr(path,bundlePath) != NULL) {
        char *shortPath = ((char *) path + strlen(bundlePath));
        size_t PathLen = strlen(AssetSearchBase) + strlen(shortPath) + 1;
        char ReplacingPath[256];
        ReplacingPath[PathLen] = '\0';
        strcpy(ReplacingPath, AssetSearchBase);
        strcpy(ReplacingPath + strlen(AssetSearchBase), shortPath);
        LOGD((string("fopen CURRENT checking path=")+string(path)).c_str());
        LOGD((string("fopen REPLACING path=")+string(ReplacingPath)).c_str());
        if (!access(ReplacingPath, F_OK)) { // File overriden replace always first.
            LOGD("path matching Replace Hit!");
            FILE *open = fopen(ReplacingPath, mode);
            if(open != NULL) {
                return open;
            }
            else fclose(open);
        }
        else if(PathReplaceList != nullptr){ // Path matching replace as secondary.
            char *search = FindPathReplace(PathReplaceList, shortPath);
            if(search != nullptr){
                char bundleReplacePath[256];
                strcpy(bundleReplacePath,bundlePath);
                strcpy(bundleReplacePath + strlen(bundlePath),search);
                bundleReplacePath[(strlen(bundlePath) + strlen(search))] = '\0';
                LOGD((string("fopen bundle REPLACING path=")+string(bundleReplacePath)).c_str());
                FILE *open = fopen(bundleReplacePath,mode);
                if(open != NULL) return open;
                else {
                    LOGD("Unable to open file.");
                    LOGD((string("errno = ")+to_string(errno)).c_str());
                }
            }
            else{
                LOGD("Unable to find replacement");
            }
        }
    }
    return fopen(path,mode);
}

void ReloadPathReplace(){
    vector<PathReplace *> total;

    PathReplace **mainReplace = nullptr;
    if(!access(PathReplaceListFile,F_OK)){
        /* From path.txt */
        mainReplace = ParseReplaceContentFromFile(PathReplaceListFile);
        for(int i=0;mainReplace[i] != nullptr;i++) {
            total.push_back(mainReplace[i]);
        }
    }

    /* Config selection. */
    if(Config[CFG_EnableHidePureTexture]){
        PathReplace *p = (PathReplace *)malloc(sizeof(PathReplace));
        p->target = PureTexture;
        p->replace = EmptyTexture;
        total.push_back(p);
    }
    if(Config[CFG_ReplacingExtraTrackTextureIndx] >= 0) {
        for(int i=0;ExtraTrackTextures[i] != nullptr;i++){
            PathReplace *p = (PathReplace *)malloc(sizeof(PathReplace));
            p->target = ExtraTrackTextures[i];
            p->replace = ExtraTrackTextures[Config[CFG_ReplacingExtraTrackTextureIndx]];
            total.push_back(p);
        }
    }
    if(Config[CFG_ReplacingTrackTextureIndx] >= 0){
        for(int i=0;TrackTextures[i] != nullptr;i++){
            PathReplace *p = (PathReplace *)malloc(sizeof(PathReplace));
            p->target = TrackTextures[i];
            p->replace = TrackTextures[Config[CFG_ReplacingTrackTextureIndx]];
            total.push_back(p);
        }
    }
    if(Config[CFG_NoteSide] >= 0){
        for(int i=0;NoteTextures[i] != nullptr;i++){
            PathReplace *p = (PathReplace *)malloc(sizeof(PathReplace));
            p->target = NoteTextures[i];
            p->replace = NoteTextures[Config[CFG_NoteSide]];
            total.push_back(p);
        }
        for(int i=0;NoteHoldTexture[i] != nullptr;i++){
            PathReplace *p = (PathReplace *)malloc(sizeof(PathReplace));
            p->target = NoteHoldTexture[i];
            p->replace = NoteHoldTexture[Config[CFG_NoteSide]];
            total.push_back(p);
        }
        for(int i=0;NoteHoldHiTexture[i] != nullptr;i++){
            PathReplace *p = (PathReplace *)malloc(sizeof(PathReplace));
            p->target = NoteHoldHiTexture[i];
            p->replace = NoteHoldHiTexture[Config[CFG_NoteSide]];
            total.push_back(p);
        }
    }
    if(Config[CFG_ArcTapSide] >= 0){
        for(int i=0;ArcTapModelTexture[i] != nullptr;i++){
            PathReplace *p = (PathReplace *)malloc(sizeof(PathReplace));
            p->target = ArcTapModelTexture[i];
            p->replace = ArcTapModelTexture[Config[CFG_ArcTapSide]];
            total.push_back(p);
        }
        for(int i=0;ArcTapModelObject[i] != nullptr;i++){
            PathReplace *p = (PathReplace *)malloc(sizeof(PathReplace));
            p->target = ArcTapModelObject[i];
            p->replace = ArcTapModelObject[Config[CFG_ArcTapSide]];
            total.push_back(p);
        }
        for(int i=0;ArcTapModelMaterial[i] != nullptr;i++){
            PathReplace *p = (PathReplace *)malloc(sizeof(PathReplace));
            p->target = ArcTapModelMaterial[i];
            p->replace = ArcTapModelMaterial[Config[CFG_ArcTapSide]];
            total.push_back(p);
        }
    }

    /* Build up */
    PathReplace **newEntry = (PathReplace **)malloc(sizeof(PathReplace**) * (total.size() + 1));
    for(int i=0;i<total.size();i++){
        newEntry[i] = total[i];
    }
    newEntry[total.size()] = nullptr;
    PathReplaceList = newEntry;

    /* Free extra memory usage */
    if(mainReplace != nullptr) free(mainReplace);
}

extern "C"
JNIEXPORT void JNICALL
Java_xyz_anon_arcsider_NativeHooker_Load(JNIEnv *env, jclass thiz, jbyteArray c_str) {
    LOGD("Begin Loadout");

    size_t len = env->GetArrayLength(c_str);
    jbyte *elem = env->GetByteArrayElements(c_str, 0);

    memcpy(PathReplaceListFile,elem,len);
    strcpy(PathReplaceListFile + len, PathReplaceListFileRelPath);
    PathReplaceListFile[len + strlen(PathReplaceListFileRelPath)] = '\0';

    memcpy(bundlePath, elem, len);
    strcpy(bundlePath + len, bundlePathRelPath);
    bundlePath[len + strlen(bundlePathRelPath)] = '\0';

    memcpy(AssetSearchBase, elem, len);
    strcpy(AssetSearchBase + len, AssetSearchBaseRelPath);
    AssetSearchBase[len + strlen(AssetSearchBaseRelPath)] = '\0';

    env->ReleaseByteArrayElements(c_str, elem, NULL);
    LOGI("Directory path init fin.");

    LOGI((string("PathReplaceListFile = ") + string(PathReplaceListFile)).c_str());
    LOGI((string("bundlePath = ") + string(bundlePath)).c_str());
    LOGI((string("AssetSearchBase = ") + string(AssetSearchBase)).c_str());

    ReloadPathReplace();

    /*for(int i=0;PathReplaceList[i] != nullptr;i++){
        LOGD(PathReplaceList[i]->target);
        LOGD(PathReplaceList[i]->replace);
        LOGD("--------");
    }*/

    Hooker::HookGotPltByName("libcocos2dcpp.so", "AAssetManager_open", (void **)&rAAssetManager_open);
    Hooker::HookGotPltByName("libcocos2dcpp.so", "AAsset_getLength", (void **)&rAAsset_getLength);
    Hooker::HookGotPltByName("libcocos2dcpp.so", "AAsset_read", (void **)&rAAsset_read);
    Hooker::HookGotPltByName("libcocos2dcpp.so", "AAsset_close", (void **)&rAAsset_close);
    Hooker::HookGotPltByName("libcocos2dcpp.so", "fopen", (void**)&rfopen);
    LOGI("SUCCESSFUL Hook all required function.");
    LOGI("Loadout finish.");
}
extern "C"
JNIEXPORT void JNICALL
Java_xyz_anon_arcsider_NativeHooker_SetConfig(JNIEnv *env, jclass clazz, jint indx, jlong value) {
    Config[indx] = value;
    switch (indx) {
        case CFG_ReplacingTrackTextureIndx:
            if (value > TrackLightIndx) {
                Config[CFG_ReplacingExtraTrackTextureIndx] = 1;
            } else {
                Config[CFG_ReplacingExtraTrackTextureIndx] = 0;
            }
            break;
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_xyz_anon_arcsider_NativeHooker_ReloadReplaceTable(JNIEnv *env, jclass clazz) {
    ReloadPathReplace();
}