#!/bin/bash
# flash_board_dbus_driver.sh - Flash DBus Driver on FRDM-RW612 via J-Link

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ELF_FILE="${SCRIPT_DIR}/build/zephyr/zephyr.elf"
FULL_ERASE="${FULL_ERASE:-0}"

if [ ! -f "$ELF_FILE" ]; then
    echo "Error: ELF file not found at $ELF_FILE"
    exit 1
fi

TMP_SCRIPT=$(mktemp /tmp/jlink_script.XXXX.jlink)

if [ "$FULL_ERASE" = "1" ]; then
    ERASE_CMD="erase"
    echo "Full erase enabled (FULL_ERASE=1)."
else
    ERASE_CMD=""
    echo "Incremental flash mode (default, no full erase)."
fi

cat <<EOL > "$TMP_SCRIPT"
device RW612
if SWD
speed 1000
connect
${ERASE_CMD}
loadfile $ELF_FILE
r
g
exit
EOL

echo "Flashing board with $ELF_FILE ..."

JLINK_LOG="$(mktemp /tmp/jlink_output.XXXX.log)"
JLinkExe -CommanderScript "$TMP_SCRIPT" | tee "$JLINK_LOG"

if grep -Eq "FAILED:|Cannot connect|Could not connect|Connection failed" "$JLINK_LOG"; then
    echo "Error: J-Link flash failed. Check probe USB access/permissions and board connection." >&2
    rm -f "$JLINK_LOG"
    rm "$TMP_SCRIPT"
    exit 1
fi

rm -f "$JLINK_LOG"
rm "$TMP_SCRIPT"

echo "Flashing complete. Board should now be running DBus Driver."