package com.pantech.settings.device;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.IBinder;
import android.util.Log;
import java.io.IOException;

public final class ChargeLimitService extends Service
        implements SharedPreferences.OnSharedPreferenceChangeListener {
    private Intent mBattery;
    private SharedPreferences mPreferences;
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override public void onReceive(Context context, Intent intent) {
            mBattery = intent;
            applyLimit();
        }
    };

    @Override public void onCreate() {
        super.onCreate();
        mPreferences = TouchMode.preferences(this);
        mPreferences.registerOnSharedPreferenceChangeListener(this);
        registerReceiver(mReceiver, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
    }

    private void applyLimit() {
        try {
            ChargingControl.apply(this, ChargingControl.limit(this), mBattery);
        } catch (IOException e) {
            Log.e("VegaSettings", "Cannot apply charging limit", e);
        }
    }

    @Override public void onSharedPreferenceChanged(SharedPreferences preferences, String key) {
        if (ChargingControl.LIMIT.equals(key)) applyLimit();
    }

    @Override public int onStartCommand(Intent intent, int flags, int startId) {
        return START_STICKY;
    }

    @Override public void onDestroy() {
        unregisterReceiver(mReceiver);
        mPreferences.unregisterOnSharedPreferenceChangeListener(this);
        super.onDestroy();
    }

    @Override public IBinder onBind(Intent intent) { return null; }
}
