#!/bin/sh
x=0
while [ $x -lt 3 ]; do
  echo "loop $x"
  
done
if [ $x -eq 3 ]; then
  echo "finished"
fi
