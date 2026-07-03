#!/bin/bash

#  This is a basic implementation of the game Tic-Tac-Toe
#  with little in the way of visual appeal.

board=(0 1 2 3 4 5 6 7 8)
gameover=false
turn_ctr=0

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

    clear
    echo "------------------------------------------------"
    for ((i=0; i<${#board[@]}; i++))
    do
				if [[ $((i % 3)) -eq 2 ]]
				then
						printf "\t%s\n" ${board[$i]}
						if [[ i != 8 ]]; then echo "------------------------------------------------"; fi
				else
						printf "\t%s\t|" ${board[$i]}
				fi
    done

    check_win $usr_sym
    if [[ $? -eq 0 ]]
    then
				echo "Congrats you won!"
				echo "It took $turn_ctr moves"
				gameover=true
    fi
    check_win $cpu_sym
    if [[ $? -eq 0 ]]
    then
				echo "Sorry, you lost"
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

    echo "Enter your next move choice"
    read -sn1 choice
    validate_move $choice $usr_sym
    local move_is_valid=$?
    if [[ $move_is_valid -ne 0 ]]
    then
				echo "Sorry that position is invalid, please try again."
				make_usr_move
    fi
    board[$choice]=$usr_sym
    update_game
}

function make_cpu_move {

    choice=$((RANDOM % 9))
    validate_move $choice $cpu_sym
    local move_is_valid=$?
    if [[ $move_is_valid -ne 0 ]]
    then
				make_cpu_move
    fi
    echo "Now it's the CPU's turn"
    sleep 1
    board[$choice]=$cpu_sym
    update_game
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
    echo "We're flipping a coin to see who goes first!"
		printf "%s\r" $frame
		sleep 1
    for frame in ${flip_animation[@]}
    do
				printf "%s\r" $frame
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

function reset_board {

    board=(0 1 2 3 4 5 6 7 8)
    gameover=false
    turn_ctr=0
}

function play_game {

    echo "Would you like to begin a new game?(y/n)"
    read -sn1 new_game
    while [[ $new_game = "y" ]];
    do
				reset_board
				choose_symbols
				flip_coin
				update_game
				take_turns
				echo "Would you like to play again?(y/n)"
				read -sn1 new_game
    done
    echo "Good bye"
}

######################### MAIN FUNCTION #########################

echo "WELCOME TO TIC-TAC-TOE"
play_game
echo "Program Terminated"
