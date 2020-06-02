
[![Build Status](https://travis-ci.com/metebalci/astrotools.svg?branch=master)](https://travis-ci.com/metebalci/astrotools)

# Description

These are a few custom programs that I use for astrophotography. 

- All tools use OpenMP for parallel processing, using all processing cores in the system. 
- The tools are not memory optimized, for example, at_stack loads all files into memory, so if you have 100x 24 Mpixel images, that means you need `100 x 8B (double) x 24M (number of pixels) = 18 GB` of memory. 
- Especially the boundary checking of image dimensions are very missing in the code. You should pay attention to provide images with same dimensions.
- fits files are saved with cfitsio GZIP_2 compression internally by default. Set AT_NO_FITS_COMPRESSION environment variable to anything other than 0 to disable compression but be aware files will be huge (~200MB for 24MP images), but load/save performance might be better if you have a fast storage e.g. NVMe SSD.

Append `\!` to fits output file arguments if you want to overwrite it. This is the behavior of cfitsio library which I use for loading and saving fits files.

- at_raw2fits: converts a RAW file (from a DSLR etc.) to a very simple FITS file consisting of only primary HDU with a two dimensional array of 64-bit floating point reals (double). The size of the array is the visible area in the RAW file, and the array elements are the same as the values in the RAW file stored as double. For batch processing, use a bash script like `raw2fits.sh` in the repo.

- at_stack: stacks a set of FITS files, whose paths are provided in a text file, using a stacking method, and saves the result into a FITS file. The file list can be generated with `find` command, e.g. `find . -name '*.fits' > file_list`. 

- at_stat: do a statistical calculation on a fits file.

- at_calc: do an arithmetic calculation on one or two fits file and save the result to another fits file. For batch processing, use a bash script like `subtract.sh` and `divide.sh` in the repo.

- at_median_filter: median filter a fits and write output to another fits. Filter size can be specified. The filter size argument is number of pixels from center to edge (i.e. radius), so for 3x3 filter, filter size = 1 should be given.

- at_superpixel: converts a FITS file to another by combining pixels (3x3 for xtrans), making super pixels. For batch processing, use a bash script like `superpixel.sh` in the repo.

# Status

I use the tools on Linux (Ubuntu 18.04) and with Fuji X-Trans (Fuji X-T2 and Fuji X-H1) cameras, so they are tested only in these environments.

# Build

Just clone and run `make`.

# Dependencies

- [libraw](https://www.libraw.org/)
- [cfitsio](https://heasarc.gsfc.nasa.gov/fitsio/)
