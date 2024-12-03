package xyz.anon.arcsider;

public class NativeHooker {
    static{
        System.loadLibrary("arcsider");
    }
    public native static void Load(byte[] c_str);
    public native static void SetConfig(int indx,long value);
    public native static void ReloadReplaceTable();
}
