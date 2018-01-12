#! /bin/bash

BT_CLIENT="./BrickClient"

if [ "$#" -eq 0 ]
	then
		ARGS="-t b -a 50 -s 5 -d 10000"
	else
		ARGS="$@"
fi

source mac.sh &> /dev/null # Try to load mac address
ret_code=$?  

if [ "$ret_code" -ne "0" ]
	then	
		echo "Reading brick MAC address..."
		NXT_ADDR=$(NeXTTool /COM=usb -deviceinfo | grep Address | cut -f 2 -d =)
fi

if [ "$NXT_ADDR" == "" ]
	then
		echo "No brick found, is it connected to USB and turned on?"
		exit 1
fi 
echo "MAC: $NXT_ADDR"

read -p "Start application on the brick and press [Enter]..."
echo "Starting BT client..."
echo "Bluetooth PIN may be asked, just enter \"1234\""
$BT_CLIENT -m $NXT_ADDR $ARGS

