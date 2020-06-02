L_FITS_DIR=light_fits
D_FITS_DIR=dark_fits
F_FITS_DIR=flat_fits
DF_FITS_DIR=dark_flat_fits
MASTER_DIR=master
LMD_FITS_DIR=light_minus_dark_fits

rm -rf $MASTER_DIR
mkdir $MASTER_DIR

rm -rf $LMD_FITS_DIR
mkdir $LMD_FITS_DIR

echo "dark stacking"
./at_stack $D_FITS_DIR/dark_fits.list med3 $MASTER_DIR/master_dark.fits
echo "flat stacking"
./at_stack $F_FITS_DIR/flat_fits.list med3 $MASTER_DIR/master_flat.fits
echo "dark flat stacking"
./at_stack $DF_FITS_DIR/dark_flat_fits.list med3 $MASTER_DIR/master_dark_flat.fits

echo "light - master dark subtraction"
./subtract.sh $L_FITS_DIR $MASTER_DIR/master_dark.fits $LMD_FITS_DIR

echo "master flat - master dark flat subtraction"
./at_calc - $MASTER_DIR/master_flat.fits $MASTER_DIR/master_dark_flat.fits $MASTER_DIR/master_flat_minus_dark_flat.fits
echo "flat - median filter"
./at_median_filter $MASTER_DIR/master_flat_minus_dark_flat.fits $MASTER_DIR/master_flat_minus_dark_flat_post_median.fits 1
