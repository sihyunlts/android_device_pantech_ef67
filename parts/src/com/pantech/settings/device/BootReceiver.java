package com.pantech.settings.device;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public final class BootReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(final Context context, Intent intent) {
        if (!Intent.ACTION_BOOT_COMPLETED.equals(intent.getAction())) return;
        final PendingResult pending = goAsync();
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    int mode = TouchMode.preferences(context).getInt(TouchMode.KEY, 0);
                    int result = TouchMode.write(mode);
                    if (result != 0) Log.e("VegaSettings", "Cannot restore touch mode: " + result);
                } finally {
                    pending.finish();
                }
            }
        }, "RestoreTouchMode").start();
    }
}
