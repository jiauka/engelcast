#!/bin/sh
. /usr/sbin/ec-functions.sh
if [ $# -eq 0 ]; then
	ID=0
fi
if [ $# -eq 1 ]; then
	ID=$1
fi
CCIP=$(ccip $ID)
MAC=`uci show dhcp.@host[$ID].mac | cut -d '=' -f 2 |tr -d "'"`
if [  -z $CCIP ]; then
	exit 1
fi
fping -c1 -t51 $CCIP &>/dev/null
if [ $? -eq 0 ];
then
	echo "$CCIP"
fi
