package com.pantech.settings.device;

import android.content.Context;
import android.os.PowerManager;
import android.view.KeyEvent;

import com.android.internal.os.DeviceKeyHandler;

public final class SecretKeyHandler implements DeviceKeyHandler {
    private final PowerManager mPowerManager;
    private boolean mWakePress;

    public SecretKeyHandler(Context context) {
        mPowerManager = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
    }

    @Override
    public KeyEvent handleKeyEvent(KeyEvent event) {
        if (event.getKeyCode() != KeyEvent.KEYCODE_ASSIST) {
            return event;
        }
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            if (event.getRepeatCount() == 0) {
                mWakePress = !mPowerManager.isInteractive();
                return event;
            }
            return mWakePress ? null : event;
        }
        boolean consume = mWakePress;
        mWakePress = false;
        return consume ? null : event;
    }
}
