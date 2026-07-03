#!/bin/bash

cols=$( tput cols )
rows=$( tput lines )

msg=$@

input_len=${#msg}

half_input_len=$(( input_len / 2 ))

middle_col=$(( ($cols / 2) - $half_input_len ))
middle_row=$(( $rows / 2 ))

tput clear

tput cup $middle_row $middle_col
tput bold
echo $@
tput sgr0
tput cup $( tput lines ) 0
