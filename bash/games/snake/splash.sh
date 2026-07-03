#!/bin/bash

welcome="Welcome to Snake!"
press="Press any key to begin"

width=$(tput cols)
height=$(tput lines)

mid_x=$(( width/2 - ${#welcome}/2 ))
mid_y=$(( height/2 ))

clear

tput cup 0 0
echo "High Score: 0"

tput cup $mid_y $mid_x
echo $welcome

mid_y=$(( $mid_y+1 ))
mid_x=$(( width/2 - ${#press}/2 ))
tput cup $mid_y $mid_x
read -n 1 -r -s -p "$press"
clear
echo "Splash"
