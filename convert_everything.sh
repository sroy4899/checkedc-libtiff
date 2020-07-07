CCONV=/Users/shilpa-roy/checkedc-clang/llvm/cmake-build-debug/bin/cconv-standalone
INCLUDES=/Users/shilpa-roy/checkedc/include/
BASEDIR=.

CPATH=$INCLUDES $CCONV \
-output-postfix=checked \
-base-dir="$BASEDIR" \
$BASEDIR/libtiff/tif_ojpeg.c \
$BASEDIR/libtiff/tif_dirwrite.c \
$BASEDIR/libtiff/tif_tile.c \
$BASEDIR/libtiff/tif_jbig.c \
$BASEDIR/libtiff/tif_packbits.c \
$BASEDIR/libtiff/mkg3states.c \
$BASEDIR/libtiff/tif_warning.c \
$BASEDIR/libtiff/tif_webp.c \
$BASEDIR/libtiff/tif_lzma.c \
$BASEDIR/libtiff/tif_thunder.c \
$BASEDIR/libtiff/tif_swab.c \
$BASEDIR/libtiff/tif_compress.c \
$BASEDIR/libtiff/tif_codec.c \
$BASEDIR/libtiff/tif_luv.c \
$BASEDIR/libtiff/tif_open.c \
$BASEDIR/libtiff/tif_dumpmode.c \
$BASEDIR/libtiff/tif_strip.c \
$BASEDIR/libtiff/tif_print.c \
$BASEDIR/libtiff/tif_flush.c \
$BASEDIR/libtiff/tif_dirinfo.c \
$BASEDIR/libtiff/tif_fax3.c \
$BASEDIR/libtiff/tif_pixarlog.c \
$BASEDIR/libtiff/tif_getimage.c \
$BASEDIR/libtiff/tif_dir.c \
$BASEDIR/libtiff/tif_write.c \
$BASEDIR/libtiff/tif_win32.c \
$BASEDIR/libtiff/tif_jpeg_12.c \
$BASEDIR/libtiff/tif_fax3sm.c \
$BASEDIR/libtiff/tif_predict.c \
$BASEDIR/libtiff/tif_aux.c \
$BASEDIR/libtiff/tif_close.c \
$BASEDIR/libtiff/tif_extension.c \
$BASEDIR/libtiff/tif_color.c \
$BASEDIR/libtiff/tif_jpeg.c \
$BASEDIR/libtiff/tif_dirread.c \
$BASEDIR/libtiff/tif_version.c \
$BASEDIR/libtiff/tif_unix.c \
$BASEDIR/libtiff/tif_zip.c \
$BASEDIR/libtiff/tif_read.c \
$BASEDIR/libtiff/tif_error.c \
$BASEDIR/libtiff/tif_zstd.c \
$BASEDIR/libtiff/tif_lzw.c \
$BASEDIR/libtiff/tif_next.c \
$BASEDIR/tools/pal2rgb.c \
$BASEDIR/tools/tiffmedian.c \
$BASEDIR/tools/tiffcp.c \
$BASEDIR/tools/rgb2ycbcr.c \
$BASEDIR/tools/tiffgt.c \
$BASEDIR/tools/raw2tiff.c \
$BASEDIR/tools/tiffdump.c \
$BASEDIR/tools/ppm2tiff.c \
$BASEDIR/tools/tiffset.c \
$BASEDIR/tools/tiffdither.c \
$BASEDIR/tools/tiff2ps.c \
$BASEDIR/tools/tiffcrop.c \
$BASEDIR/tools/tiff2rgba.c \
$BASEDIR/tools/fax2tiff.c \
$BASEDIR/tools/tiff2pdf.c \
$BASEDIR/tools/tiffsplit.c \
$BASEDIR/tools/tiff2bw.c \
$BASEDIR/tools/thumbnail.c \
$BASEDIR/tools/tiffcmp.c \
$BASEDIR/tools/fax2ps.c \
$BASEDIR/tools/tiffinfo.c \
$BASEDIR/test/custom_dir.c \
$BASEDIR/test/strip.c \
$BASEDIR/test/defer_strile_loading.c \
$BASEDIR/test/testtypes.c \
$BASEDIR/test/defer_strile_writing.c \
$BASEDIR/test/check_tag.c \
$BASEDIR/test/short_tag.c \
$BASEDIR/test/test_arrays.c \
$BASEDIR/test/raw_decode.c \
$BASEDIR/test/rewrite_tag.c \
$BASEDIR/test/ascii_tag.c \
$BASEDIR/test/long_tag.c \
$BASEDIR/test/strip_rw.c \
$BASEDIR/contrib/dbs/tiff-rgb.c \
$BASEDIR/contrib/dbs/tiff-palette.c \
$BASEDIR/contrib/dbs/tiff-grayscale.c \
$BASEDIR/contrib/dbs/xtiff/xtiff.c \
$BASEDIR/contrib/dbs/tiff-bi.c \
$BASEDIR/contrib/ras/ras2tif.c \
$BASEDIR/contrib/ras/tif2ras.c \
$BASEDIR/contrib/pds/tif_imageiter.c \
$BASEDIR/contrib/pds/tif_pdsdirwrite.c \
$BASEDIR/contrib/pds/tif_pdsdirread.c \
$BASEDIR/contrib/mfs/mfs_file.c \
$BASEDIR/contrib/tags/maketif.c \
$BASEDIR/contrib/tags/xtif_dir.c \
$BASEDIR/contrib/tags/listtif.c \
$BASEDIR/contrib/iptcutil/iptcutil.c \
$BASEDIR/contrib/addtiffo/tif_ovrcache.c \
$BASEDIR/contrib/addtiffo/addtiffo.c \
$BASEDIR/contrib/addtiffo/tif_overview.c \
$BASEDIR/contrib/win_dib/tiff2dib.c \
$BASEDIR/port/strcasecmp.c \
$BASEDIR/port/strtol.c \
$BASEDIR/port/dummy.c \
$BASEDIR/port/snprintf.c \
$BASEDIR/port/strtoull.c \
$BASEDIR/port/strtoll.c \
$BASEDIR/port/strtoul.c \
$BASEDIR/port/lfind.c \
$BASEDIR/port/getopt.c
