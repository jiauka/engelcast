#!/bin/sh
if [ $1 == "0" ]; then
    echo "0" >/sys/devices/platform/leds-gpio/leds/dn1004s\:ethpow/brightness
else
    if [ "x$(cat /sys/devices/platform/leds-gpio/leds/dn1004s\:ethpow/brightness)" == "x0" ]; then
        echo "1" >/sys/devices/platform/leds-gpio/leds/dn1004s\:ethpow/brightness
        echo "0" >/sys/devices/platform/leds-gpio/leds/dn1004s\:ethrst/brightness
        echo "1" >/sys/devices/platform/leds-gpio/leds/dn1004s\:ethrst/brightness
        echo "0" >/sys/devices/platform/leds-gpio/leds/dn1004s\:ethrst/brightness
    fi
fi

