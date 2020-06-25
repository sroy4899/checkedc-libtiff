CCONV=/Users/shilpa-roy/checkedc-clang/llvm/cmake-build-debug/bin/cconv-standalone
INCLUDES=/Users/shilpa-roy/checkedc/include
BASEDIR=.

CPATH=$INCLUDES $CCONV \
-dump-stats \
-output-postfix=checked \
-dump-intermediate \
-extra-arg-before=-DLUA_COMPAT_5_2 \
-extra-arg-before=-DLUA_USE_LINUX \
-extra-arg-before=-w \
-base-dir="$BASEDIR" \
$BASEDIR/mkg3states.c \
$BASEDIR/tif_zstd.c