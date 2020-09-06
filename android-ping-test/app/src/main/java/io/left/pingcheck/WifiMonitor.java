package io.left.pingcheck;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.DhcpInfo;
import android.net.NetworkInfo;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Iterator;
import java.util.List;

/**
 * Created by jason on 01/03/16.
 * http://stackoverflow.com/questions/5888502/how-to-detect-when-wifi-connection-has-been-established-in-android
 */
public class WifiMonitor extends BroadcastReceiver {

    int current_net_id;
    String currentSSID;
    String gatewayIP;
    String pingStatus;
    int time;
    int difference;

    public WifiMonitor()
    {
        currentSSID = "";
        gatewayIP = "";
        pingStatus = "";
        time = 0;
        difference = 0;
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        ConnectivityManager cm = (ConnectivityManager)context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetwork = cm.getActiveNetworkInfo();
        if(activeNetwork == null)
            return;
        boolean isWiFi = activeNetwork.getType() == ConnectivityManager.TYPE_WIFI;
        if(!isWiFi)
            return;

        NetworkInfo info = intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);
        if(info != null && info.isConnected()) {
            int now = (int) (System.currentTimeMillis());
            if(time != 0)
                difference = Math.abs(time - now);

            WifiManager wifiManager = (WifiManager)context.getSystemService(Context.WIFI_SERVICE);
            WifiInfo wifiInfo = wifiManager.getConnectionInfo();

            currentSSID = wifiInfo.getSSID();
            currentSSID = currentSSID.replace("\"", "");
            Log.d("TEST", "Connected - '" + currentSSID + "'");


            DhcpInfo dhcp = wifiManager.getDhcpInfo();
            InetAddress addr = intToInetAddress(dhcp.gateway);
            gatewayIP = addr == null ? "" : addr.toString();
            gatewayIP = gatewayIP.replace("/","");
            pingStatus = ping(gatewayIP);

            WifiConfiguration config;
            if(currentSSID.equals("LOTD")) {
                config = GenerateWifiConfig.generateConfig("wave-test");
            }
            else
                config = GenerateWifiConfig.generateConfig("LOTD");

            wifiManager.disconnect();
            wifiManager.disableNetwork(current_net_id);
            wifiManager.removeNetwork(current_net_id);

            time = (int) (System.currentTimeMillis());
            int netid = wifiManager.addNetwork(config);
            if (netid == -1) {
                Log.d("ERROR", "Couldn't add the network to the wifi manager");
                return;
            }
            current_net_id = netid;
            wifiManager.enableNetwork(netid, true);
        }
    }

    public static InetAddress intToInetAddress(int hostAddress) {
        byte[] addressBytes = { (byte)(0xff & hostAddress),
                (byte)(0xff & (hostAddress >> 8)),
                (byte)(0xff & (hostAddress >> 16)),
                (byte)(0xff & (hostAddress >> 24)) };
        try {
            return InetAddress.getByAddress(addressBytes);
        } catch (UnknownHostException e) {
            throw new AssertionError();
        }
    }

    public String ping(String url) {
        Log.d("PINGING: ", url);
        String str = "";
        Runtime runtime = Runtime.getRuntime();
        try {
            Process process = runtime.exec("ping -c 1 " + url);
            process.waitFor();
            BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            int i;
            char[] buffer = new char[4096];
            StringBuffer output = new StringBuffer();
            while ((i = reader.read(buffer)) > 0)
                output.append(buffer, 0, i);
            reader.close();

            // body.append(output.toString()+"\n");
            str = output.toString();
            Log.d("PING", str);

            if(process.exitValue() != 0)
                Log.d("PING", "RETURN ERROR: " + process.exitValue());
        } catch (InterruptedException ignore) {
            ignore.printStackTrace();
            Log.d("PING ERROR", " Exception:" + ignore);
        } catch (IOException e) {
            // body.append("Error\n");
            Log.d("PING ERROR", "ERRRRRR");
            e.printStackTrace();
        }
        return str;
    }
}
