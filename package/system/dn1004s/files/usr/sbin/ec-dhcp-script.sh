#!/bin/sh
. /usr/sbin/ec-functions.sh
logger "[$0] $LINENO $1 $2 $3 $4 $DNSMASQ_SUPPLIED_HOSTNAME" 
NUMBEROFFC=$(getnumberofcc)
if [ "$DNSMASQ_SUPPLIED_HOSTNAME" == "Chromecast" ]; 
	then
	replaceMAC $2 $3
	ID=$(findMAC $2)
	if [ $? -eq 0 ]; then
		CCIP=`ec-getccip.sh $ID`
		if [ $ID -lt $NUMBEROFFC ]; then
			waitCCActive $ID 30 # wait up to 30 second for CC device CCDeviceNumber
			ec-setccname.sh $ID
		fi
		/etc/init.d/roomy-repeater restart
	fi
fi
