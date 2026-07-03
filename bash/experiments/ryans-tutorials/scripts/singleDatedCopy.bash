#!/bin/bash

# first arg is a filename

# %<conversions> come from 'man 3 strftime'
today=$( date "+%Y-%m-%d" )

# make prefix date copy
cp $1 "${today}_$1"

#make suffix date copy
ext=$( echo $1 | awk -F "\." '{print $2}' )
file=$( basename $1 ".${ext}")

cp $1 "${file}_${today}.${ext}"

ls -lh
