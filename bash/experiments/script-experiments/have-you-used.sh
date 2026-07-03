#!/bin/bash

#  The purpose of this script is to check if I have used
# the queried word (passed in as an argument to this
# script) in any of my other instances of my writing
# practice. If so I ought to choose a different word.

echo "Have you used the word \"$1\" before?"
echo "Let's check..."
echo "..."
echo "..."
alreadyUsed=$( ls | grep -E ".*\.txt$" | xargs -I{} cat {} | grep -cE ".*\\$1.*" )
echo $alreadyUsed
if [ $alreadyUsed -gt 0 ]
then
    echo "$1 has already been used!!!"
else
    echo "You're free to use $1!"
fi
