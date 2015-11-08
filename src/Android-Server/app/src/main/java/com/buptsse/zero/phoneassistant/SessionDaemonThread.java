package com.buptsse.zero.phoneassistant;

import java.io.IOException;
import java.net.Socket;

/**
 * Created by buptsse-zero on 11/8/15.
 */
public class SessionDaemonThread extends Thread
{
    Socket socket;

    public SessionDaemonThread(Socket socket)
    {
        this.socket = socket;
    }

    @Override
    public void run() {
        super.run();
        while(true)
        {
            try {
                socket.sendUrgentData(0);
                sleep(2000);
            }catch (IOException ioe){
                ioe.printStackTrace();
                try {
                    socket.close();
                }catch (IOException ioe2){
                    ioe2.printStackTrace();
                }
                return;
            }catch (InterruptedException ie){
                ie.printStackTrace();
            }
        }
    }
}
