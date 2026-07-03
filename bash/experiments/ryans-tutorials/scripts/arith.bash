#!/bin/bash

a=$( date -v +1d)
echo $a
echo ${#a}

b=$(( $1 + ($RANDOM % $2) ))
echo $b
