package com.buptsse.zero.phoneassistant.phoneinfoprovider;
;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.IPackageStatsObserver;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageStats;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.drawable.Drawable;
import android.os.RemoteException;

import java.io.ByteArrayOutputStream;
import java.lang.reflect.Method;
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
        Method getPackageSizeInfo = null;

        try {
            getPackageSizeInfo = PackageManager.class.getMethod("getPackageSizeInfo", String.class, IPackageStatsObserver.class);
        }catch (NoSuchMethodException e){
            e.printStackTrace();
            getPackageSizeInfo = null;
        }

        List<PackageInfo> pkgList = packageManager.getInstalledPackages(0);
        for(int i = 0; i < pkgList.size(); i++) {
            PackageInfo pkgInfo = pkgList.get(i);
            String appName = packageManager.getApplicationLabel(pkgInfo.applicationInfo).toString();
            if(appName == null) {
                appName = pkgInfo.packageName;
            }
            String appVersion = pkgInfo.versionName;
            String appPackage = pkgInfo.packageName;
            byte[] appIconBytes = drawableToByte(pkgInfo.applicationInfo.loadIcon(packageManager));

            long appSize = 0;
            if(getPackageSizeInfo != null) {
                try {
                    PackageSizeObserver packageSizeObserver = new PackageSizeObserver();
                    getPackageSizeInfo.invoke(packageManager, appPackage, packageSizeObserver);
                    while(packageSizeObserver.getAppSize() < 0)
                        Thread.sleep(10);
                    appSize = packageSizeObserver.getAppSize();
                }catch (Exception e){
                    e.printStackTrace();
                }
            }

            if ((pkgInfo.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) == 0) {
                appInfoList.add(new AppInfo(appName, appVersion, appPackage, appIconBytes, false, appSize));
            } else if ((pkgInfo.applicationInfo.flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) != 0) {
                appInfoList.add(new AppInfo(appName, appVersion, appPackage, appIconBytes, false, appSize));
            } else {
                appInfoList.add(new AppInfo(appName, appVersion, appPackage, appIconBytes, true, appSize));
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

    private class PackageSizeObserver extends IPackageStatsObserver.Stub
    {
        private long appSize = -1;
        @Override
        public void onGetStatsCompleted(PackageStats pStats, boolean succeeded) throws RemoteException {
            if(succeeded){
                appSize = pStats.dataSize + pStats.codeSize + pStats.cacheSize;
            }else{
                appSize = 0;
            }
        }

        public long getAppSize(){
            return appSize;
        }
    }

}
