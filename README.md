# ArcSider

Redirecting texture path by hooking 'AAssetsManager_open' and 'fopen' method.

Override texture byte data by hooking 'AAsset_read'.

Supported Android: China version & Google Play version.

## WARNING

While you used this project to modify Arcaea client(directly or indirectly modify), you are AGAINST the Arcaea Terms of use, Lowiro MIGHT BAN your account if they seeing your screenshot or playing video with modified texture, or some sepical detection code to identify you are modified.

## Notice

This project is written for learnning purposes, so some function might working incorrectly, thanks for your understanding!

This is the serious problem,so do anything at your own risk!

## Effects
Redirect or override the playing track, background, note's texture path to make the side inverse effect depends on replacing package defintion.

Most of game update won't break this modules, Except contains texture update or somethings that not defined in replacing package.

## Usage
Root: Install APK and enable in your xposed module manager.

Non-Root: (Recommend) Use LSPatch to patch Arcaea, and install the 'sider' APK.

Make sure that your device's cpu is armv8-a(64bit type), otherwise it won't work!

## LICENSE

MIT