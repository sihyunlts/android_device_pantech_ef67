package com.pantech.settings.device;

import android.app.KeyguardManager;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.PowerManager;
import android.os.UserHandle;
import android.provider.Settings;
import android.util.Log;

import java.io.FileOutputStream;
import java.io.IOException;

/** Connects the standard system-navigation setting to the EF67 sensor. */
public final class FingerprintGestureService extends Service {
    private static final String CONTROL =
            "/sys/bus/spi/devices/spi7.0/nav_settings/system_navigation";
    private HandlerThread mThread;
    private Handler mHandler;
    private ContentObserver mObserver;
    private KeyguardManager mKeyguard;
    private PowerManager mPower;
    private final Runnable mUpdate = new Runnable() {
        @Override public void run() {
            boolean enabled = Settings.Secure.getIntForUser(getContentResolver(),
                    Settings.Secure.SYSTEM_NAVIGATION_KEYS_ENABLED, 0,
                    UserHandle.USER_CURRENT) != 0;
            setNavigation(enabled && mPower.isInteractive() && !mKeyguard.isKeyguardLocked());
        }
    };
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override public void onReceive(Context context, Intent intent) {
            if (Intent.ACTION_USER_SWITCHED.equals(intent.getAction())) {
                notifyNavigationSettingChanged();
            }
            mHandler.post(mUpdate);
        }
    };

    private void notifyNavigationSettingChanged() {
        // Oreo reads Secure but observes Global for this setting. Notify that
        // observer without creating a duplicate Global setting or syncing data.
        getContentResolver().notifyChange(Settings.Global.getUriFor(
                Settings.Secure.SYSTEM_NAVIGATION_KEYS_ENABLED), null, 0);
    }

    private void setNavigation(boolean enabled) {
        try (FileOutputStream output = new FileOutputStream(CONTROL)) {
            output.write(enabled ? '1' : '0');
        } catch (IOException e) {
            Log.e("FingerprintGestures", "Cannot update sensor navigation", e);
        }
    }

    @Override public void onCreate() {
        super.onCreate();
        mKeyguard = (KeyguardManager) getSystemService(KEYGUARD_SERVICE);
        mPower = (PowerManager) getSystemService(POWER_SERVICE);
        mThread = new HandlerThread("FingerprintGestures");
        mThread.start();
        mHandler = new Handler(mThread.getLooper());
        mObserver = new ContentObserver(mHandler) {
            @Override public void onChange(boolean selfChange) {
                notifyNavigationSettingChanged();
                mUpdate.run();
            }
        };
        getContentResolver().registerContentObserver(Settings.Secure.getUriFor(
                Settings.Secure.SYSTEM_NAVIGATION_KEYS_ENABLED), false, mObserver,
                UserHandle.USER_ALL);
        IntentFilter filter = new IntentFilter(Intent.ACTION_SCREEN_OFF);
        filter.addAction(Intent.ACTION_SCREEN_ON);
        filter.addAction(Intent.ACTION_USER_PRESENT);
        filter.addAction(Intent.ACTION_USER_SWITCHED);
        registerReceiver(mReceiver, filter);
        notifyNavigationSettingChanged();
        mHandler.post(mUpdate);
    }

    @Override public int onStartCommand(Intent intent, int flags, int startId) {
        mHandler.post(mUpdate);
        return START_STICKY;
    }

    @Override public void onDestroy() {
        unregisterReceiver(mReceiver);
        getContentResolver().unregisterContentObserver(mObserver);
        mHandler.removeCallbacksAndMessages(null);
        mHandler.post(new Runnable() {
            @Override public void run() { setNavigation(false); }
        });
        mThread.quitSafely();
        super.onDestroy();
    }

    @Override public IBinder onBind(Intent intent) { return null; }
}
