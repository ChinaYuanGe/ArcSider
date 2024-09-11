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


struct OpenPathReplaceInfo
{
    char *target;
    std::function<char *()> replace;
};

struct ListOpenPathReplaceInfo{
    char **targets;
    std::function<char*()> replace;
};

static OpenPathReplaceInfo RegexReplaceList[]{
        {"img\\/bg\\/1080\\/.*.\\.jpg",[](){ //The default replacement
            return (char*)Config[CFG_DefaultReplaceBGTexture];
        }},
        {nullptr}
};

static OpenPathReplaceInfo ReplaceList[]{
        {"img/hit_pure.png", []()
                             {
                                if(Config[CFG_EnableHidePureTexture]) {
                                    return (char *) "img/empty.png";
                                }
                                else return (char *)nullptr;
                             }},
        {"img/shutter_l.png", []()
                             {
                                 return (char *)"img/shutter_l_finale.png";
                             }},
        {"img/shutter_r.png", []()
                             {
                                 return (char *)"img/shutter_r.png";
                             }},
        // Begin of bg replacement
        {"img/bg/1080/pragmatism.jpg",[](){
            return (char *)"img/bg/1080/vs_conflict.jpg";
        }},

        {"img/bg/1080/pragmatism3.jpg",[](){
            return (char *)"img/bg/1080/vs_conflict.jpg";
        }},

        {"img/bg/1080/shiawase.jpg",[](){
            return (char *)"img/bg/1080/gc_conflict.jpg";
        }},

        {"img/bg/1080/nirvluce.jpg",[](){
            return (char *)"img/bg/1080/gc_conflict.jpg";
        }},

        {"img/bg/1080/auxesia.jpg",[](){
            return (char *)"img/bg/1080/aterlbus.jpg";
        }},

        {"img/bg/1080/modelista.jpg",[](){
            return (char *)nullptr; //Ok for dark
        }},

        {"img/bg/1080/quon.jpg",[](){
            return (char *)"img/bg/1080/nijuusei-conflict-b.jpg";
        }},

        {"img/bg/1080/etherstrike.jpg",[](){
            return (char *)"img/bg/1080/finale_conflict.jpg";
        }},

        {"img/bg/1080/fractureray.jpg",[](){
            return (char *)"img/bg/1080/finale_conflict.jpg";
        }},

        {"img/bg/1080/gc_lance.jpg",[](){
            return (char *)"img/bg/1080/gc_conflict.jpg";
        }},

        {"img/bg/1080/solitarydream.jpg",[](){
            return (char *)"img/bg/1080/single2_conflict.jpg";
        }},

        {"img/bg/1080/chuni-worldvanquisher.jpg",[](){
            return (char *)"img/bg/1080/chunithmnew_conflict.jpg";
        }},

        {"img/bg/1080/ringedgenesis.jpg",[](){
            return (char *)"img/bg/1080/finale_conflict.jpg";
        }},

        {"img/bg/1080/shiawase2.jpg",[](){
            return (char *)"img/bg/1080/single_conflict.jpg";
        }},

        {"img/bg/1080/arcahv.jpg",[](){
            return (char *)"img/bg/1080/zettai.jpg";
        }},

        {"img/bg/1080/felis.jpg",[](){
            return (char *)"img/bg/1080/finale_conflict.jpg";
        }},

        {"img/bg/1080/omegafour.jpg",[](){
            return (char *)"img/bg/1080/finale_conflict.jpg";
        }},

        {"img/bg/1080/gou.jpg",[](){
            return (char *)"img/bg/1080/gc_ouroboros.jpg";
        }},

        {"img/bg/1080/aegleseeker.jpg",[](){
            return (char *)"img/bg/1080/aegleseeker_grey.jpg";
        }},

        {"img/bg/1080/tanoc_red.jpg",[](){
            return (char *)nullptr; //Ok for dark
        }},

        {"img/bg/1080/temptation.jpg",[](){
            return (char *)"img/bg/1080/maimai_boss.jpg";
        }},

        {"img/bg/1080/magnolia.jpg",[](){
            return (char *)nullptr; //Ok for dark
        }},

        {"img/bg/1080/eden_boss.jpg",[](){
            return (char *)"img/bg/1080/eden_conflict.jpg";
        }},

        {"img/bg/1080/vulcanus.jpg",[](){
            return (char *)"img/bg/1080/rotaeno_conflict.jpg";
        }},

        {"img/bg/1080/alice_light.jpg",[](){
            return (char *)"img/bg/1080/alice_conflict.jpg";
        }},

        {"img/bg/1080/base_light.jpg",[](){
            return (char *)"img/bg/1080/base_conflict.jpg";
        }},

        {"img/bg/1080/chunithmnew_light.jpg",[](){
            return (char *)"img/bg/1080/chunithmnew_conflict.jpg";
        }},

        {"img/bg/1080/cytus_light.jpg",[](){
            return (char *)"img/bg/1080/cytus_conflict.jpg";
        }},

        {"img/bg/1080/dynamix_light.jpg",[](){
            return (char *)"img/bg/1080/dynamix_conflict.jpg";
        }},

        {"img/bg/1080/eden_append_light.jpg",[](){
            return (char *)"img/bg/1080/eden_append_conflict.jpg";
        }},

        {"img/bg/1080/eden_light.jpg",[](){
            return (char *)"img/bg/1080/eden_conflict.jpg";
        }},

        {"img/bg/1080/finale_light.jpg",[](){
            return (char *)"img/bg/1080/finale_conflict.jpg";
        }},

        {"img/bg/1080/gc_light.jpg",[](){
            return (char *)"img/bg/1080/gc_conflict.jpg";
        }},

        {"img/bg/1080/hime_light.jpg",[](){
            return (char *)"img/bg/1080/hime_conflict.jpg";
        }},

        {"img/bg/1080/lanota-light.jpg",[](){
            return (char *)"img/bg/1080/lanota-conflict.jpg";
        }},

        {"img/bg/1080/maimai_light.jpg",[](){
            return (char *)"img/bg/1080/maimai_conflict.jpg";
        }},

        {"img/bg/1080/meta_mysteria.jpg",[](){
            return (char *)"img/bg/1080/wacca_conflict.jpg";
        }},

        {"img/bg/1080/mirai_light.jpg",[](){
            return (char *)"img/bg/1080/mirai_conflict.jpg";
        }},

        {"img/bg/1080/musedash_light.jpg",[](){
            return (char *)"img/bg/1080/musedash_conflict.jpg";
        }},

        {"img/bg/1080/nijuusei-light-b.jpg",[](){
            return (char *)"img/bg/1080/nijuusei-conflict-b.jpg";
        }},

        {"img/bg/1080/nijuusei2_light.jpg",[](){
            return (char *)"img/bg/1080/nijuusei2_conflict.jpg";
        }},

        {"img/bg/1080/observer_light.jpg",[](){
            return (char *)"img/bg/1080/observer_conflict.jpg";
        }},

        {"img/bg/1080/omatsuri_light.jpg",[](){
            return (char *)"img/bg/1080/omatsuri_conflict.jpg";
        }},

        {"img/bg/1080/ongeki_light.jpg",[](){
            return (char *)"img/bg/1080/ongeki_conflict.jpg";
        }},

        {"img/bg/1080/prelude_light.jpg",[](){
            return (char *)"img/bg/1080/prelude_conflict.jpg";
        }},

        {"img/bg/1080/rotaeno_light.jpg",[](){
            return (char *)"img/bg/1080/rotaeno_conflict.jpg";
        }},

        {"img/bg/1080/single_light.jpg",[](){
            return (char *)"img/bg/1080/single_conflict.jpg";
        }},

        {"img/bg/1080/single2_light.jpg",[](){
            return (char *)"img/bg/1080/single2_conflict.jpg";
        }},

        {"img/bg/1080/tanoc_light.jpg",[](){
            return (char *)"img/bg/1080/tanoc_conflict.jpg";
        }},

        {"img/bg/1080/tonesphere-solarsphere.jpg",[](){
            return (char *)"img/bg/1080/tonesphere-darksphere.jpg";
        }},

        {"img/bg/1080/touhou_light.jpg",[](){
            return (char *)"img/bg/1080/touhou_conflict.jpg";
        }},

        {"img/bg/1080/vs_light.jpg",[](){
            return (char *)"img/bg/1080/vs_conflict.jpg";
        }},

        {"img/bg/1080/wacca_light.jpg",[](){
            return (char *)"img/bg/1080/wacca_conflict.jpg";
        }},

        {"img/bg/1080/rei.jpg",[](){
            return (char *)"img/bg/1080/yugamu.jpg";
        }},

        {"img/bg/1080/zettai_light.jpg",[](){
            return (char *)"img/bg/1080/zettai.jpg";
        }},
        {"img/bg/1080/testify.jpg",[](){
            return (char *)"img/bg/1080/byd_2_conflict.jpg";
        }},

        {"img/bg/1080/epilogue.jpg",[](){
            return (char *)"img/bg/1080/finale_conflict.jpg";
        }},
        
        // Dark bg for doing nothing...
        {"img/bg/1080/base_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/sheriruth.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/single_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/dynamix_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/lethaeus.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/mirai_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/yugamu.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/axiumcrisis.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/grievouslady.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/lanota-conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/nijuusei-conflict-b.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/tonesphere-darksphere.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/tiferet.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/alexandrite.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/gc_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/gc_ouroboros.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/zettai.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/cyaegha.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/chuni-garakuta.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/chuni-ikazuchi.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/prelude_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/omatsuri_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/tanoc_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/saikyostronger.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/mirai_awakened.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/vs_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/tempestissimo.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/gc_buchigire.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/alice_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/ongeki_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/maimai_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/maimai_boss.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/chunithmthird_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/observer_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/wacca_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/wacca_boss.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/nijuusei2_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/hime_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/musedash_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/single2_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/finale_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/pentiment.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/arcanaeden.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/lamia.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/touhou_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/apophenia.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/chunithmnew_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/spidersthread.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/cytus_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/cytus_boss.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/eden_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/eden_append_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/arghena.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/nihil.jpg",[](){
            return (char *)nullptr;
        }},

        {"img/bg/1080/alterego.jpg",[](){
            return (char *)nullptr;
        }},
        {"img/bg/1080/rotaeno_conflict.jpg",[](){
            return (char *)nullptr;
        }},

        //Manual Replacment
        {"img/bg/diamond_shiawase.png",[](){
            return (char*)"img/bg/diamond.png";
        }},
        {"img/bg/diamond_dynamix_light.png",[](){
            return (char*)"img/bg/diamond.png";
        }},
        {nullptr}};

