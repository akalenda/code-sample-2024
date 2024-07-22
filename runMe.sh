#!/bin/bash

# You can run with defaults, 
#   e.g. sh ./runMe.sh
# or with custom inputs,
#   e.g. sh ./runMe.sh "hoh lot dog cog hog" hoh cog
#
# You can also try running just get-shortest-path.js on different paths
# after everything is constructed

INPUT=${1:-"hi hit hot hoh oh lot dot dog log cog"}
SRC=${2:-"dot"}
DST=${3:-"hi"}

if [ ! -f out/construct-min-span-trees.elf ]
then
  mkdir out
  g++ -o out/construct-min-span-trees.elf src/construct-min-span-trees.cpp
fi

echo "$INPUT" \
  | tee out/_00-input.txt \
  | python3 src/construct-adjacency-list.py \
  | tee out/_10-adjacency-list.json \
  | ./out/construct-min-span-trees.elf \
  | tee out/_20-min-span-trees.json \
  | nodejs src/get-shortest-path "${SRC}" "${DST}" \
  | tee out/_30-path.json
