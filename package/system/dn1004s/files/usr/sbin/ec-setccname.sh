#!/bin/sh
. /usr/sbin/ec-functions.sh
if [ $# -eq 0 ]; then
	ID=0
	NEWNAME=`uci show wireless.@wifi-iface[$ID]|grep ssid|cut  -d  '=' -f 2 |tr -d "'"`
fi
if [ $# -eq 1 ]; then
	if echo $1 | egrep -q '^[0-9]+$'; then
		ID=$1
		NEWNAME=`uci show wireless.@wifi-iface[$ID]|grep ssid|cut  -d  '=' -f 2 |tr -d "'"`
	else
		ID=0
		NEWNAME=$1
	fi
fi
if [ $# -eq 2 ]; then
	ID=$1
	NEWNAME=$2
fi
echo  "newname[$ID] $ID $NEWNAME"
waitCCActive $ID
CCIP=`ec-getccip.sh $ID`

if [ -z $CCIP ]; then
	exit 0
fi
if [ "x$NEWNAME" != "x$CCNAME" ]; then
#	curl -s -X POST -H "Content-Type: application/json" -d '{"name": '\"$NEWNAME\"'}' http://$CCIP:8008/setup/set_eureka_info
    curl -k -X POST -H "Content-Type: application/json" -H "Authorization: roomy.cast" -d '{"name": '\"$NEWNAME\"',"settings":{"timezone":"Europe\/Madrid","time_format":2,"locale":"es"},"opt_in":{"stats":false}}' https://$CCIP:8443/setup/set_eureka_info
	/etc/init.d/roomy-repeater restart
fi
