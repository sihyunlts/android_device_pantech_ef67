package com.pantech.settings.device;

import android.content.Context;
import android.content.SharedPreferences;

final class TouchMode {
    static final String KEY = "touch_mode";

    static {
        System.loadLibrary("vega_settings");
    }

    static native int read();
    static native int write(int mode);

    static SharedPreferences preferences(Context context) {
        return context.getSharedPreferences("vega_features", Context.MODE_PRIVATE);
    }
}
