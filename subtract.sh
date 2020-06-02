#!/bin/bash

if [ $# -lt 3 ]
then
  echo "usage: ./subtract.sh input_dir subtract_file output_dir"
else
  find $1 -name '*.fits' -type f | while read fname; do
    INPUTFILE=$fname
    OUTPUTFILE=$3/$(basename "$fname")
    echo $INPUTFILE " - " $2 " = " $OUTPUTFILE
    ./at_calc - $INPUTFILE $2 $OUTPUTFILE
  done
fi
