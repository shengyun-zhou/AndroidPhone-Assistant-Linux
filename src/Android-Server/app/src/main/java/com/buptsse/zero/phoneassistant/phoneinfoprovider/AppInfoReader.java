package com.buptsse.zero.phoneassistant.phoneinfoprovider;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.drawable.Drawable;

import java.io.ByteArrayOutputStream;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by buptsse-zero on 11/9/15.
 */
public class AppInfoReader
{
    private Context context;

    public AppInfoReader(Context context)
    {
        this.context = context;
    }

    public ArrayList<AppInfo> getAllAppInfo()
    {
        ArrayList<AppInfo> appInfoList = new ArrayList<>();
        PackageManager packageManager = context.getPackageManager();
        List<PackageInfo> pkgList = packageManager.getInstalledPackages(0);
        for(int i = 0; i < pkgList.size(); i++) {
            PackageInfo pkgInfo = pkgList.get(i);
            String appName = pkgInfo.applicationInfo.name;
            System.out.println("App Name" + appName);
            String appVersion = pkgInfo.versionName;
            String appPackage = pkgInfo.packageName;
            byte[] appIconBytes = drawableToByte(pkgInfo.applicationInfo.loadIcon(packageManager));
            if ((pkgInfo.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) == 0) {
                appInfoList.add(new AppInfo(appName, appVersion, appPackage, appIconBytes, false));
            } else if ((pkgInfo.applicationInfo.flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) != 0) {
                appInfoList.add(new AppInfo(appName, appVersion, appPackage, appIconBytes, false));
            } else {
                appInfoList.add(new AppInfo(appName, appVersion, appPackage, appIconBytes, true));
            }
        }
        return appInfoList;
    }

    private byte[] drawableToByte(Drawable drawable)
    {
        if(drawable == null)
            return null;
        Bitmap bitmap = Bitmap.createBitmap(drawable.getIntrinsicWidth(),
                                            drawable.getIntrinsicHeight(), Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);
        drawable.setBounds(0, 0, drawable.getIntrinsicWidth() - 1, drawable.getIntrinsicHeight() - 1);
        drawable.draw(canvas);
        ByteArrayOutputStream bos = new ByteArrayOutputStream(drawable.getIntrinsicWidth() * drawable.getIntrinsicHeight() * 4 + 1024);
        bitmap.compress(Bitmap.CompressFormat.PNG, 100, bos);
        return bos.toByteArray();
    }
}
