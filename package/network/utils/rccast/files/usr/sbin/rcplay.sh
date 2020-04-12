#!/bin/sh
. /usr/sbin/rcfunctions.sh

if [ $# -lt 3 ]; then
    echo "usage $0 ID URL MIME FORCE"
	exit 1
fi
CCisSleep=false
if [ $# -eq 4 ]; then
    notForce=false
else
    notForce=true

fi
ccpower 1
ID=$1
PORT=8009
URL=$2
MIME=$3
ps | grep "rccast$ID.pid" | grep -v grep | awk "{print \$1}" | xargs kill -9

while [ 1 ]
do
# Wait for a CC active and pingable
    while [ 1 ] 
    do
        CCIP=$(ccip $ID)
        if [  -z $CCIP ]; then
            #echo "NO CC available"
            CCisSleep=true            
        else
            fping -c1 -t51 $CCIP &>/dev/null
            if [ $? -eq 0 ];
            then
                if $CCisSleep; then
                    sleep 5
                fi
                break
            else
                CCisSleep=true            
            fi
        fi
        sleep 1
    done
    if $notForce; then
        rccast -p /tmp/rccast$ID.pid -s -c $CCIP:$PORT
    else
        :
    fi
    if [ $? -eq 0 ]; then
        #echo "CC is Idle"
        rccast -p /tmp/rccast$ID.pid -f -c $CCIP:$PORT -u $URL -m $MIME 
    fi
done
