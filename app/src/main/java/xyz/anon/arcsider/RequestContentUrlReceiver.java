package xyz.anon.arcsider;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;

import androidx.core.content.FileProvider;

import java.io.File;


public class RequestContentUrlReceiver extends BroadcastReceiver {
    public static Uri targetSend;
    @Override
    public void onReceive(Context context, Intent intent) {
        //File f = new File(context.getFilesDir(),"replace/content.zip");

        if(targetSend != null) {
            Intent post = new Intent("xyz.anon.arcsider.RESPONSE_CURL");
            //android.net.Uri u = FileProvider.getUriForFile(context, "xyz.anon.arcsider.ReplaceContentProvider", f);
            //post.setPackage("moe.low.arc");
            //post.setData(targetSend);
            post.putExtra("curl", targetSend);
            //post.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            context.sendBroadcast(post);
            Log.i("xyz.anon.arcsider","content replace request responsed.");

            //Cleanup for avoid multi calling.
            targetSend = null;
        }
    }
}
