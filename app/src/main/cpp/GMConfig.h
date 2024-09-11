//
// Created by china on 2024/9/10.
//

#include "BaseUtils/Header/Array.hpp"

#ifndef ARCSIDER_GMCONFIG_H
#define ARCSIDER_GMCONFIG_H

#define TrackLightIndx 2

static char* TrackTextures[] ={
        "img/track.png",
        "img/track_colorless.png",
        "img/track_rei.png",
        "img/track_dark.png",
        "img/track_arcana.png",
        "img/track_black.png",
        "img/track_dark_nijuusei.png",
        "img/track_dark_vs.png",
        "img/track_finale.png",
        "img/track_pentiment.png",
        "img/track_tempestissimo.png",
        nullptr
};

static char* ExtraTrackTextures[] = {
        "img/track_extralane_light.png",
        "img/track_extralane_dark.png",
        nullptr
};

static char* NoteTextures[] ={
        "img/note.png",
        "img/note_dark.png",
        nullptr
};

static char* NoteHoldTexture[]={
        "img/note_hold.png",
        "img/note_hold_dark.png",
        nullptr
};

static char* NoteHoldHiTexture[]={
        "img/note_hold_hi.png",
        "img/note_hold_dark_hi.png",
        nullptr
};

static char* ArcTapModelTexture[]={
        "models/tap_l.png",
        "models/tap_d.png",
        nullptr
};

static char* ArcTapModelObject[]={
        "models/tap_l.obj",
        "models/tap_d.obj",
        nullptr
};
static char* ArcTapModelMaterial[]={
        "models/tap_l.mtl",
        "models/tap_d.mtl",
        nullptr
};

static unsigned long long Config[] = {
#define CFG_FileCheckEnd 0
        false,

#define CFG_GlobalEnable 1
        true,

#define CFG_EnableHidePureTexture 2
        true,

#define CFG_ReplacingTrackTextureIndx 3
        4,

#define CFG_ReplacingExtraTrackTextureIndx 4
        1,

#define CFG_NoteSide 5
        1,

#define CFG_ArcTapSide 6
        1,

#define CFG_DefaultReplaceBGTexture 7
        (unsigned long long)"img/bg/1080/base_conflict.jpg"

};

#endif //ARCSIDER_GMCONFIG_H
