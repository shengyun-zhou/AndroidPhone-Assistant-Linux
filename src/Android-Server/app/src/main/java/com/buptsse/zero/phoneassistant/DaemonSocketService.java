package com.buptsse.zero.phoneassistant;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.support.annotation.Nullable;

import java.net.ServerSocket;

public class DaemonSocketService extends Service
{
    private SocketServiceBinder binder = null;
    private SocketThread socketThread = null;
    private boolean is_running = false;

    public class SocketServiceBinder extends Binder{
        private Service service;
        public SocketServiceBinder(Service service)
        {
            this.service = service;
        }

        public Service getService() {
            return service;
        }
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        binder = new SocketServiceBinder(this);
        return binder;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        is_running = true;
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

    @Override
    public void onDestroy() {
        if(socketThread != null && socketThread.isAlive())
            socketThread.stopServer();
        is_running = false;
        super.onDestroy();
    }

    public boolean is_service_run()
    {
        return is_running;
    }
}
