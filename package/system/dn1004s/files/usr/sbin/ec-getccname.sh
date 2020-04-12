#!/bin/sh
CCIP=`ec-getccip.sh $1`
if [ ! -z $CCIP ]; then
	CCNAME=`curl -s http://$CCIP:8008/ssdp/device-desc.xml | sed -n -e 's/.*<friendlyName>\(.*\)<\/friendlyName>.*/\1/p'`
	if [ $? -eq 0 ]; then
		if [ -z "$CCNAME" ]; then
			echo "_NONE_"
		else
			echo $CCNAME
		fi
	else
		echo "_NONE_"
	fi
else
	echo "_NONE_"
fi


