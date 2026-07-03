#!/bin/bash

while true; do
read -rsn1 -p "Please input a character: " input
if [ "$input" = "a" ]; then
    echo "hello world"
else
		echo "Not that one!"
fi
done
