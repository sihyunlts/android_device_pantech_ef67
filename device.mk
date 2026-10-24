# The kernel's FunctionFS endpoints use synchronous I/O.
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += sys.usb.ffs.aio_compat=true

# TWRP also writes the gadget vendor ID when switching MTP.
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += usb.vendor=10a9
