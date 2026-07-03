#!/bin/bash

#  This is an implementation of the game Tic-Tac-Toe
# it's rules and assumptions are as follows:
#   - We will print a splash screen:
#     - There will be a title header and a menu with 3 options
#       - 1 Player (P1:<#wins>, CPU:<#wins>)
#       - 2 Player (P1:<#wins>, P2:<#wins>)
#       - Quit
#   - We will print a colored game board and a grid on that board
#   - At the game's end we will jump back to the splash screen
#     - where the title header will be replaced with the Win msg
#     - and the Menu will be displayed with updated <#wins>

############################################
### Global variables for art positioning ###
############################################
LINES=$(tput lines)
COLUMNS=$(tput cols)
board_height=$( echo "scale=2; 8*$LINES/10" | bc -l )
board_width=$( echo "scale=2; 5/2 * $board_height" | bc -l )
# Remove the decimal digits and the decimal point with
# the ${param%word} syntax which works the same as the
# ${param#word} syntax but matches word from the end as
# opposed to the beginning of param.
board_height=$( echo "${board_height%.*}" )
board_width=$( echo "${board_width%.*}" )

board_center_row=$(( $LINES/2 )) # row for the center point
board_top_left_row=$(( board_center_row - ((board_height /2)) ))
board_center_col=$(( $COLUMNS/2 )) # col for the center point
board_top_left_col=$(( board_center_col - ((board_width/2)) ))

grid_top_left_center_row=$(( $board_top_left_row + ($board_height/3)/2 ))
grid_top_left_center_col=$(( $board_top_left_col + $board_top_left_col/2 ))

############################################
#### Global variables for game keeping #####
############################################
gameboard=(0 1 2 3 4 5 6 7 8)
gameover=false
turn_ctr=0
num_p1_wins=0
num_p2_wins=0
num_cpu_wins=0

##############################################################
### little helpers for terminal print control and key input ##
##############################################################
ESC=$( printf "\033" )
CSI=$( printf "\x9B" )
cursor_blink_on()  { printf "$ESC[?25h"; }
cursor_blink_off() { printf "$ESC[?25l"; }
cursor_to()        { printf "$ESC[$1;${2:-1}H"; }
print_option()     { printf " $1 "; }
print_selected()   { printf "$ESC[7m $1 $ESC[27m"; }
# Changes the IFS
# Then calls 'read' using 'R' as a delimiter and
# takes advantage of the escape seq string $'E[6n' returning '\E[<row>;<col>R'
# to store '\E[<row>' in ROW and '<col>' in COL
# Then uses the ${param#word} syntax to match '*[' to the beginning of ROW
# and delete that matched portion returning only '<row>'
get_cursor_row()   { IFS=';';
										 read -sdR -p $'\E[6n' ROW COL;
										 echo "${ROW#*[}";
									 }
# Same as above but doesn't have to use ${param#word} syntax to grab <col>
get_cursor_col()   { IFS=';';
										 read -sdR -p $'\E[6n' ROW COL;
										 echo ${COL}; }
# Reads up to 3 characters of input or until it sees a delimiter
key_input()        { read -s -n3 key 2>/dev/null >&2;
										 if [[ $key = "$ESC[A" ]]; then echo up;    fi
										 if [[ $key = "$ESC[B" ]]; then echo down;  fi
										 if [[ $key = "$ESC[C" ]]; then echo right; fi
										 if [[ $key = "$ESC[D" ]]; then echo left;  fi										 
										 if [[ $key = ""       ]]; then echo enter; fi; }

