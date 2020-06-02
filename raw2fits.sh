#!/bin/bash

if [ $# -lt 2 ]
then
  echo "usage: ./raw2fits.sh input_dir output_dir"
else
  find $1 -name '*.RAF' -type f | while read fname; do
    RAWFILE=$fname
    FITSFILE=$2/$(basename "$fname" .RAF).fits
    echo $RAWFILE " -> " $FITSFILE
    ./at_raw2fits $RAWFILE $FITSFILE
  done
fi
