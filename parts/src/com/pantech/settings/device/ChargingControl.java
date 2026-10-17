package com.pantech.settings.device;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.BatteryManager;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

final class ChargingControl {
    static final ExecutorService WORKER = Executors.newSingleThreadExecutor();
    static final String LIMIT = "charging_limit";
    private static final String PAUSED = "charging_limit_paused";
    private static final String PATH = "/sys/class/power_supply/battery/charge_enabled";
    // Accessed only on WORKER. Thermal control can change the reported hardware state.
    private static Boolean sRequestedEnabled;

    static boolean available() { return new File(PATH).canWrite(); }

    static int limit(Context context) {
        return Integer.parseInt(TouchMode.preferences(context).getString(LIMIT, "0"));
    }

    static void apply(Context context, int limit, Intent battery) throws IOException {
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
        if (sRequestedEnabled == null || sRequestedEnabled.booleanValue() != enabled) {
            // A failed write can still change the kernel's saved request.
            sRequestedEnabled = null;
            FileWriter writer = new FileWriter(PATH);
            try { writer.write(enabled ? "1" : "0"); }
            finally { writer.close(); }
            sRequestedEnabled = enabled;
        }
        if (preferences.getBoolean(PAUSED, false) != paused) {
            preferences.edit().putBoolean(PAUSED, paused).commit();
        }
    }

    static void applySelection(Context context, int limit) throws IOException {
        Intent battery = context.registerReceiver(null, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
        apply(context, limit, battery);
        TouchMode.preferences(context).edit().putString(LIMIT, Integer.toString(limit)).commit();
    }
}
