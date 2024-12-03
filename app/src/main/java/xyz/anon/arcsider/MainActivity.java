package xyz.anon.arcsider;

import androidx.activity.result.ActivityResultCallback;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContentResolverCompat;

import android.app.AndroidAppHelper;
import android.content.BroadcastReceiver;
import android.content.ContentProvider;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.AssetManager;
import android.media.MediaMetadata;
import android.net.Uri;
import android.os.Bundle;
import android.os.FileUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Date;
import java.util.logging.Logger;

import xyz.anon.arcsider.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {
    static class Config{
        //public static boolean GlobaEnbale = true;
        public static boolean getGlobalEnable(){
            String res = Read("global_enable");
            return res == null || res.equals("true");
        }
        public static void setGlobalEnable(boolean val){Write("global_enable",val?"true":"false");}
        //public static boolean PureHidden = false;
        public static boolean getPureHidden(){
            String res = Read("pure_hidden");
            return res!=null && res.equals("true");
        }
        public static void setPureHidden(boolean val){Write("pure_hidden",val?"true":"false");}
        //public static int ArcTapStyle = 1;
        public static int getArcTapStyle(){
            String res = Read("arctap_style");
            return res != null ? Integer.valueOf(res) : 1;
        }
        public static void setArcTapStyle(int val){Write("arctap_style",Integer.toString(val));};
        //public static int NoteStyle = 1;
        public static int getNoteStyle(){
            String res = Read("note_style");
            return res != null ? Integer.valueOf(res) : 1;
        }
        public static void setNoteStyle(int val){Write("note_style",Integer.toString(val));}
        //public static int TrackStyle = 3;
        public static int getTrackStyle(){
            String res = Read("track_style");
            return res != null ? Integer.valueOf(res) : 3;
        }
        public static void setTrackStyle(int val){Write("track_style",Integer.toString(val));}

        public static String ConfigPath = null;
        public static String Read(String name){
            try {
                File s = new File(ConfigPath + "/"+name);
                byte[] rd = new byte[(int)s.length()];
                FileInputStream is = new FileInputStream(s);
                is.read(rd);
                is.close();
                String val = new String(rd);
                return val;
            }
            catch(Throwable t){
                return null;
            }
        }
        public static void Write(String name,String value){
            try{
                File root = new File(ConfigPath);
                if(!root.isDirectory()){
                    root.mkdir();
                }
                File s = new File(ConfigPath + "/" + name);
                if(s.exists() && s.isFile()){
                    s.delete();
                }
                s.createNewFile();
                FileOutputStream os = new FileOutputStream(s);
                os.write(value.getBytes());
                os.flush();
                os.close();
            }
            catch(Throwable t){}
        }
    }
    private ActivityMainBinding binding;
    RequestContentUrlReceiver CurlReqReceiver;

    ActivityResultLauncher<String> mGetContext = registerForActivityResult(new ActivityResultContracts.GetContent(), new ActivityResultCallback<Uri>() {
        @Override
        public void onActivityResult(Uri result) {
            /* Is this way to detect empty input? */
            try{
                if(result.equals(Uri.EMPTY)) return;
            }
            catch(Throwable t){ return;}

            try {
                InputStream stream = MainActivity.this.getContentResolver().openInputStream(result);
                try {
                    File target = new File(MainActivity.this.getFilesDir(),"replace/content.zip");
                    if(target.exists() && target.isFile()){
                        target.delete();
                    }
                    if(target.createNewFile()) {
                        FileOutputStream output = new FileOutputStream(target);

                        byte[] buffer = new byte[1024];

                        int rsize = stream.read(buffer);
                        while(rsize >= 0){
                            output.write(buffer,0,rsize);
                            rsize = stream.read(buffer);
                        }

                        output.flush();
                        output.close();
                        stream.close();

                        // Create request receiver
                        if (CurlReqReceiver != null) {
                            MainActivity.this.unregisterReceiver(CurlReqReceiver);
                        }
                        CurlReqReceiver = new RequestContentUrlReceiver();
                        MainActivity.this.registerReceiver(CurlReqReceiver,new IntentFilter("xyz.anon.arcsider.REQUEST_CURL"),Context.RECEIVER_EXPORTED);
                        Log.d("xyz.anon.arcsider","Content sync setup completed.");
                        ((TextView)MainActivity.this.findViewById(R.id.lbl_synctips)).setText(getText(R.string.rt_lbl_start_arcaea));

                    }
                }
                catch (Exception ex){
                    Toast.makeText(MainActivity.this,getText(R.string.rt_toast_sthgowrong).toString(),Toast.LENGTH_LONG).show();
                    Log.e("xyz.anon.arcsider",ex.getMessage());
                    ex.printStackTrace();
                }
            }
            catch(FileNotFoundException ex){
                Toast.makeText(MainActivity.this,getText(R.string.rt_toast_fileloadfailed).toString(),Toast.LENGTH_LONG).show();
            }
            catch(Throwable t){
                Toast.makeText(MainActivity.this,getText(R.string.rt_toast_sthgowrong).toString(),Toast.LENGTH_LONG).show();
                Log.e("xyz.anon.arcsider",t.getMessage());
                t.printStackTrace();
            }
        }
    });

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Config.ConfigPath = this.getFilesDir().getAbsolutePath() + "/configs";
        Log.d("xyz.anon.arcsider","CurrentConfigPath="+Config.ConfigPath);
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        //ReadConfig

        binding.switchReplaceEnable.setChecked(Config.getGlobalEnable());
        binding.switchHidePure.setChecked(Config.getPureHidden());
        ((RadioButton)binding.radiogroupNoteStyle.getChildAt(Config.getNoteStyle())).setChecked(true);
        ((RadioButton)binding.radiogroupArctapStyle.getChildAt(Config.getArcTapStyle())).setChecked(true);
        ((RadioButton)binding.radiogroupTrackstyle.getChildAt(Config.getTrackStyle())).setChecked(true);

        //Event Binding
        binding.switchReplaceEnable.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                Config.setGlobalEnable(b);
                SendConfigBroadcast();
            }
        });

        binding.switchHidePure.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                Config.setPureHidden(b);
                SendConfigBroadcast();
            }
        });

        binding.radiogroupNoteStyle.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup radioGroup, int i) {
                Config.setNoteStyle(radioGroup.indexOfChild(findViewById(i)));
                SendConfigBroadcast();
            }
        });

        binding.radiogroupArctapStyle.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup radioGroup, int i) {
                Config.setArcTapStyle(radioGroup.indexOfChild(findViewById(i)));
                SendConfigBroadcast();
            }
        });

        binding.radiogroupTrackstyle.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup radioGroup, int i) {
                Config.setTrackStyle(radioGroup.indexOfChild(findViewById(i)));
                SendConfigBroadcast();
            }
        });

        binding.btnSetReplace.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mGetContext.launch("*/*");
            }
        });

        binding.btnUseDefaultReplace.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                try {
                    InputStream stream = MainActivity.this.getAssets().open("sider-default.zip");
                    try {
                        File target = new File(MainActivity.this.getFilesDir(),"replace/content.zip");
                        if(target.exists() && target.isFile()){
                            target.delete();
                        }
                        if(target.createNewFile()) {
                            FileOutputStream output = new FileOutputStream(target);

                            byte[] buffer = new byte[1024];

                            int rsize = stream.read(buffer);
                            while(rsize >= 0){
                                output.write(buffer,0,rsize);
                                rsize = stream.read(buffer);
                            }

                            output.flush();
                            output.close();
                            stream.close();

                            // Create request receiver
                            if (CurlReqReceiver != null) {
                                MainActivity.this.unregisterReceiver(CurlReqReceiver);
                            }
                            CurlReqReceiver = new RequestContentUrlReceiver();
                            MainActivity.this.registerReceiver(CurlReqReceiver,new IntentFilter("xyz.anon.arcsider.REQUEST_CURL"),Context.RECEIVER_EXPORTED);
                            Log.d("xyz.anon.arcsider","Content sync setup completed.");
                            ((TextView)MainActivity.this.findViewById(R.id.lbl_synctips)).setText(getText(R.string.rt_lbl_start_arcaea));

                        }
                    }
                    catch (Exception ex){
                        Toast.makeText(MainActivity.this,getText(R.string.rt_toast_sthgowrong).toString(),Toast.LENGTH_LONG).show();
                        Log.e("xyz.anon.arcsider",ex.getMessage());
                        ex.printStackTrace();
                    }
                }
                catch(FileNotFoundException ex){
                    Toast.makeText(MainActivity.this,getText(R.string.rt_toast_fileloadfailed).toString(),Toast.LENGTH_LONG).show();
                }
                catch(Throwable t){
                    Toast.makeText(MainActivity.this,getText(R.string.rt_toast_sthgowrong).toString(),Toast.LENGTH_LONG).show();
                    Log.e("xyz.anon.arcsider",t.getMessage());
                    t.printStackTrace();
                }
            }
        });
    }

    void SendConfigBroadcast(){
        Intent data = new Intent("xyz.anon.arcsider.RESPONSE_SETUP");
        data.putExtra("global_enable",Config.getGlobalEnable());
        data.putExtra("pure_hidden",Config.getPureHidden());
        data.putExtra("note_style",Config.getNoteStyle());
        data.putExtra("arctap_style",Config.getArcTapStyle());
        data.putExtra("track_style",Config.getTrackStyle());
        this.sendBroadcast(data);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onDestroy() {
        if(CurlReqReceiver != null) unregisterReceiver(CurlReqReceiver);
        super.onDestroy();
    }
}