#!/bin/bash
set -euo pipefail

DEVICE_PATH="$(cd "$(dirname "$0")" && pwd)"
ANDROID_ROOT="$(cd "$DEVICE_PATH/../../.." && pwd)"
VENDOR_PATH="$ANDROID_ROOT/vendor/pantech/ef67"

if [[ $# -gt 1 || "${1:-}" == --help ]]; then
    echo "Usage: $0 [unpacked-stock-system-directory]"
    exit "$(( $# > 1 ))"
fi
if [[ ! -x "$VENDOR_PATH/fixup-blobs.sh" ]]; then
    echo "Check out vendor/pantech/ef67 before extracting stock files." >&2
    exit 1
fi
if ! command -v patchelf >/dev/null; then
    echo "patchelf is required to fix the extracted proprietary libraries." >&2
    exit 1
fi

SOURCE="${1:-adb}"
if [[ "$SOURCE" != adb ]]; then
    SOURCE="$(cd "$SOURCE" && pwd)"
fi

while IFS= read -r file || [[ -n "$file" ]]; do
    case "$file" in
        ''|\#*) continue ;;
    esac
    file="${file#-}"
    destination="$VENDOR_PATH/proprietary/$file"
    mkdir -p "$(dirname "$destination")"
    echo "Extracting /system/$file"
    if [[ "$SOURCE" == adb ]]; then
        adb pull "/system/$file" "$destination"
    else
        cp -p "$SOURCE/$file" "$destination"
    fi
done < "$DEVICE_PATH/proprietary-files.txt"

"$VENDOR_PATH/fixup-blobs.sh"
"$DEVICE_PATH/setup-makefiles.sh"
