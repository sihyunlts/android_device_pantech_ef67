package com.pantech.settings.device;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.IBinder;
import android.os.UEventObserver;
import android.os.Vibrator;
import android.util.Log;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public final class PenService extends Service
        implements SharedPreferences.OnSharedPreferenceChangeListener {
    static final String SHOW_NOTIFICATION = "pen_notification";
    static final String AUTO_MODE = "pen_auto_mode";
    private static final int NOTIFICATION_ID = 1;
    private static final String NOTIFICATION_CHANNEL_ID = "pen_status";
    private final Handler mHandler = new Handler();
    private Boolean mDetached;
    private SharedPreferences mPreferences;
    private NotificationManager mNotifications;
    private final UEventObserver mObserver = new UEventObserver() {
        @Override
        public void onUEvent(UEvent event) {
            final Boolean detached = parseState(event.get("SWITCH_STATE"));
            if (detached == null) return;
            mHandler.post(new Runnable() {
                @Override public void run() { updateState(detached, true); }
            });
        }
    };

    private static Boolean parseState(String state) {
        // S0226114 PhoneWindowManager sends connect=true for ON (inserted).
        if ("ON".equals(state)) return false;
        if ("OFF".equals(state)) return true;
        return null;
    }

    static Boolean readDetached() {
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(new FileReader(
                    "/sys/class/switch/touch_pen_detection/state"));
            String state = reader.readLine();
            return parseState(state == null ? null : state.trim());
        } catch (IOException e) {
            return null;
        } finally {
            if (reader != null) {
                try { reader.close(); } catch (IOException ignored) { }
            }
        }
    }

    @Override public void onCreate() {
        super.onCreate();
        mPreferences = TouchMode.preferences(this);
        mNotifications = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        mNotifications.createNotificationChannel(new NotificationChannel(
                NOTIFICATION_CHANNEL_ID, getString(R.string.v_pen),
                NotificationManager.IMPORTANCE_LOW));
        mPreferences.registerOnSharedPreferenceChangeListener(this);
        mObserver.startObserving("DEVPATH=/devices/virtual/switch/touch_pen_detection");
        updateState(readDetached(), false);
    }

    @Override public int onStartCommand(Intent intent, int flags, int startId) {
        return START_STICKY;
    }

    private void updateState(Boolean detached, boolean vibrate) {
        if (detached == null) {
            applyMode(true);
            return;
        }
        if (detached.equals(mDetached)) return;
        mDetached = detached;
        if (vibrate) {
            ((Vibrator) getSystemService(VIBRATOR_SERVICE)).vibrate(30);
        }
        applyMode(true);
        updateNotification();
    }

    private void applyMode(boolean automatic) {
        int selected = mPreferences.getInt(TouchMode.KEY, 0);
        int mode = automatic && Boolean.TRUE.equals(mDetached) && mPreferences.getBoolean(AUTO_MODE, false)
                ? 2 : selected;
        if (TouchMode.read() == mode) return;
        int result = TouchMode.write(mode);
        if (result != 0) Log.e("VegaSettings", "Cannot apply touch mode: " + result);
    }

    private void updateNotification() {
        if (!Boolean.TRUE.equals(mDetached) || !mPreferences.getBoolean(SHOW_NOTIFICATION, true)) {
            mNotifications.cancel(NOTIFICATION_ID);
            return;
        }
        PendingIntent settings = PendingIntent.getActivity(this, 0,
                new Intent(this, VegaSettings.class), PendingIntent.FLAG_UPDATE_CURRENT);
        Notification notification = new Notification.Builder(this, NOTIFICATION_CHANNEL_ID)
                .setSmallIcon(R.drawable.stat_sys_accessories_pen)
                .setContentTitle(getString(R.string.pen_detached))
                .setContentText(getString(R.string.pen_notification_text))
                .setContentIntent(settings).setOngoing(true).setOnlyAlertOnce(true)
                .setShowWhen(false).build();
        mNotifications.notify(NOTIFICATION_ID, notification);
    }

    @Override public void onSharedPreferenceChanged(SharedPreferences preferences, String key) {
        if (SHOW_NOTIFICATION.equals(key)) {
            mHandler.post(new Runnable() {
                @Override public void run() { updateNotification(); }
            });
        } else if (TouchMode.KEY.equals(key)) {
            mHandler.post(new Runnable() {
                @Override public void run() { applyMode(false); }
            });
        } else if (AUTO_MODE.equals(key)) {
            mHandler.post(new Runnable() {
                @Override public void run() { applyMode(true); }
            });
        }
    }

    @Override public void onDestroy() {
        mObserver.stopObserving();
        mPreferences.unregisterOnSharedPreferenceChangeListener(this);
        mHandler.removeCallbacksAndMessages(null);
        mNotifications.cancel(NOTIFICATION_ID);
        super.onDestroy();
    }

    @Override public IBinder onBind(Intent intent) { return null; }
}
