package com.pantech.settings.device;

import android.content.Context;
import cyanogenmod.providers.CMSettings;

final class SecretKey {
    static final int DEFAULT_PRESS = 7;
    static final int DEFAULT_LONG_PRESS = 0;

    static void initializeDefaults(Context context) {
        if (CMSettings.System.getString(context.getContentResolver(),
                CMSettings.System.ASSIST_WAKE_SCREEN) == null) {
            CMSettings.System.putInt(context.getContentResolver(),
                    CMSettings.System.ASSIST_WAKE_SCREEN, 1);
        }
        if (CMSettings.System.getString(context.getContentResolver(),
                CMSettings.System.KEY_ASSIST_ACTION) == null) {
            CMSettings.System.putInt(context.getContentResolver(),
                    CMSettings.System.KEY_ASSIST_ACTION, DEFAULT_PRESS);
        }
        if (CMSettings.System.getString(context.getContentResolver(),
                CMSettings.System.KEY_ASSIST_LONG_PRESS_ACTION) == null) {
            CMSettings.System.putInt(context.getContentResolver(),
                    CMSettings.System.KEY_ASSIST_LONG_PRESS_ACTION, DEFAULT_LONG_PRESS);
        }
    }
}
