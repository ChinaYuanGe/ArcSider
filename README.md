# ArcSider

FORCE changing side texture by hooking 'AAssetsManager_open' and 'fopen' method.

Supported Android: China version & Google Play version.

## Notice

This project is written for learnning purposes, so some function might working incorrectly, thanks for your understanding!

## WARNING

While you used this project to modify Arcaea client(directly or indirectly modify), you are AGAINST the Arcaea Terms of use, Lowiro MIGHT BAN your account if they seeing your screenshot or playing video with modified texture, or some sepical detection code to identify you are modified.

This is the serious problem,so do anything at your own risk!

## Effects
Redirect the playing track, background, note's texture path to make the side inverse effect.

Every background redirection is effected by pre-defined replacing table, if no record is hitted, it will redirect to 'base_conflict.jpg' by default.

## Usage
Root: Install APK and enable in your xposed module manager.

Non-Root: (Recommend) Use LSPatch to patch Arcaeam, install the 'sider' APK.

Make sure that your device's cpu is armv8-a(64bit type), or it won't work!

## LICENSE

MIT