static ListOpenPathReplaceInfo ArrayReplaceList[] = {
        {TrackTextures,[](){
            return TrackTextures[Config[CFG_ReplacingTrackTextureIndx]];
        }},
        {ExtraTrackTextures,[](){
            return ExtraTrackTextures[Config[CFG_ReplacingExtraTrackTextureIndx]];
        }},
        {ArcTapModelTexture,[](){
            return ArcTapModelTexture[Config[CFG_ArcTapSide]];
        }},
        {ArcTapModelMaterial,[](){
            return ArcTapModelMaterial[Config[CFG_ArcTapSide]];
        }},
        {ArcTapModelObject,[](){
            return ArcTapModelObject[Config[CFG_ArcTapSide]];
        }},
        {NoteTextures,[](){
            return NoteTextures[Config[CFG_NoteSide]];
        }},
        {NoteHoldTexture,[](){
            return NoteHoldTexture[Config[CFG_NoteSide]];
        }},
        {NoteHoldHiTexture,[](){
            return NoteHoldHiTexture[Config[CFG_NoteSide]];
        }},
        {nullptr}
};

AAsset* ReplaceFun_AAssetManager_open(AAssetManager *mgr, const char *filename, int mode){
    if(!Config[CFG_GlobalEnable]){
        return AAssetManager_open(mgr, filename, mode);
    }

    if(!Config[CFG_FileCheckEnd]){
        if(!strcmp(filename,"img/shutter_l.png")){
            Config[CFG_FileCheckEnd] = true;
        }
        else return AAssetManager_open(mgr, filename, mode);
    }

    //Meet replacing
    for(int i=0;ReplaceList[i].target != nullptr;i++) {
        if (strcmp(filename, ReplaceList[i].target) == 0)
        {
            char *fetchResult = ReplaceList[i].replace();
            if (fetchResult != nullptr)
            {
                __android_log_print(ANDROID_LOG_INFO, __LOGTAG, "Hitted replacement: %s => %s", ReplaceList[i].target, fetchResult);
                AAsset *openResult = AAssetManager_open(mgr,fetchResult,mode);
                if(openResult != nullptr) {
                    return openResult;
                }
                else return AAssetManager_open(mgr, filename, mode);
            }
            else
                return AAssetManager_open(mgr, filename, mode);
        }
    }

    //ArrayTarget Replacing
    for(int i=0;ArrayReplaceList[i].targets != nullptr;i++) {
        for(int j=0;ArrayReplaceList[i].targets[j] != nullptr;j++){
            if(!strcmp(filename, ArrayReplaceList[i].targets[j])){
                char *fetchResult = ArrayReplaceList[i].replace();
                if(fetchResult != nullptr){
                    __android_log_print(ANDROID_LOG_INFO, __LOGTAG, "Hitted arrayReplacement: %s => %s", ArrayReplaceList[i].targets[j], fetchResult);

                    AAsset *openResult = AAssetManager_open(mgr,fetchResult,mode);
                    if(openResult != nullptr) {
                        return openResult;
                    }
                    else return AAssetManager_open(mgr, filename, mode);
                }
                else
                    return AAssetManager_open(mgr, filename, mode);
            }
        }
    }

    //Regex replacing for upper's non-matches.
    for(int i=0;RegexReplaceList[i].target != nullptr;i++) {
        regex r(RegexReplaceList[i].target);
        if (regex_match(filename,r))
        {
            char *fetchResult = RegexReplaceList[i].replace();
            if (fetchResult != nullptr)
            {
                __android_log_print(ANDROID_LOG_INFO, __LOGTAG, "Hitted replacement: %s => %s", ReplaceList[i].target, fetchResult);
                AAsset *openResult = AAssetManager_open(mgr,fetchResult,mode);
                if(openResult != nullptr) {
                    return openResult;
                }
                else return AAssetManager_open(mgr, filename, mode);
            }
            else
                return AAssetManager_open(mgr, filename, mode);
        }
    }

    return AAssetManager_open(mgr, filename, mode);
}
static char *dataPathPrefix = "/data/user/0/moe.low.arc/files/cb/active/";
FILE *hookm_fopen(const char *path, const char *mode)
{
    if(!Config[CFG_GlobalEnable]){
        return fopen(path,mode);
    }

    if(!Config[CFG_FileCheckEnd]) return fopen(path,mode);

    char filename[128];

    strcpy(filename,(path+strlen(dataPathPrefix)));
    //Meet replacing
    for(int i=0;ReplaceList[i].target != nullptr;i++) {
        if (strcmp(filename, ReplaceList[i].target) == 0)
        {
            char *fetchResult = ReplaceList[i].replace();
            if (fetchResult != nullptr)
            {
                char replaceFullPath[128];
                strcpy(replaceFullPath,dataPathPrefix);
                strcpy((replaceFullPath+strlen(dataPathPrefix)),fetchResult);
                __android_log_print(ANDROID_LOG_INFO, __LOGTAG, "Hitted fopen replacement: %s => %s", path, replaceFullPath);
                if(!access(replaceFullPath,F_OK)) {

                    return fopen(replaceFullPath, mode);
                }
                else return fopen(path,mode);
            }
            else
                return fopen(path,mode);
        }
    }

    for(int i=0;RegexReplaceList[i].target != nullptr;i++) {
        regex r(RegexReplaceList[i].target);
        if (regex_match(filename,r)){
            char *fetchResult = RegexReplaceList[i].replace();
            if (fetchResult != nullptr)
            {
                char replaceFullPath[128];
                strcpy(replaceFullPath,dataPathPrefix);
                strcpy((replaceFullPath+strlen(dataPathPrefix)),fetchResult);
                __android_log_print(ANDROID_LOG_INFO, __LOGTAG, "Hitted fopen replacement: %s => %s", path, replaceFullPath);
                if(!access(replaceFullPath,F_OK)) {

                    return fopen(replaceFullPath, mode);
                }
                else return fopen(path,mode);
            }
            else
                return fopen(path,mode);
        }
    }



    return fopen(path, mode);
}

extern "C"
JNIEXPORT void JNICALL
Java_xyz_anon_arcsider_NativeHooker_Load(JNIEnv *env, jclass thiz) {
    Hooker::HookGotPltByName("libcocos2dcpp.so", "AAssetManager_open", (void **)&ReplaceFun_AAssetManager_open);
    Hooker::HookGotPltByName("libcocos2dcpp.so","fopen",(void**)&hookm_fopen);
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