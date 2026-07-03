#!/bin/bash

# A simple demonstration script
# Ryan 8/3/2021
 
if [ $# != 1 ]
then
    echo Usage: A single argument which is the directory to backup
    exit
fi
if [ ! -d ./foo ]
then
    echo 'The given directory does not seem to exist (possible typo?)'
    exit
fi
date=`date +%F`
 
# Do we already have a backup folder for todays date?
if [ -d ./projectbackups/$1_$date ]
then
    echo 'This project has already been backed up today, overwrite?'
    read answer
    if [ $answer != 'y' ]
    then
        exit
    fi
else
    mkdir ./projectbackups/$1_$date
fi
cp -R ./foo/$1 ./projectbackups/$1_$date
echo Backup of $1 completed
