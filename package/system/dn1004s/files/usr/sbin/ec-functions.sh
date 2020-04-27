#!/bin/sh

getccdisabled () {
	if [ -z $1 ]; then
		return 
	fi
	DISABLED=`uci show dhcp.@host[$1].disabled | cut -d '=' -f 2 |tr -d "'" 2>/dev/null`
	if [ -z $DISABLED ]; then
		echo 0
		return 0
	fi
	echo $DISABLED
	return 0
}

#get number of active cc
getnumberofcc () {
    echo "1"
}

findMAC () {
	if [ -z $1 ]; then
		return ""
	fi
	ID=0
	NUMBEROFFC=$(getnumberofcc)
	while [ $ID -lt $NUMBEROFFC ]
	do
		MAC=`uci show dhcp.@host[$ID].mac | cut -d '=' -f 2 |tr -d "'" 2>/dev/null`
		if [ ! -z $MAC ]; then
			if [ "x$MAC" == "x$1" ]; then
					echo $ID
					return 0
			fi
		fi
		true $(( ID++ ))
	done
	return 1
	
}

#get ccci from uci
ccip () {
	CCIP=`uci show dhcp.@host[$1].ip | cut -d '=' -f 2 |tr -d "'" 2>/dev/null`
	if [ ! -z "$CCIP" ]; then
		echo $CCIP	
		return 0
	else
		echo ""
		return 1
		
	fi
}

replaceMAC () {
	if [ $# -ne 2 ]; then
		return 1
	fi

	# check if it already exists
	ID=0
	logger "[replaceMAC]  $1 $2" 
	MAC=`uci show dhcp.@host[0].mac|cut  -d  '=' -f 2 |tr -d "'" 2>/dev/null`
    if [ ! -z $MAC ]; then
        if [ "x$MAC" == "x$1" ]; then
            IP=$(ccip $ID)
			#logger "[replaceMAC] $LINENO $MAC $IP" 
            if [ "x$IP" != "x$2" ]; then #new IP
                uci set dhcp.@host[$ID].ip="$2"
				uci commit dhcp
            fi
			echo $ID
			return 0 #already there
		fi
    fi
	ID=0
    MAC=`uci show dhcp.@host[$ID].mac|cut  -d  '=' -f 2 |tr -d "'" 2>/dev/null`
    if [ ! -z $MAC ]; then
        uci set dhcp.@host[$ID].mac="$1"
        uci set dhcp.@host[$ID].ip="$2"
        uci commit dhcp
        echo $ID
        return 0
    fi
	return 1
}

	
# waitCCActive device seconds
#returns 0 if found
# 1 id not found 
waitCCActive () { 
	if [ $# -ne 2  ]; then
		return 1
	fi
	CCNAME=`ec-getccname.sh $1`
	cnt=0
	while [ "$CCNAME" == "_NONE_" ]
	do	
		
		CCNAME=`ec-getccname.sh $1`
		if [ $cnt -eq $2 ];
		then
			return 1 #timeout
		fi
		sleep 1
		cnt=$(( $cnt + 1 ))
	done
	return 0 #found
}

	
# waitCCActive device seconds
#returns 0 if found
# 1 id not found 
waitCCActive () { 
	if [ $# -ne 2  ]; then
		return 1
	fi
	CCNAME=`ec-getccname.sh $1`
	cnt=0
	while [ "$CCNAME" == "_NONE_" ]
	do	
		
		CCNAME=`ec-getccname.sh $1`
		if [ $cnt -eq $2 ];
		then
			return 1 #timeout
		fi
		sleep 1
		cnt=$(( $cnt + 1 ))
	done
	return 0 #found
}
