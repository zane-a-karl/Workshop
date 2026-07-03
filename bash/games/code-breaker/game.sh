#!/bin/bash

#  This is an implementation of the game Code Breaker
# it's similar to the game Mastermind but with a few
# caveats:
#   - We will have 5 code slots but 8 options per slot
#   - We will keep score
#     - 1 pt for each second it takes to solve
#     - 30 pts for each full attempt up to 8
#   - We will have a 2 player mode and a cpu mode
#     - in two player P2 will be prompted to enter a code
#     and P1 will look away and then P2 will leave as P1
#     solves the code
#     - in cpu mode the cpu will randomly generate a code
#     and P1 will work to solve it

# e.g. code = 1 2 3 4 5
# attempt 1:  8 7 6 5 4 | ~ ~ _ _ _
# attempt 2:  8 7 1 2 3 | ~ ~ ~ _ _
# attempt 3:  5 4 3 2 1 | * ~ ~ ~ ~
# attempt 4:  5 2 3 4 1 | * * * ~ ~
# attempt 5:  1 2 3 4 5 | * * * * *

# * => one of your choices is in the code and in the right position
# ~ => one of your choices is in the code
# _ => one of your choices is not in the code
# the position of the *, ~, or _ tells you nothing about the correct
# position of the number
# I will assume I will print *s first then ~s and then _s
