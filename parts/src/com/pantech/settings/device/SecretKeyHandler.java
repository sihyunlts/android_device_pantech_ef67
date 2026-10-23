package com.pantech.settings.device;

import android.content.ContentResolver;
import android.content.Context;
import android.os.PowerManager;
import android.os.UserHandle;
import android.view.KeyEvent;

import lineageos.providers.LineageSettings;

import com.android.internal.os.DeviceKeyHandler;

public final class SecretKeyHandler implements DeviceKeyHandler {
    private final PowerManager mPowerManager;
    private final ContentResolver mResolver;
    private boolean mWakePress;

    public SecretKeyHandler(Context context) {
        mPowerManager = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        mResolver = context.getContentResolver();
    }

    @Override
    public KeyEvent handleKeyEvent(KeyEvent event) {
        if (event.getKeyCode() != KeyEvent.KEYCODE_ASSIST) {
            return event;
        }
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            if (event.getRepeatCount() == 0) {
                mWakePress = !mPowerManager.isInteractive();
                if (mWakePress && LineageSettings.System.getIntForUser(mResolver,
                        LineageSettings.System.ASSIST_WAKE_SCREEN, 1,
                        UserHandle.USER_CURRENT) != 0) {
                    mPowerManager.wakeUp(event.getEventTime(), "ef67:SECRET_KEY");
                }
            }
            return mWakePress ? null : event;
        }
        boolean consume = mWakePress;
        mWakePress = false;
        return consume ? null : event;
    }
}
