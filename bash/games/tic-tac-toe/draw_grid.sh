#!/bin/bash

# (1)I just want to draw a gird on the screen using "#"s
# (2)I want it to resize as I resize the screen

# (3)I want to be able to print characters into the
#middle of each gird space and select which space by using
#the arrow keys or WASD.

width=$(tput cols)
height=$(tput lines)

step_y=$((height / 5))
step_x=$((width / 5))

step_y_2=$((step_y / 2))
step_x_2=$((step_x / 2))

function draw_grid {

    clear
    width=$(tput cols)
    height=$(tput lines)
    
    step_y=$((height / 5))
    step_x=$((width / 5))
    local i=0
    local j=0
    # horizontal lines
    while [[ $i -lt $((1 + (5 * step_y))) ]]; do
				while [[ $j -lt $((1 + (5 * step_x))) ]]; do
						tput cup $i $j
						echo "#"
						((j += 2))
				done
				j=0
				((i += step_y))
    done
    i=0
    j=0
    # vertical lines
    while [[ $j -lt $((1 + (5 * step_x))) ]]; do
				while [[ $i -lt $((1 + (5 * step_y))) ]]; do
						tput cup $i $j
						echo "#"
						((i += 1))
				done
				i=0
				((j += step_x))
    done
}

trap draw_grid WINCH
draw_grid
# while true; do
#     sleep 2
# done



