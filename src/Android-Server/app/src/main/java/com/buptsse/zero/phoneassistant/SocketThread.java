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
    private Service service;


    @Override
    public void run() {
        super.run();
        Socket sessionSocket = null;
        try{
            serverSocket = new ServerSocket(BIND_PORT, 1);
            Log.i("AndroidServer", "Start successfully");
            sessionSocket = serverSocket.accept();
            Log.i("AndroidServer", "Accepted");
            PrintStream socketOutput = new PrintStream(sessionSocket.getOutputStream());
            Scanner socketInput = new Scanner(sessionSocket.getInputStream());
            while (true){
                if(socketInput.hasNext()) {
                    String receiceMsg = socketInput.next();
                    Log.i("AndroidServerReceive", receiceMsg);
                    socketOutput.print("Hello,PC!");
                }
                else{
                    sleep(1000);
                }
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
            return;
        }
    }
}
