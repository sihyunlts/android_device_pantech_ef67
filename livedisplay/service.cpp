#include <hidl/HidlTransportSupport.h>
#include "DisplayColorCalibration.h"

int main() {
    using vendor::lineage::livedisplay::V2_0::sysfs::DisplayColorCalibration;
    android::hardware::configureRpcThreadpool(1, true);
    android::sp<DisplayColorCalibration> calibration = new DisplayColorCalibration();
    if (calibration->registerAsService() != android::OK) return 1;
    android::hardware::joinRpcThreadpool();
    return 1;
}
