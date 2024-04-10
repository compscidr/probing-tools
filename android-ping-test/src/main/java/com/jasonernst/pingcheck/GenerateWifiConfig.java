package com.jasonernst.pingcheck;

import android.net.wifi.WifiConfiguration;
import android.util.Log;

/**
 * Created by jason on 01/03/16.
 */
public class GenerateWifiConfig {

    public static WifiConfiguration generateConfig(String ssid)
    {
        WifiConfiguration config = new WifiConfiguration();
        config.SSID = ssid;         //need to use backslashes here if SDK version < LOLLIPOP (http://stackoverflow.com/questions/28313440/how-to-connect-android-device-to-wpa2-psk-secured-wifi-hotspot-network-program)
        config.hiddenSSID = false;  //set to true once this works
        config.priority = 100;      //this should be the highest priority, or the first one in the list in order to be selected first

        //when we addNetwork, this string must contain the inner quote marks ", but later on when we create the AP and pass it
        //the config, we do not do this.

        if(ssid.equals("LOTD"))
            config.preSharedKey = "\"l3ft0fth3d0t\"";
        else if(ssid.equals("wave-test"))
            config.preSharedKey = "\"robotics\"";
        config.allowedProtocols.set(WifiConfiguration.Protocol.RSN);
        config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
        config.status = WifiConfiguration.Status.ENABLED;
        config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
        config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
        config.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
        config.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
        config.allowedProtocols.set(WifiConfiguration.Protocol.RSN);
        config.allowedProtocols.set(WifiConfiguration.Protocol.WPA);

        return config;
    }
}
