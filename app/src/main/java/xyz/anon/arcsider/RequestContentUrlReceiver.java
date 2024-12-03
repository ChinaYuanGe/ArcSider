package xyz.anon.arcsider;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import androidx.core.content.FileProvider;

import java.io.File;


public class RequestContentUrlReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        File f = new File(context.getFilesDir(),"replace/content.zip");

        if(f.isFile()) {
            Intent post = new Intent("xyz.anon.arcsider.RESPONSE_CURL");
            android.net.Uri u = FileProvider.getUriForFile(context, "xyz.anon.arcsider.ContentProvider", f);
            context.grantUriPermission("moe.low.arc",u,Intent.FLAG_GRANT_READ_URI_PERMISSION);
            post.putExtra("curl", u);
            post.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            context.sendBroadcast(post);
            Log.i("xyz.anon.arcsider","content replace request responsed.");
        }
    }
}
