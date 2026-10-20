# qmuxd runs as radio and needs kernel wake locks.
[system/bin/qmuxd]
mode: 0755
user: AID_ROOT
group: AID_SHELL
caps: BLOCK_SUSPEND
