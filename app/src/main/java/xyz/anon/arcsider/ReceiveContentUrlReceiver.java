package xyz.anon.arcsider;

import android.app.Application;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Process;
import android.util.Log;
import android.widget.Toast;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContract;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.ActivityOptionsCompat;
import androidx.core.content.FileProvider;

import java.io.*;
import java.util.Dictionary;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Map;

import javax.security.auth.login.LoginException;

public class ReceiveContentUrlReceiver extends BroadcastReceiver {
    static Map<String,String> lang = new HashMap<String,String>();
    @Override
    public void onReceive(Context context, Intent intent) {
        lang.put("en","[Sider] Sync completed, shut in 3 second.");
        lang.put("zh","[Sider] 同步完毕,将在 3 秒后关闭.");

        Log.d("xyz.anon.arcsider","Begin receiving replacing content.");
        try {
            InputStream s = context.getContentResolver().openInputStream(intent.getParcelableExtra("curl"));

            File tDir = new File(context.getDataDir(),"asset_patch");

            if(!tDir.isDirectory()){
                tDir.delete();
            }
            tDir.mkdir();
            ZipHelper.Extract(s,(tDir).toString());
            Log.i("xyz.anon.arcsider", "content file sync completed.");

            NativeHooker.ReloadReplaceTable();

            String curLang = context.getResources().getConfiguration().getLocales().get(0).getISO3Language().substring(0,2);

            String output = "[Sider] Sync completed, shut in 3 second.";

            if(lang.containsKey(curLang)){
                output = lang.get(curLang);
            }

            Toast.makeText(context,output,Toast.LENGTH_LONG).show();
            new Thread(()->{
                try{Thread.sleep(3000);}catch(Throwable t){}
                Process.killProcess(Process.myPid());
            }).start();
        }
        catch(Throwable t){
            Log.e("xyz.anon.arcsider", "Unable to sync content file: "+t.getMessage());
            Log.e("xyz.anon.arcsider",t.toString());

            t.printStackTrace();
        }
    }
}