# Renders a text based list of options that can be selected by the
# user using up, down, left, right, and enter keys and returns the chosen option.
#
#   Arguments   : list of options, maximum of 256
#                 "opt1" "opt2" ...
#   Return value: selected index (0 for opt1, 1 for opt2 ...)
function usr_select_TTT_option {

    # ensure cursor and input echoing back on upon a ctrl+c during read -s
    trap "cursor_blink_on; stty echo; printf '\n'; exit" SIGINT
    cursor_blink_off

    local selected=0
    while true; do
				# Redraw the colored/uncolored options each time an arrow key is pressed
        local idx=0
				for i in $( seq 0 2 ); do
						for j in $( seq 0 2 ); do
								row_inc=$(( $i * ($grid_top_left_center_row - $board_top_left_row) ))
								col_inc=$(( $j * 2 * ($grid_top_left_center_col - $board_top_left_col) ))
								tput cup $(( grid_top_left_center_row + 2*row_inc + 1 )) $(( grid_top_left_center_col + $col_inc ))
								if [ $idx -eq $selected ]; then
										print_selected " " #"${gameboard[$idx]}"
								else
										print_option " " #"${gameboard[$idx]}"
								fi
								((idx++))
						done
				done
        # user key control
        case $(key_input) in
            enter) break;;
            up)    ((selected-=3));
									 if [[ $selected -lt 0  ]]; then ((selected+=9)); fi;;
						down)  ((selected+=3));
									 if [[ $selected -ge $# ]]; then ((selected-=9)); fi;;
						right) ((selected++));
									 if [[ $((selected%3)) -eq 0 ]]; then ((selected-=3)); fi;;						
						left)  ((selected--));
									 if [[ $(( (selected+3)%3)) -eq 2 ]]; then ((selected+=3)); fi;;
        esac
    done

    # cursor position back to normal
    cursor_to $lastrow
    printf "\n"
    cursor_blink_on

    return $selected
}

####################################
#### FUNCTION TO DRAW THE BOARD ####
####################################
function draw_board {

		tput cup $(( board_top_left_row )) $(( board_top_left_col ))
		tput setab 7 #white bg
		tput setaf 0 #black fg
		row=$(get_cursor_row)
		col=$(get_cursor_col)

		for row_offset in $(seq 0 $board_height);
		do
				board_line=""
				for col_offset in $(seq 0 $board_width);
				do
						board_line+=" "
				done
				tput cup $(( row + row_offset )) $(( col ))
				echo -n "$board_line"
				board_line=""
		done

		tput cup $(get_cursor_row) 0
}

####################################
#### FUNCTION TO DRAW THE GRID #####
####################################
function draw_grid {

		# NOTE: There are a bunch of weird off-by-1 errros with respect to drawing
		# the grid lines. I think it is due to rounding of integers during the
		# global variable positioning arithmetic. Maybe checkout this link for
		# rounding with 'bc' and 'awk' https://stackoverflow.com/questions/2395284/round-a-divided-number-in-bash
		
		# Print horizontal lines
		for i in $( seq 0 $board_width );
		do
				tput cup $(( $board_top_left_row + 1 + $board_height/3 )) $(( $board_top_left_col + 1 + $i ))
				echo -n "-"
				tput cup $(( $board_top_left_row + 2 + (2 * $board_height/3) )) $(( $board_top_left_col + 1 + $i ))
				echo -n "-"
		done
		# Print vertical lines
		for i in $( seq 0 $board_height );
		do
				tput cup $(( $board_top_left_row + 1 + $i )) $(( $board_top_left_col + 1 + $board_width/3 ))
				echo -n "|"
				tput cup $(( $board_top_left_row + 1 + $i )) $(( $board_top_left_col + 2 + (2 * $board_width/3) ))
				echo -n "|"
		done
}

