#!/bin/sh /etc/rc.common

. /lib/functions/network.sh
. /usr/sbin/ec-functions.sh
START=90
STOP=10

start() {
	local INTERFACES_PARAMS
    INTERFACES_PARAMS=""
	INTERFACES_PARAMS="eth1 "
	ID=0
	NUMBEROFCC=$(getnumberofcc)
	while [ $ID -lt $NUMBEROFCC ]
	do
		CCIP="$(ccip $ID)"
		DISABLED=$(getccdisabled $ID)
		if [ "x$DISABLED" = "x0" ]; then
			if [ "x$CCIP" != "x0.0.0.0" ]; then
                WIFIID=`uci get wireless.@wifi-iface[$ID].network |cut -d ' ' -f 1`      
				DEVICEID=`ifstatus $WIFIID|grep "\"device\"" |cut -d ':' -f 2|tr -d '", '`
                if [ "x$DEVICEID" = "x"  ] && [ "x$ISSUITE" = "x1" ]; then
                    DEVICEID="wlan0-$ID"
                fi
                if [ "x$DEVICEID" != "x"  ]; then
                    CCIP="$(ccip $ID)"
                    INTERFACES_PARAMS="$INTERFACES_PARAMS $DEVICEID:$CCIP"
                fi
			fi
		fi
		true $(( ID++ ))
	done
	/usr/sbin/roomy-repeater $INTERFACES_PARAMS
}

stop() {
  kill `cat /var/run/roomy-repeater.pid` &>/dev/null
  killall roomy-repeater
}
