#!/bin/bash

if [ $# -lt 2 ]
then
  echo "usage: ./superpixel.sh input_dir output_dir"
else
  find $1 -name '*.fits' -type f | while read fname; do
    INPUTFILE=$fname
    OUTPUTFILE=$2/$(basename "$fname")
    echo $INPUTFILE " >>> " $OUTPUTFILE
    ./at_superpixel $INPUTFILE $OUTPUTFILE
  done
fi
