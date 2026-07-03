#!/bin/bash

# first arg is a filename

# %<conversions> come from 'man 3 strftime'
today=$( date "+%Y-%m-%d" )

# make prefix date copy
#echo "$@" | sed "s; ;\n;g" | xargs -I ^ cp ^ "${today}_^"

# make suffix date copy

# one line solution
echo "$@" | sed "s; ;\n;g" | xargs -I {} bash -cs 'var="{}"; td=$( date "+%Y-%m-%d" ); cp "${var}" "${var%%.*}_${td}.${var##*.}"'


# stack exchange solution
 
# qXargs() {
#   printf '%s\n' "$1" |
#   sed \
#     -e "s:[\\'${IFS%?}\"]:\\\\&:g" \
#     -e '$!s:$:\\:'  \
#   ;
# }

# # loop over command-line arguments
# # quote them to make xargs safe and
# # break apart arg into head portion and
# #'extension and slip in today's date
# for arg
# do
#    src=$(qXargs "$arg")
#   head=$(qXargs "${arg%.*}")
#   tail=$(qXargs "${arg##*.}")
#   printf '%s\n%s_%s.%s\n'  \
#     "$src" \
#     "$head" "$today" "$tail" ;
# done | xargs -n 2 -t mv


