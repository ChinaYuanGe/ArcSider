package xyz.anon.arcsider;

import java.io.*;

public class FileHelper {
    public static boolean deleteDir(File dir) {
        if(dir.isDirectory()){
            for(File f:dir.listFiles()){
                if(f.isDirectory()){
                    deleteDir(f);
                    f.delete();
                }
                else f.delete();
            }
            return dir.delete();
        }
        else if(dir.isFile()){
            return dir.delete();
        }
        return false;
    }
}
