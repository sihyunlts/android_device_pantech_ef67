package com.pantech.settings.device;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.BatteryManager;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

final class ChargingControl {
    static final String LIMIT = "charging_limit";
    private static final String PAUSED = "charging_limit_paused";
    private static final String PATH = "/sys/class/power_supply/battery/charging_enabled";

    static boolean available() { return new File(PATH).canWrite(); }

    static int limit(Context context) {
        return Integer.parseInt(TouchMode.preferences(context).getString(LIMIT, "0"));
    }

    static boolean apply(Context context, int limit, Intent battery) throws IOException {
        SharedPreferences preferences = TouchMode.preferences(context);
        boolean paused = preferences.getBoolean(PAUSED, false);
        if (limit != limit(context)) paused = false;
        if (limit == 0) {
            paused = false;
        } else {
            if (battery == null) throw new IOException("Battery state unavailable");
            int level = battery.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
            int scale = battery.getIntExtra(BatteryManager.EXTRA_SCALE, 0);
            if (level < 0 || scale <= 0) throw new IOException("Battery level unavailable");
            int percent = level * 100 / scale;
            if (percent >= limit) paused = true;
            else if (percent <= limit - 2) paused = false;
        }
        boolean enabled = !paused;
        BufferedReader reader = new BufferedReader(new FileReader(PATH));
        boolean current;
        try { current = "1".equals(reader.readLine()); }
        finally { reader.close(); }
        if (current != enabled) {
            FileWriter writer = new FileWriter(PATH);
            try { writer.write(enabled ? "1" : "0"); }
            finally { writer.close(); }
        }
        if (preferences.getBoolean(PAUSED, false) != paused) {
            preferences.edit().putBoolean(PAUSED, paused).commit();
        }
        return enabled;
    }

    static void applySelection(Context context, int limit) throws IOException {
        Intent battery = context.registerReceiver(null, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
        apply(context, limit, battery);
    }
}
