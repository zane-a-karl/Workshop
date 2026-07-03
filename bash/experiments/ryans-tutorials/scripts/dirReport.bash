#!/bin/bash

# How many files are in the directory?
nfiles=`ls -l $1 | wc -l`
echo There are $nfiles files in $1

# How many directories are in the directory?
ndirs=`ls -l $1 | grep '^d' | wc -l`
echo There are $ndirs dirs in $1

# What is the biggest file?
bigfileinfo=`ls -Sl $1 | tail +2 | head -1 | awk '{print $5 " " $9}'`
bigfilebytes=`echo $bigfileinfo | sed 's; ;\n;' | head -1`
bigfilename=`echo $bigfileinfo | sed 's; ;\n;' | tail -1`
echo the biggest file is $bigfilename with $bigfilebytes bytes.

# What is the most recently modified or created file?
recfileinfo=`ls -lt $1 | grep '^-' | head -1 | awk '{print $9 ":" $6 " " $7 " " $8}'`
recfilename=`echo $recfileinfo | sed 's;:;\n;' | head -1`
recfiledate=`echo $recfileinfo | sed 's;:;\n;' | tail -1`
echo $recfilename is the mostly recently mod/crea file with $recfiledate.

# A list of people who own files in the directory.
owners=`ls -l $1 | tail +2 | awk '{print $3}' | uniq`
echo The following own files within $1: $owners
