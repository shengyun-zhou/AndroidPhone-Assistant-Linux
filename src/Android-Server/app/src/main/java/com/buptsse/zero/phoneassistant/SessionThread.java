package com.buptsse.zero.phoneassistant;

import android.util.Log;

import java.io.IOException;
import java.io.PrintStream;
import java.net.Socket;
import java.util.Scanner;

/**
 * Created by buptsse-zero on 11/7/15.
 */
public class SessionThread extends Thread
{
    private Socket sessionSocket;
    PrintStream socketOutput = null;
    Scanner socketInput = null;

    SessionThread(Socket sessionSocket)
    {
        this.sessionSocket = sessionSocket;
    }

    @Override
    public void run() {
        super.run();
        try{
            socketOutput = new PrintStream(sessionSocket.getOutputStream());
            socketInput = new Scanner(sessionSocket.getInputStream());
            socketOutput.print("Hello,PC!");
            while (true){
                sessionSocket.sendUrgentData(0);
                if(socketInput.hasNext()) {
                    String receiceMsg = socketInput.next();
                    Log.i("AndroidServerReceive", receiceMsg);
                }
                else{
                    sleep(1000);
                }
            }
        }catch (Exception e){
            e.printStackTrace();
            Log.i("AndroidServerSession", "Closed");
            try {
                sessionSocket.close();
            }catch (IOException ioe){
                e.printStackTrace();
            }
            return;
        }
    }
}
