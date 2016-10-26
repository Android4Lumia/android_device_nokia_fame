#!/system/bin/sh

setprop gsm.version.baseband `strings /dev/block/bootdevice/by-name/modem  | grep "M8930B-AAAATAZM" | head -1`
