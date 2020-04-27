#!/bin/sh
i=0
while [ 1 ]
do
    let i++
    logger "engelcast" $i
    echo "engelcast" $i
  
   sleep 1
done
