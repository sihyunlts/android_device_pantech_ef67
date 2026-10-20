package com.pantech.settings.device;

import android.content.Context;
import lineageos.providers.LineageSettings;

final class SecretKey {
    static final int DEFAULT_PRESS = 7;
    static final int DEFAULT_LONG_PRESS = 0;

    static void initializeDefaults(Context context) {
        if (LineageSettings.System.getString(context.getContentResolver(),
                LineageSettings.System.ASSIST_WAKE_SCREEN) == null) {
            LineageSettings.System.putInt(context.getContentResolver(),
                    LineageSettings.System.ASSIST_WAKE_SCREEN, 1);
        }
        if (LineageSettings.System.getString(context.getContentResolver(),
                LineageSettings.System.KEY_ASSIST_ACTION) == null) {
            LineageSettings.System.putInt(context.getContentResolver(),
                    LineageSettings.System.KEY_ASSIST_ACTION, DEFAULT_PRESS);
        }
        if (LineageSettings.System.getString(context.getContentResolver(),
                LineageSettings.System.KEY_ASSIST_LONG_PRESS_ACTION) == null) {
            LineageSettings.System.putInt(context.getContentResolver(),
                    LineageSettings.System.KEY_ASSIST_LONG_PRESS_ACTION, DEFAULT_LONG_PRESS);
        }
    }
}
