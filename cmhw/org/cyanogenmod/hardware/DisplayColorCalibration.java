package org.cyanogenmod.hardware;

import java.io.File;

import org.cyanogenmod.internal.util.FileUtils;

public class DisplayColorCalibration {
    private static final String CONTROL = "/sys/devices/platform/kcal_ctrl/kcal";

    public static boolean isSupported() {
        File control = new File(CONTROL);
        return control.canRead() && control.canWrite();
    }

    public static int getMinValue() {
        return 35;
    }

    public static int getMaxValue() {
        return 256;
    }

    public static int getDefValue() {
        return 256;
    }

    public static String getCurColors() {
        return FileUtils.readOneLine(CONTROL);
    }

    public static boolean setColors(String colors) {
        return FileUtils.writeLine(CONTROL, colors);
    }
}
