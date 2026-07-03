#!/bin/bash

if [ -d $1 ]
then
    echo $1 is a dir
elif [ -r $1 ]
then
     echo we can read $1
elif [ -x $1 ]
then
     echo we can exec $1
elif [ -w $1 ]
then
     echo we can write $1
elif [ -s $1 ]
then
     echo "$1 has stuff in it"
else
    echo $1 exists
fi
