package xyz.anon.arcsider;

public class NativeHooker {
    static{
        System.loadLibrary("arcsider");
    }

    public native static void Load();
    public native static void SetConfig(int indx,long value);
}
