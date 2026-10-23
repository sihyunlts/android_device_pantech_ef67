# qmuxd runs as radio and needs kernel wake locks.
[system/bin/qmuxd]
mode: 0755
user: AID_ROOT
group: AID_SHELL
caps: BLOCK_SUSPEND

# File capabilities are required because this kernel lacks ambient capabilities.
[vendor/bin/hw/android.hardware.wifi@1.0-service]
mode: 0755
user: AID_WIFI
group: AID_WIFI
caps: NET_ADMIN NET_RAW SYS_MODULE

# Match rild.rc on the kernel without ambient capabilities.
[vendor/bin/hw/rild]
mode: 0755
user: AID_ROOT
group: AID_SHELL
caps: BLOCK_SUSPEND NET_ADMIN NET_RAW
