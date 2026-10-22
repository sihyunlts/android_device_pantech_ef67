package com.pantech.settings.device;

import android.os.AsyncTask;
import android.os.Bundle;
import android.content.Context;
import android.content.Intent;
import lineageos.providers.LineageSettings;
import android.support.v7.preference.ListPreference;
import android.support.v7.preference.Preference;
import android.support.v14.preference.PreferenceFragment;
import android.support.v14.preference.SwitchPreference;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.style.TextAppearanceSpan;
import android.widget.Toast;
import java.io.IOException;

public final class VegaSettingsFragment extends PreferenceFragment
        implements Preference.OnPreferenceChangeListener {
    private ListPreference mTouchMode;
    private ListPreference mChargingLimit;
    private SwitchPreference mSecretWake;
    private ListPreference mSecretPress;
    private ListPreference mSecretLongPress;

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        getPreferenceManager().setSharedPreferencesName("vega_features");
        SecretKey.initializeDefaults(getActivity());
        getActivity().startService(new Intent(getActivity(), PenService.class));
        getActivity().startService(new Intent(getActivity(), ChargeLimitService.class));
        addPreferencesFromResource(R.xml.vega_settings);
        mTouchMode = (ListPreference) findPreference(TouchMode.KEY);
        String[] names = getResources().getStringArray(R.array.touch_mode_entries);
        String[] descriptions = getResources().getStringArray(R.array.touch_mode_summaries);
        CharSequence[] entries = new CharSequence[names.length];
        for (int i = 0; i < names.length; i++) {
            SpannableString entry = new SpannableString(names[i] + "\n" + descriptions[i]);
            entry.setSpan(new TextAppearanceSpan(getActivity(), android.R.style.TextAppearance_Small),
                    names[i].length() + 1, entry.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            entries[i] = entry;
        }
        mTouchMode.setEntries(entries);
        mTouchMode.setOnPreferenceChangeListener(this);
        mSecretWake = (SwitchPreference) findPreference(LineageSettings.System.ASSIST_WAKE_SCREEN);
        mSecretWake.setOnPreferenceChangeListener(this);
        mSecretPress = (ListPreference) findPreference(LineageSettings.System.KEY_ASSIST_ACTION);
        mSecretLongPress = (ListPreference) findPreference(LineageSettings.System.KEY_ASSIST_LONG_PRESS_ACTION);
        mChargingLimit = (ListPreference) findPreference(ChargingControl.LIMIT);
        mChargingLimit.setOnPreferenceChangeListener(this);
        mSecretPress.setOnPreferenceChangeListener(this);
        mSecretLongPress.setOnPreferenceChangeListener(this);
    }

    @Override
    public void onResume() {
        super.onResume();
        updateMode();
        mSecretWake.setChecked(LineageSettings.System.getInt(getActivity().getContentResolver(),
                LineageSettings.System.ASSIST_WAKE_SCREEN, 1) != 0);
        updateKeyAction(mSecretPress, SecretKey.DEFAULT_PRESS);
        updateKeyAction(mSecretLongPress, SecretKey.DEFAULT_LONG_PRESS);
        boolean chargingAvailable = ChargingControl.available();
        mChargingLimit.setEnabled(chargingAvailable);
        mChargingLimit.setSummary(chargingAvailable ? "%s"
                : getString(R.string.charging_unavailable));
    }

    private void updateKeyAction(ListPreference preference, int defaultAction) {
        int action = LineageSettings.System.getInt(getActivity().getContentResolver(), preference.getKey(), defaultAction);
        preference.setValue(Integer.toString(action));
        preference.setSummary(preference.getEntry());
    }

    private void updateMode() {
        int mode = TouchMode.read();
        boolean available = mode >= 0 && mode <= 2;
        mTouchMode.setEnabled(available);
        if (available) {
            mTouchMode.setValue(Integer.toString(mode));
            mTouchMode.setSummary(R.string.touch_mode_summary);
        } else {
            mTouchMode.setSummary(R.string.touch_mode_unavailable);
        }
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object value) {
        final Context context = getActivity().getApplicationContext();
        if (preference == mChargingLimit) {
            final String selection = (String) value;
            mChargingLimit.setEnabled(false);
            new AsyncTask<Void, Void, Boolean>() {
                @Override protected Boolean doInBackground(Void... unused) {
                    try {
                        ChargingControl.applySelection(context,
                                Integer.parseInt(selection));
                        return true;
                    } catch (IOException e) {
                        return false;
                    }
                }

                @Override protected void onPostExecute(Boolean success) {
                    if (!isAdded() || getActivity().isFinishing()) return;
                    mChargingLimit.setEnabled(ChargingControl.available());
                    if (success) {
                        mChargingLimit.setValue(selection);
                    } else {
                        Toast.makeText(getActivity(), R.string.charging_limit_failed,
                                Toast.LENGTH_SHORT).show();
                    }
                }
            }.executeOnExecutor(ChargingControl.WORKER);
            return false;
        }
        if (preference == mSecretWake) {
            if (LineageSettings.System.putInt(getActivity().getContentResolver(),
                    LineageSettings.System.ASSIST_WAKE_SCREEN, (Boolean) value ? 1 : 0)) {
                return true;
            }
            Toast.makeText(getActivity(), R.string.secret_key_failed, Toast.LENGTH_SHORT).show();
            return false;
        }
        if (preference == mSecretPress || preference == mSecretLongPress) {
            ListPreference key = (ListPreference) preference;
            int action = Integer.parseInt((String) value);
            if (LineageSettings.System.putInt(getActivity().getContentResolver(), key.getKey(), action)) {
                key.setValue((String) value);
                key.setSummary(key.getEntry());
            } else {
                Toast.makeText(getActivity(), R.string.secret_key_failed, Toast.LENGTH_SHORT).show();
            }
            return false;
        }
        final int mode = Integer.parseInt((String) value);
        mTouchMode.setEnabled(false);
        new AsyncTask<Void, Void, Integer>() {
            @Override
            protected Integer doInBackground(Void... unused) {
                int result = TouchMode.write(mode);
                if (result == 0) {
                    TouchMode.preferences(context).edit()
                            .putInt(TouchMode.KEY, mode).commit();
                }
                return result;
            }

            @Override
            protected void onPostExecute(Integer result) {
                if (!isAdded() || getActivity().isFinishing()) return;
                updateMode();
                if (result != 0) {
                    Toast.makeText(getActivity(), R.string.touch_mode_failed,
                            Toast.LENGTH_SHORT).show();
                }
            }
        }.execute();
        return false;
    }
}
