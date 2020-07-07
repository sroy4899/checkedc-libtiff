CCONV=/Users/shilpa-roy/checkedc-clang/llvm/cmake-build-debug/bin/cconv-standalone
INCLUDES=/Users/shilpa-roy/checkedc/include/
BASEDIR=./libtiff

CPATH=$INCLUDES $CCONV \
-output-postfix=checked \
-base-dir="$BASEDIR" \
$BASEDIR/tif_ojpeg.c \
$BASEDIR/tif_dirwrite.c \
$BASEDIR/tif_tile.c \
$BASEDIR/tif_jbig.c \
$BASEDIR/tif_packbits.c \
$BASEDIR/mkg3states.c \
$BASEDIR/tif_warning.c \
$BASEDIR/tif_webp.c \
$BASEDIR/tif_lzma.c \
$BASEDIR/tif_thunder.c \
$BASEDIR/tif_swab.c \
$BASEDIR/tif_compress.c \
$BASEDIR/tif_codec.c \
$BASEDIR/tif_luv.c \
$BASEDIR/tif_open.c \
$BASEDIR/tif_dumpmode.c \
$BASEDIR/tif_strip.c \
$BASEDIR/tif_print.c \
$BASEDIR/tif_flush.c \
$BASEDIR/tif_dirinfo.c \
$BASEDIR/tif_fax3.c \
$BASEDIR/tif_pixarlog.c \
$BASEDIR/tif_getimage.c \
$BASEDIR/tif_dir.c \
$BASEDIR/tif_write.c \
$BASEDIR/tif_win32.c \
$BASEDIR/tif_jpeg_12.c \
$BASEDIR/tif_fax3sm.c \
$BASEDIR/tif_predict.c \
$BASEDIR/tif_aux.c \
$BASEDIR/tif_close.c \
$BASEDIR/tif_extension.c \
$BASEDIR/tif_color.c \
$BASEDIR/tif_jpeg.c \
$BASEDIR/tif_dirread.c \
$BASEDIR/tif_version.c \
$BASEDIR/tif_unix.c \
$BASEDIR/tif_zip.c \
$BASEDIR/tif_read.c \
$BASEDIR/tif_error.c \
$BASEDIR/tif_zstd.c \
$BASEDIR/tif_lzw.c \
$BASEDIR/tif_next.c

