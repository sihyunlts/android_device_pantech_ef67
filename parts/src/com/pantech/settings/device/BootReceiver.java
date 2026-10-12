package com.pantech.settings.device;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public final class BootReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        if (Intent.ACTION_BOOT_COMPLETED.equals(intent.getAction())) {
            SecretKey.initializeDefaults(context);
            context.startService(new Intent(context, PenService.class));
            context.startService(new Intent(context, ChargeLimitService.class));
        }
    }
}
