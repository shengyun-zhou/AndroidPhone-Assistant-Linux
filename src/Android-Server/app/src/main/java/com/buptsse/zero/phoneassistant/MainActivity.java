package com.buptsse.zero.phoneassistant;

import android.app.ActivityManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.util.List;

public class MainActivity extends AppCompatActivity {
    private final String KEY_START_SERVICE = "start-service";

    private Button buttonStartService = null;
    private Button buttonStopService = null;
    private TextView textServiceStatus = null;
    private SocketDaemonService.SocketServiceBinder binder = null;
    private ServiceConnection serviceConnection = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textServiceStatus = (TextView)findViewById(R.id.text_service_status);
        buttonStartService = (Button)findViewById(R.id.button_service_start);
        buttonStopService = (Button)findViewById(R.id.button_service_stop);

        serviceConnection = new ServiceConnection() {
            @Override
            public void onServiceConnected(ComponentName name, IBinder service) {
                binder = (SocketDaemonService.SocketServiceBinder)service;
                buttonStartService.setEnabled(false);
                buttonStopService.setEnabled(true);
                textServiceStatus.setText("Daemon service status:running");
            }

            @Override
            public void onServiceDisconnected(ComponentName name) {
                buttonStartService.setEnabled(true);
                buttonStopService.setEnabled(false);
                textServiceStatus.setText("Daemon service status:stopped");
                binder = null;
            }
        };

        final Intent serviceIntent = new Intent();
        serviceIntent.setClass(MainActivity.this, SocketDaemonService.class);

        boolean startServiceFlag = false;
        Intent intentPack = getIntent();
        startServiceFlag = intentPack.getBooleanExtra(KEY_START_SERVICE, false);

        if(isServiceRunning())
        {
            textServiceStatus.setText("Daemon service status:running");
            buttonStartService.setEnabled(false);;
            bindService(serviceIntent, serviceConnection, Context.BIND_AUTO_CREATE);
        }else if(!startServiceFlag) {
            textServiceStatus.setText("Daemon service status:stopped");
            buttonStopService.setEnabled(false);
        }else {
            startService(serviceIntent);
            bindService(serviceIntent, serviceConnection, Context.BIND_AUTO_CREATE);
            textServiceStatus.setText("Daemon service status:running");
            buttonStartService.setEnabled(false);
        }

        buttonStartService.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startService(serviceIntent);
                bindService(serviceIntent, serviceConnection, Context.BIND_AUTO_CREATE);
            }
        });

        buttonStopService.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(binder == null)
                    return;
                unbindService(serviceConnection);;
                stopService(serviceIntent);
                buttonStartService.setEnabled(true);
                buttonStopService.setEnabled(false);
                textServiceStatus.setText("Daemon service status:stopped");
                binder = null;
            }
        });
    }

    private boolean isServiceRunning()
    {
        ActivityManager manager = (ActivityManager)getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningServiceInfo> runningServiceInfoList = manager.getRunningServices(100);
        if(runningServiceInfoList == null)
            return false;
        for(int i = 0; i < runningServiceInfoList.size(); i++)
        {
            if(runningServiceInfoList.get(i).service.getClassName().equals(SocketDaemonService.class.getName()))
                return true;
        }
        return false;
    }

    @Override
    public void onBackPressed() {
        if(binder != null) {
            unbindService(serviceConnection);
            binder = null;
        }
        super.onBackPressed();
    }
}
