package org.cyanogenmod.hardware;

import java.io.File;

import org.cyanogenmod.hardware.util.FileUtils;

public class TapToWake {
    private static final String CONTROL =
            "/sys/bus/i2c/devices/1-004a/double_tap_to_wake";

    public static boolean isSupported() {
        File control = new File(CONTROL);
        return control.canRead() && control.canWrite();
    }

    public static boolean isEnabled() {
        return "1".equals(FileUtils.readOneLine(CONTROL));
    }

    public static boolean setEnabled(boolean enabled) {
        return FileUtils.writeLine(CONTROL, enabled ? "1" : "0");
    }
}
