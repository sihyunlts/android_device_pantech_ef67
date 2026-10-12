EF67_DT_IMAGE := $(PRODUCT_OUT)/dt.img
EF67_DT_STAGE := $(PRODUCT_OUT)/obj/EF67_DTBS
EF67_DTB_TOOL := $(HOST_OUT_EXECUTABLES)/dtbToolEF67

$(EF67_DT_IMAGE): $(INSTALLED_KERNEL_TARGET) $(EF67_DTB_TOOL)
	$(hide) mkdir -p $(EF67_DT_STAGE)
	$(hide) cp $(KERNEL_OUT)/arch/arm/boot/dts/apq8084-ef67s.dtb $(EF67_DT_STAGE)/
	$(hide) cp $(KERNEL_OUT)/arch/arm/boot/dts/apq8084-ef67l.dtb $(EF67_DT_STAGE)/
	$(hide) $(EF67_DTB_TOOL) -2 -s $(BOARD_KERNEL_PAGESIZE) -p $(KERNEL_OUT)/scripts/dtc/ -o $@ $(EF67_DT_STAGE)/

$(INSTALLED_BOOTIMAGE_TARGET): $(MKBOOTIMG) $(INTERNAL_BOOTIMAGE_FILES) $(EF67_DT_IMAGE)
	$(hide) $(MKBOOTIMG) $(INTERNAL_BOOTIMAGE_ARGS) $(BOARD_MKBOOTIMG_ARGS) --dt $(EF67_DT_IMAGE) --output $@
	$(hide) $(call assert-max-file-size,$@,$(BOARD_BOOTIMAGE_PARTITION_SIZE),raw)

ifneq ($(INSTALLED_RECOVERYIMAGE_TARGET),)
$(INSTALLED_RECOVERYIMAGE_TARGET): $(MKBOOTIMG) $(recovery_ramdisk) $(recovery_kernel) $(EF67_DT_IMAGE)
	$(hide) $(MKBOOTIMG) $(INTERNAL_RECOVERYIMAGE_ARGS) $(BOARD_MKBOOTIMG_ARGS) --dt $(EF67_DT_IMAGE) --output $@
	$(hide) $(call assert-max-file-size,$@,$(BOARD_RECOVERYIMAGE_PARTITION_SIZE),raw)
endif