function print_title {

    title=$1
    tput cup $(( $LINES/4 )) $(( $COLUMNS/2 - ${#title}/2 ))
    echo $title
}

####################################################################
###################### GAME LOGIC FUNCTIONS ########################
####################################################################

function check_draw {

    if [[ $turn_ctr -ge 9 ]] && [[ $gameover = false ]]
    then
				return 0
    fi
    return 1
}

function check_win {

    if [[ ${board[0]} = $1 ]] && [[ ${board[1]} = $1 ]] && [[ ${board[2]} = $1 ]] ; then return 0; fi
    if [[ ${board[3]} = $1 ]] && [[ ${board[4]} = $1 ]] && [[ ${board[5]} = $1 ]] ; then return 0; fi
    if [[ ${board[6]} = $1 ]] && [[ ${board[7]} = $1 ]] && [[ ${board[8]} = $1 ]] ; then return 0; fi

    if [[ ${board[0]} = $1 ]] && [[ ${board[3]} = $1 ]] && [[ ${board[6]} = $1 ]] ; then return 0; fi
    if [[ ${board[1]} = $1 ]] && [[ ${board[4]} = $1 ]] && [[ ${board[7]} = $1 ]] ; then return 0; fi
    if [[ ${board[2]} = $1 ]] && [[ ${board[5]} = $1 ]] && [[ ${board[8]} = $1 ]] ; then return 0; fi

    if [[ ${board[0]} = $1 ]] && [[ ${board[4]} = $1 ]] && [[ ${board[8]} = $1 ]] ; then return 0; fi
    if [[ ${board[2]} = $1 ]] && [[ ${board[4]} = $1 ]] && [[ ${board[6]} = $1 ]] ; then return 0; fi

    return 1
}

function update_game {

		local selected=$1
		# Redraw the colored/uncolored options each time an arrow key is pressed
    local idx=0
		for i in $( seq 0 2 ); do
				for j in $( seq 0 2 ); do
						row_inc=$(( $i * ($grid_top_left_center_row - $board_top_left_row) ))
						col_inc=$(( $j * 2 * ($grid_top_left_center_col - $board_top_left_col) ))
						tput cup $(( grid_top_left_center_row + 2*row_inc + 1 )) $(( grid_top_left_center_col + $col_inc ))
						if [ $idx -eq $selected ]; then
								print_selected "${gameboard[$idx]}"
						else
								print_option "${gameboard[$idx]}"
						fi
						((idx++))
				done
		done

    check_win $usr_sym
    if [[ $? -eq 0 ]]
    then
				echo "Congrats you won!"
				echo "It took $turn_ctr moves"
				((num_p1_wins++))
				gameover=true
    fi
    check_win $cpu_sym
    if [[ $? -eq 0 ]]
    then
				echo "Sorry, you lost"
				((num_cpu_wins++))
				gameover=true
    fi
}

function validate_move {

    if [[ $1 -ge 0 ]] && [[ $1 -le 8 ]] && [[ ${board[$1]} != $usr_sym ]] && [[ ${board[$1]} != $cpu_sym ]]
    then
				return 0
    else
				return 1
    fi
}

function make_usr_move {

		str="P1"
		tput cup $(( $LINES/2 )) 5
		#		banner -w 30 "$str"
		echo "$str"
		usr_select_TTT_option
    choice=$?
    validate_move $choice $usr_sym
    local move_is_valid=$?
    if [[ $move_is_valid -ne 0 ]]
    then
				echo "Sorry that position is invalid, please try again."
				make_usr_move
    fi
    board[$choice]=$usr_sym
    update_game $choice
}

function make_cpu_move {

		str="CPU"
		tput cup $(( $LINES/2 )) 5
		#		banner -w 30 "$str"
		echo "$str"
    choice=$((RANDOM % 9))
    validate_move $choice $cpu_sym
    local move_is_valid=$?
    if [[ $move_is_valid -ne 0 ]]
    then
				make_cpu_move
    fi
    printf "%s" "Now it's the CPU's turn"
    sleep 1
		printf "%s" "."
		sleep 1
		printf "%s" "."
		sleep 1
		printf "%s" "."
    board[$choice]=$cpu_sym
    update_game $choice
}

function take_turns {

    while [[ $gameover != true ]]
    do
				if [[ $(( ((turn_ctr + heads_tails)) % 2)) -eq 0 ]]
				then
						make_cpu_move
				else
						make_usr_move
				fi
				((turn_ctr++))
				check_draw
				if [[ $? -eq 0 ]]
				then
						echo "It's a Tie! Rematch maybe?"
						gameover=true
				fi
    done
}

function flip_coin {

    local flip_animation=("_" "/" "|" "-" "\\" "_")
    echo -e "We're flipping a coin to see who goes first!\n\n"
		printf "\t%s\r" $frame
		sleep 1
    for frame in ${flip_animation[@]}
    do
				printf "\t%s\r" $frame
				sleep 1
    done
    heads_tails=$RANDOM
    if [[ $((heads_tails % 2)) -eq 0 ]]
    then
				echo "It was heads the cpu goes first!"
    else
				echo "It was tails it's your turn!"
    fi
    sleep 1
}

function choose_symbols {

		echo "Your symbol may be any upper or lowercase character in the alphabet."
    read -n1 -p "Choose your player's symbol: " usr_sym
    printf "\n"
		local sym_regex='^[A-Za-z]$'
    local int='^[0-9]+$'
    if ! [[ $usr_sym =~ $sym_regex ]]
    then
				echo "I'm sorry that symbol is not allowed. Please choose again."
				choose_symbols
    else
				if [[ $usr_sym == "X" ]] ; then cpu_sym="O" ; else cpu_sym="X" ; fi
    fi
}

function reset_game_variables {

    gameboard=("" "" "" "" "" "" "" "" "") #9 empty strs
    gameover=false
    turn_ctr=0
}

function play_TTT {

		reset_game_variables
		choose_symbols
		flip_coin
		# Give player a second to see who won the coin toss
		sleep 2
		clear

		msg=$1
		tput cup $(( $board_top_left_row - 1)) $(( ($COLUMNS / 2) - (${#msg} / 2) ))
		echo "$msg"
		draw_board
		draw_grid
		msg="Select using arrow keys and enter to confirm."
		tput cup $(( $board_top_left_row + $board_height + 2 )) $(( ($COLUMNS / 2) - (${#msg} / 2) ))
		echo "$msg"
		
		take_turns
}

#################################################################
######################### MAIN FUNCTION #########################
#################################################################

function main {

		trap "tput setab 0; tput setaf 7; tput clear" SIGINT
		# If the user changes the screen size in the middle
		# of the game we need to redraw the board and the grid!
		trap "draw_board; draw_grid" WINCH

		# COLOR SCHEME FOR THE GAME:
		#  - MENU SHOWING USER OPTIONS
		#  - BOARD SHOWING GRID PLAY SPACE
		BG_WHITE="$(tput setab 7)"
		FG_BLUE="$(tput setaf 4)"

		tput smcup

		while [[ $REPLY != 0 ]]; do
				echo -n ${BG_WHITE}${FG_BLUE}
				clear
				print_title "YOU ARE PLAYING TIC-TAC-TOE!!!"
				# reset cursor for options
				tput cup $(( $LINES / 3)) 0
				cat <<- _end_menu_
      Please Select:

      1. 1 Player (P1:$num_p1_wins, CPU:$num_cpu_wins)
      2. 2 Player (P1:$num_p1_wins,  P2:$num_p2_wins)
      3. Quit

			_end_menu_

				read -p "      Enter selection: > " selection

				# Reset screen to normal white-on-black scheme
				tput rmcup
				tput sgr0
				clear

				# echo
				msg=""

				case $selection in
						1) play_TTT "It's you vs. the CPU!"
							 ;;
						2) play_TTT "It's you vs. Player2!"
							 ;;
						3) break
							 ;;
						*) msg="Invalid Entry"
							 echo "$msg"
							 ;;
				esac
				#				printf "\n\nPress any key to continue."
				#				read -n 1
		done

}

main
tput rmcup
echo "Program Terminated."
