#!/bin/bash
# flash_board_dbus_driver.sh - Flash DBus Driver on FRDM-RW612 via J-Link

ELF_FILE="/home/wis3re/dbus_porting/zephyr_dbus_driver/build/zephyr/zephyr.elf"

if [ ! -f "$ELF_FILE" ]; then
    echo "Error: ELF file not found at $ELF_FILE"
    exit 1
fi

TMP_SCRIPT=$(mktemp /tmp/jlink_script.XXXX.jlink)

cat <<EOL > $TMP_SCRIPT
device RW612
if SWD
speed 1000
connect
erase
loadfile $ELF_FILE
r
g
exit
EOL

echo "Flashing board with $ELF_FILE ..."

JLinkExe -CommanderScript $TMP_SCRIPT

rm $TMP_SCRIPT

echo "Flashing complete. Board should now be running DBus Driver."