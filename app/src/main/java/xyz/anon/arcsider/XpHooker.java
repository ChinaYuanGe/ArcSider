package xyz.anon.arcsider;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

import java.lang.annotation.Native;
import java.nio.charset.StandardCharsets;

import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class XpHooker implements IXposedHookLoadPackage {
    ConfigBoradcastReceiver cfgbdReceiver;
    ReceiveContentUrlReceiver ContentUrlReceiver;

    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam loadPackageParam) throws Throwable {
        Class<?> Activity = loadPackageParam.classLoader.loadClass("low.moe.AppActivity");
        java.lang.reflect.Method AcCreateM = Activity.getDeclaredMethod("onCreate", android.os.Bundle.class);
        java.lang.reflect.Method AcDestroyM = Activity.getDeclaredMethod("onDestroy");

        XposedBridge.hookMethod(AcCreateM, new XC_MethodHook() {
            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                Activity mainac = ((Activity) param.thisObject);
                Log.d("moe.oly.crc.hooker", mainac.getExternalFilesDir(null).getAbsolutePath());
                cfgbdReceiver = new ConfigBoradcastReceiver();

                mainac.getApplicationContext().registerReceiver(cfgbdReceiver,new IntentFilter("xyz.anon.arcsider.RESPONSE_SETUP"),Context.RECEIVER_EXPORTED);

                Intent bdIntent = new Intent("xyz.anon.arcsider.REQUEST_SETUP");
                bdIntent.setComponent(new ComponentName("xyz.anon.arcsider","xyz.anon.arcsider.ArcaeaRequestSetupReceiver"));
                mainac.sendBroadcast(bdIntent);

                // Register get content url receiver
                ContentUrlReceiver = new ReceiveContentUrlReceiver();
                mainac.getApplicationContext().registerReceiver(ContentUrlReceiver,new IntentFilter("xyz.anon.arcsider.RESPONSE_CURL"),Context.RECEIVER_EXPORTED);

                Intent reqCurlIntent = new Intent("xyz.anon.arcsider.REQUEST_CURL");
                //reqCurlIntent.setComponent(new ComponentName("xyz.anon.arcsider","xyz.anon.arcsider.RequestContentUrlReceiver"));
                mainac.sendBroadcast(reqCurlIntent);

                NativeHooker.Load(mainac.getDataDir().toString().getBytes(StandardCharsets.US_ASCII));

                super.afterHookedMethod(param);
            }
        });

        XposedBridge.hookMethod(AcDestroyM, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                Activity mainac = ((Activity) param.thisObject);
                //mainac.getApplicationContext().unregisterReceiver(cfgbdReceiver);
                mainac.getApplicationContext().unregisterReceiver(ContentUrlReceiver);
                super.beforeHookedMethod(param);
            }
        });
    }

    class ConfigBoradcastReceiver extends BroadcastReceiver{
        @Override
        public void onReceive(Context context, Intent intent) {
            boolean globalen = intent.getBooleanExtra("global_enable",true);
            NativeHooker.SetConfig(1,globalen ? 1 : 0);
            boolean purehidden = intent.getBooleanExtra("pure_hidden",false);
            NativeHooker.SetConfig(2,purehidden ? 1 : 0);
            NativeHooker.SetConfig(5,intent.getIntExtra("note_style",1));
            NativeHooker.SetConfig(6,intent.getIntExtra("arctap_style",1));
            NativeHooker.SetConfig(3,intent.getIntExtra("track_style",3));

            NativeHooker.ReloadReplaceTable();
        }
    }
}
