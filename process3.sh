L_FITS_DIR=light_fits
D_FITS_DIR=dark_fits
F_FITS_DIR=flat_fits
DF_FITS_DIR=dark_flat_fits
MASTER_DIR=master
LMD_FITS_DIR=light_minus_dark_fits
C_FITS_DIR=calibrated
LSP_FITS_DIR=light_superpixel

rm -rf $C_FITS_DIR
mkdir $C_FITS_DIR

rm -rf $LSP_FITS_DIR
mkdir $LSP_FITS_DIR

echo "light - normalization with master flat"
./divide.sh $LMD_FITS_DIR $MASTER_DIR/master_flat_minus_dark_flat_post_median.fits $C_FITS_DIR

echo "light - super pixel"
./superpixel.sh $C_FITS_DIR $LSP_FITS_DIR
