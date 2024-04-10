package com.jasonernst.pingcheck;

import android.content.Context;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import java.util.List;

public class MainActivity extends AppCompatActivity {

    private WifiManager wifiManager;
    private WifiMonitor wifiMonitor;

    @Override
    protected void onDestroy() {
        wifiManager.disconnect();
        wifiManager.disableNetwork(wifiMonitor.current_net_id);
        wifiManager.removeNetwork(wifiMonitor.current_net_id);

        super.onDestroy();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        wifiMonitor = new WifiMonitor();
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
        registerReceiver(wifiMonitor, intentFilter);

        boolean ssidFound = false;
        wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);

        wifiManager.setWifiEnabled(true);
        wifiManager.disconnect();
        if (ActivityCompat.checkSelfPermission(this, android.Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            return;
        }
        List<ScanResult> scans = wifiManager.getScanResults();
        TextView statusText = (TextView) findViewById(R.id.txtStatus);
        statusText.setText("scanning...");

        WifiConfiguration config = GenerateWifiConfig.generateConfig("LOTD");

        //todo: do a check first to see if network exists in the configuration file.
        int netid = wifiManager.addNetwork(config);
        wifiMonitor.current_net_id = netid;
        if (netid == -1) {
            Log.d("ERROR", "Couldn't add the network to the wifi manager");
            return;
        }
        wifiManager.enableNetwork(netid, true);

        new Thread() {
            @Override
            public void run() {
                try {
                    while (!isInterrupted()) {
                        Thread.sleep(200);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                refresh();
                            }
                        });
                    }
                } catch (InterruptedException e) {
                    // ignored
                }
            }
        }.start();
    }

    private void refresh() {
        updateText();
        //listClients(300);
    }

    private void updateText() {
        StringBuilder sb = new StringBuilder();
        TextView statusText = (TextView) findViewById(R.id.txtStatus);
        if(wifiMonitor.currentSSID != "") {
            sb.append(wifiMonitor.currentSSID + "\n");
        }
        if(wifiMonitor.gatewayIP != "") {
            sb.append(wifiMonitor.gatewayIP + "\n");
        }
        if(wifiMonitor.pingStatus != "")
        {
            sb.append(wifiMonitor.pingStatus + "\n");
        }
        sb.append("Difference: " + wifiMonitor.difference + "ms");
        statusText.setText(sb.toString());
    }
}
