package io.left.porttest;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkInfo;
import android.os.Build;
import android.os.Process;
import android.provider.ContactsContract;
import android.support.annotation.RequiresApi;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import java.io.IOException;
import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.UnknownHostException;

public class MainActivity extends AppCompatActivity {

    public static final String TAG = "PORT TEST";

    @RequiresApi(api = Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /*
        ConnectivityManager connectivityManager = (ConnectivityManager) this.getSystemService(Context.CONNECTIVITY_SERVICE);
        Network[] network = connectivityManager.getAllNetworks();
        if(network != null && network.length >0 ){
            for(int i = 0 ; i < network.length ; i++){
                NetworkInfo networkInfo = connectivityManager.getNetworkInfo(network[i]);
                int networkType = networkInfo.getType();
                if(ConnectivityManager.TYPE_MOBILE == networkType ){
                    connectivityManager.bindProcessToNetwork(network[i]);
                }
            }
        }*/
        //loopbacktest();
    }

    public void loopbacktest() {
        new Thread() {
            public void run() {
                try {
                    DatagramSocket socket = new DatagramSocket(5000);
                    byte[] buf = new byte[1024];
                    DatagramPacket p = new DatagramPacket(buf, 1024);
                    while (true) {
                        socket.receive(p);
                        byte[] recv = new byte[p.getLength()];
                        System.arraycopy(p.getData(), 0, recv, 0, p.getLength());
                        Log.d(TAG, new String(recv));
                    }
                } catch(IOException ex) {
                    Log.d(TAG, ex.toString());
                }
            }
        }.start();
    }

    public void udp(View v) {
        int startPort = 80;
        int endport = 80;
        while(startPort <= endport) {
            final int port = startPort;
            new Thread() {
                public void run() {
                    byte[] buf = "TEST FROM APP".getBytes();
                    DatagramSocket socket = null;
                    try {
                        socket = new DatagramSocket(2000);
                        DatagramPacket p = new DatagramPacket(buf, buf.length, InetAddress.getByName("research.rightmesh.io"), port);
                        //DatagramPacket p2 = new DatagramPacket(buf, buf.length, InetAddress.getByName("127.0.0.1"), 5000);
                        socket.send(p);
                        Log.d(TAG, "SEND ON PORT: " + port);
                        DatagramPacket p2 = new DatagramPacket(buf, buf.length);
                        socket.setSoTimeout(1000);
                        socket.receive(p2);
                        Log.d(TAG, "GOT RESPONSE!");
                        //socket.send(p2);
                        socket.close();

                    } catch (UnknownHostException ex) {
                        Log.d(TAG, ex.toString());
                    } catch (IOException ex) {
                        Log.d(TAG, ex.toString());
                        if(socket != null) {
                            try {
                                socket.close();
                            } catch(Exception ex2) {
                                Log.d(TAG, ex2.toString());
                            }
                        }
                    }
                }
            }.start();
            startPort++;
        }
    }

    public void tcp(View v) {
        new Thread() {
            public void run() {
                try {
                    Log.d(TAG, "trying to connect");
                    Socket socket = new Socket(InetAddress.getByName("research.rightmesh.io"), 443);
                    BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                    OutputStream outputStream = socket.getOutputStream();
                    outputStream.write("test\n".getBytes());
                    String line = bufferedReader.readLine();
                    Log.d(TAG, line);
                    socket.close();
                } catch(UnknownHostException ex) {
                    Log.d(TAG, ex.toString());
                } catch (IOException ex) {
                    Log.d(TAG, ex.toString());
                }
            }
        }.start();
    }
}
