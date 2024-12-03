package xyz.anon.arcsider;

import android.app.Application;
import java.io.*;

public class MainApplication extends Application {
    @Override
    public void onCreate() {
        File zipaDir = new File(this.getFilesDir().getAbsolutePath().toString() + "/replace");
        if(!zipaDir.exists() || !zipaDir.isDirectory()){
            zipaDir.delete();
        }
        zipaDir.mkdir();
        super.onCreate();
    }
}
