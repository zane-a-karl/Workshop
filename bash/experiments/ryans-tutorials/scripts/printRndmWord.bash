#!/bin/bash

wordsDir=/usr/share/dict/words

#keep only the words with length $1
wordsWithN=$( cat $wordsDir | egrep "^.{$1}$" )
numWords=$( echo "${wordsWithN}" | wc -l )

rnum=$(( $RANDOM % $numWords ))

cat $wordsDir | egrep "^.{$1}$" | head -n $rnum | tail -1
