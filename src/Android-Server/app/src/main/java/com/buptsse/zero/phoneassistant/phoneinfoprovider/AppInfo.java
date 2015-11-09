package com.buptsse.zero.phoneassistant.phoneinfoprovider;

/**
 * Created by buptsse-zero on 11/9/15.
 */
public class AppInfo
{
    private String appName;
    private String appVersion;
    private String appPackageName;
    private byte[] appIconBytes;
    private boolean isSystemApp;

    public AppInfo(String appName, String appVersion, String appPackageName, byte[] appIconBytes, boolean isSystemApp)
    {
        this.appName = appName;
        this.appVersion = appVersion;
        this.appPackageName = appPackageName;
        this.appIconBytes = appIconBytes;
        this.isSystemApp = isSystemApp;
    }

    public String getAppName() {
        return appName;
    }

    public String getAppVersion() {
        return appVersion;
    }

    public String getAppPackageName() {
        return appPackageName;
    }

    public byte[] getAppIconBytes() {
        return appIconBytes;
    }

    public boolean isSystemApp() {
        return isSystemApp;
    }
}
