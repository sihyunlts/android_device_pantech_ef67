package com.pantech.settings.device;

import android.app.ActionBar;
import android.os.AsyncTask;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.style.TextAppearanceSpan;
import android.widget.Toast;
import android.view.MenuItem;

public final class VegaSettings extends PreferenceActivity
        implements Preference.OnPreferenceChangeListener {
    private ListPreference mTouchMode;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ActionBar actionBar = getActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
        }
        addPreferencesFromResource(R.xml.vega_settings);
        mTouchMode = (ListPreference) findPreference(TouchMode.KEY);
        String[] names = getResources().getStringArray(R.array.touch_mode_entries);
        String[] descriptions = getResources().getStringArray(R.array.touch_mode_summaries);
        CharSequence[] entries = new CharSequence[names.length];
        for (int i = 0; i < names.length; i++) {
            SpannableString entry = new SpannableString(names[i] + "\n" + descriptions[i]);
            entry.setSpan(new TextAppearanceSpan(this, android.R.style.TextAppearance_Small),
                    names[i].length() + 1, entry.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            entries[i] = entry;
        }
        mTouchMode.setEntries(entries);
        mTouchMode.setOnPreferenceChangeListener(this);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onResume() {
        super.onResume();
        updateMode();
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
        final int mode = Integer.parseInt((String) value);
        mTouchMode.setEnabled(false);
        new AsyncTask<Void, Void, Integer>() {
            @Override
            protected Integer doInBackground(Void... unused) {
                int result = TouchMode.write(mode);
                if (result == 0) {
                    TouchMode.preferences(getApplicationContext()).edit()
                            .putInt(TouchMode.KEY, mode).commit();
                }
                return result;
            }

            @Override
            protected void onPostExecute(Integer result) {
                if (isFinishing() || isDestroyed()) return;
                updateMode();
                if (result != 0) {
                    Toast.makeText(VegaSettings.this, R.string.touch_mode_failed,
                            Toast.LENGTH_SHORT).show();
                }
            }
        }.execute();
        return false;
    }
}
