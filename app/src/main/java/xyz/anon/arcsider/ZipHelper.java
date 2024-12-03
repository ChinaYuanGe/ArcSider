package xyz.anon.arcsider;

import java.util.zip.*;
import java.io.*;

public class ZipHelper {
    /* Copy from here: https://www.cnblogs.com/luciochn/p/14515029.html */
    public static void Extract(String zipFilePath,String desDirectory) throws Exception{
        File desDir = new File(desDirectory);
        if (!desDir.exists()) {
            boolean mkdirSuccess = desDir.mkdir();
            if (!mkdirSuccess) {
                throw new Exception("创建解压目标文件夹失败");
            }
        }
        ZipHelper.Extract(new FileInputStream(zipFilePath),desDirectory);
    }
    public static void Extract(InputStream stream, String desDirectory) throws Exception{
        // 读入流
        ZipInputStream zipInputStream = new ZipInputStream(stream);
        // 遍历每一个文件
        ZipEntry zipEntry = zipInputStream.getNextEntry();
        while (zipEntry != null) {
            if (zipEntry.isDirectory()) { // 文件夹
                String unzipFilePath = desDirectory + File.separator + zipEntry.getName();
                // 直接创建
                mkdir(new File(unzipFilePath));
            } else { // 文件
                String unzipFilePath = desDirectory + File.separator + zipEntry.getName();
                File file = new File(unzipFilePath);
                // 创建父目录
                mkdir(file.getParentFile());
                // 写出文件流
                BufferedOutputStream bufferedOutputStream =
                        new BufferedOutputStream(new FileOutputStream(unzipFilePath));
                byte[] bytes = new byte[1024];
                int readLen;
                while ((readLen = zipInputStream.read(bytes)) != -1) {
                    bufferedOutputStream.write(bytes, 0, readLen);
                }
                bufferedOutputStream.close();
            }
            zipInputStream.closeEntry();
            zipEntry = zipInputStream.getNextEntry();
        }
        zipInputStream.close();
    }
    private static void mkdir(File file) {
        if (null == file || file.exists()) {
            return;
        }
        mkdir(file.getParentFile());
        file.mkdir();
    }
}
