package xyz.anon.arcsider;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.widget.Toast;

public class ArcaeaRequestSetupReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        Log.d("xyz.anon.arcsider","ArcaeaReqSetupBD, Recd.");
        MainActivity.Config.ConfigPath = context.getFilesDir().getAbsolutePath()+"/configs";
        Log.d("xyz.anon.arcsider","setup config Path="+MainActivity.Config.ConfigPath);

        Intent data = new Intent("xyz.anon.arcsider.RESPONSE_SETUP");
        data.putExtra("global_enable", MainActivity.Config.getGlobalEnable());
        data.putExtra("pure_hidden", MainActivity.Config.getPureHidden());
        data.putExtra("note_style", MainActivity.Config.getNoteStyle());
        data.putExtra("arctap_style", MainActivity.Config.getArcTapStyle());
        data.putExtra("track_style", MainActivity.Config.getTrackStyle());
        context.sendBroadcast(data);
    }
}
