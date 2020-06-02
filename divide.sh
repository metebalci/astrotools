#!/bin/bash

if [ $# -lt 3 ]
then
  echo "usage: ./divide.sh input_dir divisor_file output_dir"
else
  find $1 -name '*.fits' -type f | while read fname; do
    INPUTFILE=$fname
    DIVISORFILE=$2
    OUTPUTFILE=$3/$(basename "$fname")
    echo $INPUTFILE " / " $DIVISORFILE " = " $OUTPUTFILE
    ./at_calc / $INPUTFILE $DIVISORFILE $OUTPUTFILE
  done
fi
