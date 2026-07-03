#!/bin/bash

box_width=50
box_height=10

c=$(tput cols)
r=$(tput lines)

c_center=$(( c/2 ))
r_center=$(( r/2 ))
c_upper_left=$(( c_center - ((box_width/2)) ))
r_upper_left=$(( r_center - ((box_height /2)) ))

get_cursor_row()   { IFS=';' read -sdR -p $'\E[6n' ROW COL; echo ${ROW#*[}; }
get_cursor_col()   { IFS=';' read -sdR -p $'\E[6n' ROW COL; echo ${COL}; }										 

tput cup $(( r_upper_left )) $(( c_upper_left ))
tput setab 4 #blue bg
tput setaf 7 #white bg
row=$(get_cursor_row)
col=$(get_cursor_col)

for row_offset in $(seq 0 $box_height);
do
		box_line=""
		for col_offset in $(seq 0 $box_width);
		do
				#tput cup $(( row + row_offset )) $(( col + col_offset ))
				#echo -n " "
				box_line+=" "
		done
		tput cup $(( row + row_offset )) $(( col ))
		echo -n "$box_line"
		box_line=""
done

tput setab 0 #black bg
tput cup $(get_cursor_row) 0
