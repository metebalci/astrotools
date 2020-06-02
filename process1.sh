L_DIR=light
D_DIR=dark
F_DIR=flat
DF_DIR=dark_flat

L_FITS_DIR=light_fits
D_FITS_DIR=dark_fits
F_FITS_DIR=flat_fits
DF_FITS_DIR=dark_flat_fits

rm -rf $L_FITS_DIR
rm -rf $D_FITS_DIR
rm -rf $F_FITS_DIR
rm -rf $DF_FITS_DIR

mkdir $L_FITS_DIR
mkdir $D_FITS_DIR
mkdir $F_FITS_DIR
mkdir $DF_FITS_DIR

echo "light - raw2fits conversion"
./raw2fits.sh $L_DIR $L_FITS_DIR
echo "dark - raw2fits conversion"
./raw2fits.sh $D_DIR $D_FITS_DIR
echo "flat - raw2fits conversion"
./raw2fits.sh $F_DIR $F_FITS_DIR
echo "dark flat - raw2fits conversion"
./raw2fits.sh $DF_DIR $DF_FITS_DIR

echo "file list generation"
find $D_FITS_DIR -name '*.fits' -type f > $D_FITS_DIR/dark_fits.list
find $F_FITS_DIR -name '*.fits' -type f > $F_FITS_DIR/flat_fits.list
find $DF_FITS_DIR -name '*.fits' -type f > $DF_FITS_DIR/dark_flat_fits.list
