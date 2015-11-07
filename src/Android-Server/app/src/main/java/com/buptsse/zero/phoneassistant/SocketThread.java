package com.buptsse.zero.phoneassistant;

import android.app.Service;
import android.util.Log;

import java.io.IOException;
import java.io.PrintStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;

/**
 * Created by buptsse-zero on 11/6/15.
 */
public class SocketThread extends Thread
{
    public static final int BIND_PORT = 42687;
    private ServerSocket serverSocket = null;

    @Override
    public void run() {
        super.run();
        Socket sessionSocket = null;
        try{
            serverSocket = new ServerSocket(BIND_PORT, 3);
            Log.i("AndroidServer", "Start successfully");
            while(true) {
                sessionSocket = serverSocket.accept();
                Log.i("AndroidServer", "Accepted");
                new SessionThread(sessionSocket).start();
            }
        }catch (Exception e){
            e.printStackTrace();
            try{
                if(sessionSocket != null)
                    sessionSocket.close();
                if(serverSocket != null)
                    serverSocket.close();
            }catch (IOException ioe){
                e.printStackTrace();
            }
            Log.i("AndroidServer", "Stopped");
            return;
        }
    }

    public void stopServer(){
        if(!isAlive())
            return;
        try {
            serverSocket.close();
            sleep(500);
        }catch (Exception e){
            e.printStackTrace();
        }
        return;
    }
}
