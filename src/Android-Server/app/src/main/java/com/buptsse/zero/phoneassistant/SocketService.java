package com.buptsse.zero.phoneassistant;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.support.annotation.Nullable;

import java.net.ServerSocket;

public class SocketService extends Service
{
    private Binder binder = null;
    private SocketThread socketThread = null;
    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        binder = new Binder();
        return binder;
    }

    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if(socketThread == null || !socketThread.isAlive())
        {
            socketThread = new SocketThread();
            socketThread.start();
        }
        return super.onStartCommand(intent, flags, startId);
    }
}
