package com.pantech.settings.device;

import android.content.Context;
import android.provider.Settings;

final class SecretKey {
    static final int DEFAULT_PRESS = 7;
    static final int DEFAULT_LONG_PRESS = 0;

    static void initializeDefaults(Context context) {
        if (Settings.System.getString(context.getContentResolver(),
                Settings.System.ASSIST_WAKE_SCREEN) == null) {
            Settings.System.putInt(context.getContentResolver(),
                    Settings.System.ASSIST_WAKE_SCREEN, 1);
        }
        if (Settings.System.getString(context.getContentResolver(),
                Settings.System.KEY_ASSIST_ACTION) == null) {
            Settings.System.putInt(context.getContentResolver(),
                    Settings.System.KEY_ASSIST_ACTION, DEFAULT_PRESS);
        }
        if (Settings.System.getString(context.getContentResolver(),
                Settings.System.KEY_ASSIST_LONG_PRESS_ACTION) == null) {
            Settings.System.putInt(context.getContentResolver(),
                    Settings.System.KEY_ASSIST_LONG_PRESS_ACTION, DEFAULT_LONG_PRESS);
        }
    }
}
