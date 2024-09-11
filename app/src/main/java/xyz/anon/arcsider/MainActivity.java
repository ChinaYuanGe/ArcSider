package xyz.anon.arcsider;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AndroidAppHelper;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.FileUtils;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.RadioGroup;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
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
        static String Read(String name){
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
        static void Write(String name,String value){
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
    //private ArcaeaRequestSetupReceiver requestSetupReceiver;

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

        //requestSetupReceiver = new ArcaeaRequestSetupReceiver();

        //registerReceiver(requestSetupReceiver,new IntentFilter("xyz.cnyg.arcsider.REQUEST_SETUP"));

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
    protected void onDestroy() {
        //unregisterReceiver(requestSetupReceiver);
        super.onDestroy();
    }
}