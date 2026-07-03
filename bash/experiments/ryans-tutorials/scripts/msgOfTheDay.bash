#!/bin/bash

dow=$( date | awk -F" " '{print $1}' )

case $dow in
    "Mon")
	echo "I hate Mondays"
	;;
    "Tue")
	echo "Time to be productive"
	;;
    "Wed")
	echo "Happy hump day"
	;;
    "Thu")
	echo "Friday's nearly here"
	;;
    "Fri")
	echo "TGIF"
	;;
    *)
	echo "bliss"
	;;
esac
