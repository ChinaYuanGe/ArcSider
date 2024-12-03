package xyz.anon.arcsider;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.widget.Toast;

import java.io.*;

public class ReceiveContentUrlReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
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

            Toast.makeText(context,"[Sider] Sync completed",Toast.LENGTH_LONG).show();
        }
        catch(Throwable t){
            Log.d("xyz.anon.arcsider", "Unable to sync content file: "+t.getMessage());
        }
    }
}
