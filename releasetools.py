def FullOTA_InstallEnd(info):
    # qmuxd runs as radio and needs CAP_BLOCK_SUSPEND for kernel wake locks.
    info.script.Mount("/system")
    info.script.SetPermissions("/system/bin/qmuxd", 0, 2000, 0755,
                               None, "0x1000000000")


def IncrementalOTA_InstallEnd(info):
    FullOTA_InstallEnd(info)
