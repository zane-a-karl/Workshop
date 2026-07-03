#!/bin/bash

# convert jpg to png
# for img in $1/*.jpg
# do
#     cp $img $1/$( basename -s .jpg $img ).png
# done

# make a backup of dir files
# if [ ! -d /backup/ ]
# then
#    mkdir backup
# fi
# for file in $1/*
# do
#     used=$( df $1 | tail -1 | awk '{print $5}' | sed "s;%;;g" )
#     if [ $used -gt 90 ]
#     then
# 	echo Low disk space 1>&2
# 	break
#     fi
#     if [ -d $file ]
#     then
# 	echo "Not backing up dirs($file) just files"
# 	continue
#     fi
#     cp $file $1/backup/
# done

# choose a char from southpark
# names='Kyle Cartman Stan Quit'
# PS3='Select character: '

# select name in $names
# do
#     if [ $name == 'Quit' ]
#     then
# 	break
#     fi
#     echo "Hello $name"
# done

# echo Bye

# even or odd
# for i in {1..10}
# do
#     if (( $i % 2 == 0 ))
#     then
# 	echo "$i is even"
#     else
# 	echo "$i is odd"
#     fi
# done

# echo all done

# print diff depending on file or dir input
if [ -d $1 ]
then
    ls $1
    nfiles=$( ls $1 | wc -l )
    echo $1 has $nfiles files.
else
    ls -lh $1 | awk -F" " '{print $5}'
fi


