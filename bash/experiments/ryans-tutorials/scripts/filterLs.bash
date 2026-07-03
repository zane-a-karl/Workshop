#!/bin/bash

cat /dev/stdin | tail -n +2 | awk -F " " '{printf "%-25s %-5s %10s\n", $9, $5, $3}'
