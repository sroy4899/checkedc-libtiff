/*
 * Copyright (c) 1988-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#ifndef _TIFFIO_
#define	_TIFFIO_

/*
 * TIFF I/O Library Definitions.
 */
#include "tiff.h"
#include "tiffvers.h"

/*
 * TIFF is defined as an incomplete type to hide the
 * library's internal data structures from clients.
 */
typedef struct tiff TIFF;

/*
 * The following typedefs define the intrinsic size of
 * data types used in the *exported* interfaces.  These
 * definitions depend on the proper definition of types
 * in tiff.h.  Note also that the varargs interface used
 * to pass tag types and values uses the types defined in
 * tiff.h directly.
 *
 * NB: ttag_t is unsigned int and not unsigned short because
 *     ANSI C requires that the type before the ellipsis be a
 *     promoted type (i.e. one of int, unsigned int, pointer,
 *     or double) and because we defined pseudo-tags that are
 *     outside the range of legal Aldus-assigned tags.
 * NB: tsize_t is signed and not unsigned because some functions
 *     return -1.
 * NB: toff_t is not off_t for many reasons; TIFFs max out at
 *     32-bit file offsets, and BigTIFF maxes out at 64-bit
 *     offsets being the most important, and to ensure use of
 *     a consistently unsigned type across architectures.
 *     Prior to libtiff 4.0, this was an unsigned 32 bit type.
 */
/*
 * this is the machine addressing size type, only it's signed, so make it
 * int32 on 32bit machines, int64 on 64bit machines
 */
typedef TIFF_SSIZE_T tmsize_t;
typedef uint64 toff_t;          /* file offset */
/* the following are deprecated and should be replaced by their defining
   counterparts */
typedef uint32 ttag_t;          /* directory tag */
typedef uint16 tdir_t;          /* directory index */
typedef uint16 tsample_t;       /* sample number */
typedef uint32 tstrile_t;       /* strip or tile number */
typedef tstrile_t tstrip_t;     /* strip number */
typedef tstrile_t ttile_t;      /* tile number */
typedef tmsize_t tsize_t;       /* i/o size in bytes */
typedef void* tdata_t;          /* image data ref */

#if !defined(__WIN32__) && (defined(_WIN32) || defined(WIN32))
#define __WIN32__
#endif

/*
 * On windows you should define USE_WIN32_FILEIO if you are using tif_win32.c
 * or AVOID_WIN32_FILEIO if you are using something else (like tif_unix.c).
 *
 * By default tif_unix.c is assumed.
 */

#if defined(_WINDOWS) || defined(__WIN32__) || defined(_Windows)
#  if !defined(__CYGWIN) && !defined(AVOID_WIN32_FILEIO) && !defined(USE_WIN32_FILEIO)
#    define AVOID_WIN32_FILEIO
#  endif
#endif

#if defined(USE_WIN32_FILEIO)
# define VC_EXTRALEAN
# include <windows.h>
# ifdef __WIN32__
DECLARE_HANDLE(thandle_t);     /* Win32 file handle */
# else
typedef HFILE thandle_t;       /* client data handle */
# endif /* __WIN32__ */
#else
typedef void* thandle_t;       /* client data handle */
#endif /* USE_WIN32_FILEIO */

/*
 * Flags to pass to TIFFPrintDirectory to control
 * printing of data structures that are potentially
 * very large.   Bit-or these flags to enable printing
 * multiple items.
 */
#define TIFFPRINT_NONE	       0x0    /* no extra info */
#define TIFFPRINT_STRIPS       0x1    /* strips/tiles info */
#define TIFFPRINT_CURVES       0x2    /* color/gray response curves */
#define TIFFPRINT_COLORMAP     0x4    /* colormap */
#define TIFFPRINT_JPEGQTABLES  0x100  /* JPEG Q matrices */
#define TIFFPRINT_JPEGACTABLES 0x200  /* JPEG AC tables */
#define TIFFPRINT_JPEGDCTABLES 0x200  /* JPEG DC tables */

/* 
 * Colour conversion stuff
 */

/* reference white */
#define D65_X0 (95.0470F)
#define D65_Y0 (100.0F)
#define D65_Z0 (108.8827F)

#define D50_X0 (96.4250F)
#define D50_Y0 (100.0F)
#define D50_Z0 (82.4680F)

/* Structure for holding information about a display device. */

typedef unsigned char TIFFRGBValue;               /* 8-bit samples */

typedef struct {
	float d_mat _Checked[3][3];                        /* XYZ -> luminance matrix */
	float d_YCR;                              /* Light o/p for reference white */
	float d_YCG;
	float d_YCB;
	uint32 d_Vrwr;                            /* Pixel values for ref. white */
	uint32 d_Vrwg;
	uint32 d_Vrwb;
	float d_Y0R;                              /* Residual light for black pixel */
	float d_Y0G;
	float d_Y0B;
	float d_gammaR;                           /* Gamma values for the three guns */
	float d_gammaG;
	float d_gammaB;
} TIFFDisplay;

typedef struct {                                  /* YCbCr->RGB support */
	TIFFRGBValue* clamptab;                   /* range clamping table */
	int* Cr_r_tab;
	int* Cb_b_tab;
	int32* Cr_g_tab;
	int32* Cb_g_tab;
	int32* Y_tab;
} TIFFYCbCrToRGB;

typedef struct {                                  /* CIE Lab 1976->RGB support */
	int range;                                /* Size of conversion table */
#define CIELABTORGB_TABLE_RANGE 1500
	float rstep, gstep, bstep;
	float X0, Y0, Z0;                         /* Reference white point */
	TIFFDisplay display;
	float Yr2r _Checked[1501];  /* Conversion of Yr to r */
	float Yg2g _Checked[1501];  /* Conversion of Yg to g */
	float Yb2b _Checked[1501];  /* Conversion of Yb to b */
} TIFFCIELabToRGB;

/*
 * RGBA-style image support.
 */
typedef struct _TIFFRGBAImage TIFFRGBAImage;
/*
 * The image reading and conversion routines invoke
 * ``put routines'' to copy/image/whatever tiles of
 * raw image data.  A default set of routines are 
 * provided to convert/copy raw image data to 8-bit
 * packed ABGR format rasters.  Applications can supply
 * alternate routines that unpack the data into a
 * different format or, for example, unpack the data
 * and draw the unpacked raster on the display.
 */
typedef void (*tileContigRoutine)
    (TIFFRGBAImage*, uint32*, uint32, uint32, uint32, uint32, int32, int32,
	unsigned char*);
typedef void (*tileSeparateRoutine)
    (TIFFRGBAImage*, uint32*, uint32, uint32, uint32, uint32, int32, int32,
	unsigned char*, unsigned char*, unsigned char*, unsigned char*);
/*
 * RGBA-reader state.
 */
struct _TIFFRGBAImage {
	TIFF* tif;                              /* image handle */
	int stoponerr;                          /* stop on read error */
	int isContig;                           /* data is packed/separate */
	int alpha;                              /* type of alpha data present */
	uint32 width;                           /* image width */
	uint32 height;                          /* image height */
	uint16 bitspersample;                   /* image bits/sample */
	uint16 samplesperpixel;                 /* image samples/pixel */
	uint16 orientation;                     /* image orientation */
	uint16 req_orientation;                 /* requested orientation */
	uint16 photometric;                     /* image photometric interp */
	uint16* redcmap;                        /* colormap palette */
	uint16* greencmap;
	uint16* bluecmap;
	/* get image data routine */
	_Ptr<int (TIFFRGBAImage *, uint32 *, uint32 , uint32 )> get;
	/* put decoded strip/tile */
	union {
	    void (*any)(TIFFRGBAImage*);
	    tileContigRoutine contig;
	    tileSeparateRoutine separate;
	} put;
	TIFFRGBValue* Map;                      /* sample mapping array */
	uint32** BWmap;                         /* black&white map */
	uint32** PALmap;                        /* palette image map */
	TIFFYCbCrToRGB* ycbcr;                  /* YCbCr conversion state */
	TIFFCIELabToRGB* cielab;                /* CIE L*a*b conversion state */

	uint8* UaToAa;                          /* Unassociated alpha to associated alpha conversion LUT */
	uint8* Bitdepth16To8;                   /* LUT for conversion from 16bit to 8bit values */

	int row_offset;
	int col_offset;
};

/*
 * Macros for extracting components from the
 * packed ABGR form returned by TIFFReadRGBAImage.
 */
#define TIFFGetR(abgr) ((abgr) & 0xff)
#define TIFFGetG(abgr) (((abgr) >> 8) & 0xff)
#define TIFFGetB(abgr) (((abgr) >> 16) & 0xff)
#define TIFFGetA(abgr) (((abgr) >> 24) & 0xff)

/*
 * A CODEC is a software package that implements decoding,
 * encoding, or decoding+encoding of a compression algorithm.
 * The library provides a collection of builtin codecs.
 * More codecs may be registered through calls to the library
 * and/or the builtin implementations may be overridden.
 */
typedef int (*TIFFInitMethod)(TIFF*, int);
typedef struct {
	char* name;
	uint16 scheme;
	_Ptr<int (TIFF *, int )> init;
} TIFFCodec;

#include <stdio.h>
#include <stdarg.h>

/* share internal LogLuv conversion routines? */
#ifndef LOGLUV_PUBLIC
#define LOGLUV_PUBLIC 1
#endif

#if !defined(__GNUC__) && !defined(__attribute__)
#  define __attribute__(x) /*nothing*/
#endif

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif
typedef void (*TIFFErrorHandler)(const char*, const char*, va_list);
typedef void (*TIFFErrorHandlerExt)(thandle_t, const char*, const char*, va_list);
typedef tmsize_t (*TIFFReadWriteProc)(thandle_t, void*, tmsize_t);
typedef toff_t (*TIFFSeekProc)(thandle_t, toff_t, int);
typedef int (*TIFFCloseProc)(thandle_t);
typedef toff_t (*TIFFSizeProc)(thandle_t);
typedef int (*TIFFMapFileProc)(thandle_t, void** base, toff_t* size);
typedef void (*TIFFUnmapFileProc)(thandle_t, void* base, toff_t size);
typedef void (*TIFFExtendProc)(TIFF*);

extern const char* TIFFGetVersion(void);

extern const TIFFCodec * TIFFFindCODEC(uint16);
extern TIFFCodec * TIFFRegisterCODEC(uint16, const char *, _Ptr<int (TIFF *, int )> init);
extern void TIFFUnRegisterCODEC(_Ptr<TIFFCodec> c);
extern int TIFFIsCODECConfigured(uint16);
extern TIFFCodec* TIFFGetConfiguredCODECs(void);

/*
 * Auxiliary functions.
 */

extern void * _TIFFmalloc(tmsize_t s);
extern void * _TIFFcalloc(tmsize_t nmemb, tmsize_t siz);
extern void * _TIFFrealloc(void *p, tmsize_t s);
extern void _TIFFmemset(void *p, int v, tmsize_t c);
extern void _TIFFmemcpy(void *d, const void *s, tmsize_t c);
extern int _TIFFmemcmp(const void *p1, const void *p2, tmsize_t c);
extern void _TIFFfree(void *p);

/*
** Stuff, related to tag handling and creating custom tags.
*/
extern int TIFFGetTagListCount(TIFF *tif : itype(_Ptr<TIFF>));
extern uint32 TIFFGetTagListEntry(TIFF *tif : itype(_Ptr<TIFF>), int tag_index);
    
#define TIFF_ANY       TIFF_NOTYPE     /* for field descriptor searching */
#define TIFF_VARIABLE  -1              /* marker for variable length tags */
#define TIFF_SPP       -2              /* marker for SamplesPerPixel tags */
#define TIFF_VARIABLE2 -3              /* marker for uint32 var-length tags */

#define FIELD_CUSTOM    65

typedef struct _TIFFField TIFFField;
typedef struct _TIFFFieldArray TIFFFieldArray;

extern const TIFFField * TIFFFindField(TIFF *, uint32, TIFFDataType);
extern const TIFFField * TIFFFieldWithTag(TIFF *, uint32);
extern const TIFFField * TIFFFieldWithName(_Ptr<TIFF> tif, const char *);

extern uint32 TIFFFieldTag(const TIFFField *fip : itype(_Ptr<const TIFFField>));
extern const char * TIFFFieldName(_Ptr<const TIFFField> fip);
extern TIFFDataType TIFFFieldDataType(_Ptr<const TIFFField> fip);
extern int TIFFFieldPassCount(_Ptr<const TIFFField> fip);
extern int TIFFFieldReadCount(_Ptr<const TIFFField> fip);
extern int TIFFFieldWriteCount(_Ptr<const TIFFField> fip);

typedef int (*TIFFVSetMethod)(TIFF*, uint32, va_list);
typedef int (*TIFFVGetMethod)(TIFF*, uint32, va_list);
typedef void (*TIFFPrintMethod)(TIFF*, FILE*, long);

typedef struct {
    _Ptr<int (TIFF *, uint32 , struct __va_list_tag *)> vsetfield; /* tag set routine */
    _Ptr<int (TIFF *, uint32 , struct __va_list_tag *)> vgetfield; /* tag get routine */
    _Ptr<void (TIFF *, FILE *, long )> printdir; /* directory print routine */
} TIFFTagMethods;

extern _Ptr<TIFFTagMethods> TIFFAccessTagMethods(_Ptr<TIFF> tif);
extern void * TIFFGetClientInfo(_Ptr<TIFF> tif, const char *);
extern void TIFFSetClientInfo(_Ptr<TIFF> tif, void *, const char *);

extern void TIFFCleanup(TIFF *tif);
extern void TIFFClose(TIFF *tif);
extern int TIFFFlush(TIFF *tif);
extern int TIFFFlushData(TIFF *tif);
extern int TIFFGetField(TIFF *tif, uint32 tag, ...);
extern int TIFFVGetField(TIFF *tif, uint32 tag, va_list ap);
extern int TIFFGetFieldDefaulted(TIFF *tif, uint32 tag, ...);
extern int TIFFVGetFieldDefaulted(TIFF *tif, uint32 tag, va_list ap);
extern int TIFFReadDirectory(TIFF* tif);
extern int TIFFReadCustomDirectory(TIFF* tif, toff_t diroff, const TIFFFieldArray* infoarray);
extern int TIFFReadEXIFDirectory(TIFF* tif, toff_t diroff);
extern uint64 TIFFScanlineSize64(TIFF *tif);
extern tmsize_t TIFFScanlineSize(TIFF *tif);
extern uint64 TIFFRasterScanlineSize64(_Ptr<TIFF> tif);
extern tmsize_t TIFFRasterScanlineSize(_Ptr<TIFF> tif);
extern uint64 TIFFStripSize64(TIFF *tif);
extern tmsize_t TIFFStripSize(TIFF *tif);
extern uint64 TIFFRawStripSize64(TIFF *tif, uint32 strip);
extern tmsize_t TIFFRawStripSize(TIFF *tif, uint32 strip);
extern uint64 TIFFVStripSize64(TIFF *tif, uint32 nrows);
extern tmsize_t TIFFVStripSize(TIFF *tif, uint32 nrows);
extern uint64 TIFFTileRowSize64(TIFF *tif : itype(_Ptr<TIFF>));
extern tmsize_t TIFFTileRowSize(TIFF *tif);
extern uint64 TIFFTileSize64(TIFF *tif);
extern tmsize_t TIFFTileSize(TIFF *tif);
extern uint64 TIFFVTileSize64(TIFF *tif, uint32 nrows);
extern tmsize_t TIFFVTileSize(TIFF *tif, uint32 nrows);
extern uint32 TIFFDefaultStripSize(TIFF *tif, uint32 request);
extern void TIFFDefaultTileSize(_Ptr<TIFF> tif, _Ptr<uint32> tw, _Ptr<uint32> th);
extern int TIFFFileno(_Ptr<TIFF> tif);
extern int TIFFSetFileno(_Ptr<TIFF> tif, int);
extern thandle_t TIFFClientdata(_Ptr<TIFF> tif);
extern thandle_t TIFFSetClientdata(_Ptr<TIFF> tif, thandle_t);
extern int TIFFGetMode(_Ptr<TIFF> tif);
extern int TIFFSetMode(_Ptr<TIFF> tif, int);
extern int TIFFIsTiled(TIFF *tif : itype(_Ptr<TIFF>));
extern int TIFFIsByteSwapped(_Ptr<TIFF> tif);
extern int TIFFIsUpSampled(_Ptr<TIFF> tif);
extern int TIFFIsMSB2LSB(_Ptr<TIFF> tif);
extern int TIFFIsBigEndian(_Ptr<TIFF> tif);
extern _Ptr<tmsize_t (thandle_t , void *, tmsize_t )> TIFFGetReadProc(_Ptr<TIFF> tif);
extern _Ptr<tmsize_t (thandle_t , void *, tmsize_t )> TIFFGetWriteProc(_Ptr<TIFF> tif);
extern _Ptr<toff_t (thandle_t , toff_t , int )> TIFFGetSeekProc(_Ptr<TIFF> tif);                                                          
extern _Ptr<int (thandle_t )> TIFFGetCloseProc(_Ptr<TIFF> tif);
extern _Ptr<toff_t (thandle_t )> TIFFGetSizeProc(_Ptr<TIFF> tif);
extern _Ptr<int (thandle_t , void **, toff_t *)> TIFFGetMapFileProc(_Ptr<TIFF> tif);
extern _Ptr<void (thandle_t , void *, toff_t )> TIFFGetUnmapFileProc(_Ptr<TIFF> tif);
extern uint32 TIFFCurrentRow(_Ptr<TIFF> tif);
extern uint16 TIFFCurrentDirectory(_Ptr<TIFF> tif);
extern uint16 TIFFNumberOfDirectories(_Ptr<TIFF> tif);
extern uint64 TIFFCurrentDirOffset(_Ptr<TIFF> tif);
extern uint32 TIFFCurrentStrip(_Ptr<TIFF> tif);
extern uint32 TIFFCurrentTile(_Ptr<TIFF> tif);
extern int TIFFReadBufferSetup(TIFF* tif, void* bp, tmsize_t size);
extern int TIFFWriteBufferSetup(TIFF *tif, void *bp, tmsize_t size);  
extern int TIFFSetupStrips(TIFF *);
extern int TIFFWriteCheck(TIFF *, int, const char *);
extern void TIFFFreeDirectory(TIFF *tif : itype(_Ptr<TIFF>));
extern int TIFFCreateDirectory(TIFF *);
extern int TIFFCreateCustomDirectory(TIFF *, _Ptr<const TIFFFieldArray> infoarray);
extern int TIFFCreateEXIFDirectory(TIFF *);
extern int TIFFLastDirectory(_Ptr<TIFF> tif);
extern int TIFFSetDirectory(TIFF *, uint16);
extern int TIFFSetSubDirectory(TIFF *, uint64);
extern int TIFFUnlinkDirectory(TIFF *, uint16);
extern int TIFFSetField(TIFF *, uint32, ...);
extern int TIFFVSetField(TIFF *, uint32, va_list);
extern int TIFFUnsetField(TIFF *, uint32);
extern int TIFFWriteDirectory(TIFF *);
extern int TIFFWriteCustomDirectory(TIFF *, _Ptr<uint64> pdiroff);
extern int TIFFCheckpointDirectory(TIFF *);
extern int TIFFRewriteDirectory(TIFF *);
extern int TIFFDeferStrileArrayWriting(_Ptr<TIFF> tif);
extern int TIFFForceStrileArrayWriting(TIFF *);

#if defined(c_plusplus) || defined(__cplusplus)
extern void TIFFPrintDirectory(TIFF*, FILE*, long = 0);
extern int TIFFReadScanline(TIFF* tif, void* buf, uint32 row, uint16 sample = 0);
extern int TIFFWriteScanline(TIFF* tif, void* buf, uint32 row, uint16 sample = 0);
extern int TIFFReadRGBAImage(TIFF*, uint32, uint32, uint32*, int = 0);
extern int TIFFReadRGBAImageOriented(TIFF*, uint32, uint32, uint32*,
    int = ORIENTATION_BOTLEFT, int = 0);
#else
extern void TIFFPrintDirectory(TIFF *, FILE *, long);
extern int TIFFReadScanline(TIFF* tif, void* buf, uint32 row, uint16 sample);
extern int TIFFWriteScanline(TIFF *tif, void *buf, uint32 row, uint16 sample);
extern int TIFFReadRGBAImage(TIFF *, uint32, uint32, uint32 *, int);
extern int TIFFReadRGBAImageOriented(TIFF *, uint32, uint32, uint32 *, int, int);
#endif

extern int TIFFReadRGBAStrip(TIFF *, uint32, uint32 *);
extern int TIFFReadRGBATile(TIFF *, uint32, uint32, uint32 *);
extern int TIFFReadRGBAStripExt(TIFF *, uint32, uint32 *, int stop_on_error);
extern int TIFFReadRGBATileExt(TIFF *, uint32, uint32, uint32 *, int stop_on_error);
extern int TIFFRGBAImageOK(TIFF *, char [1024]);
extern int TIFFRGBAImageBegin(_Ptr<TIFFRGBAImage> img, TIFF *, int, char [1024]);
extern int TIFFRGBAImageGet(TIFFRGBAImage *, uint32 *, uint32, uint32);
extern void TIFFRGBAImageEnd(_Ptr<TIFFRGBAImage> img);
extern TIFF * TIFFOpen(const char *, _Array_ptr<const char> mode);
# ifdef __WIN32__
extern TIFF* TIFFOpenW(const wchar_t*, const char*);
# endif /* __WIN32__ */
extern TIFF * TIFFFdOpen(int, const char *, _Array_ptr<const char> mode);
extern TIFF * TIFFClientOpen(const char *, _Array_ptr<const char> mode, thandle_t, _Ptr<tmsize_t (thandle_t , void *, tmsize_t )> readproc, _Ptr<tmsize_t (thandle_t , void *, tmsize_t )> writeproc, _Ptr<toff_t (thandle_t , toff_t , int )> seekproc, _Ptr<int (thandle_t )> closeproc, _Ptr<toff_t (thandle_t )> sizeproc, _Ptr<int (thandle_t , void **, toff_t *)> mapproc, _Ptr<void (thandle_t , void *, toff_t )> unmapproc);
extern const char * TIFFFileName(TIFF *tif : itype(_Ptr<TIFF>));
extern const char * TIFFSetFileName(_Ptr<TIFF> tif, const char *);
extern void TIFFError(const char*, const char*, ...) __attribute__((__format__ (__printf__,2,3)));
extern void TIFFErrorExt(thandle_t, const char*, const char*, ...) __attribute__((__format__ (__printf__,3,4)));
extern void TIFFWarning(const char *, const char *, ...);
extern void TIFFWarningExt(thandle_t, const char *, const char *, ...);
extern TIFFErrorHandler TIFFSetErrorHandler(TIFFErrorHandler);
extern TIFFErrorHandlerExt TIFFSetErrorHandlerExt(TIFFErrorHandlerExt);
extern _Ptr<void (const char *, const char *, struct __va_list_tag *)> TIFFSetWarningHandler(_Ptr<void (const char *, const char *, struct __va_list_tag *)> handler);
extern TIFFErrorHandlerExt TIFFSetWarningHandlerExt(TIFFErrorHandlerExt);
extern TIFFExtendProc TIFFSetTagExtender(TIFFExtendProc);
extern uint32 TIFFComputeTile(TIFF *tif : itype(_Ptr<TIFF>), uint32 x, uint32 y, uint32 z, uint16 s);
extern int TIFFCheckTile(TIFF *tif : itype(_Ptr<TIFF>), uint32 x, uint32 y, uint32 z, uint16 s);
extern uint32 TIFFNumberOfTiles(TIFF *tif : itype(_Ptr<TIFF>));
extern tmsize_t TIFFReadTile(TIFF* tif, void* buf, uint32 x, uint32 y, uint32 z, uint16 s);  
extern tmsize_t TIFFWriteTile(TIFF *tif, void *buf, uint32 x, uint32 y, uint32 z, uint16 s);
extern uint32 TIFFComputeStrip(TIFF *tif : itype(_Ptr<TIFF>), uint32, uint16);
extern uint32 TIFFNumberOfStrips(TIFF *tif : itype(_Ptr<TIFF>));
extern tmsize_t TIFFReadEncodedStrip(TIFF* tif, uint32 strip, void* buf, tmsize_t size);
extern tmsize_t TIFFReadRawStrip(TIFF* tif, uint32 strip, void* buf, tmsize_t size);  
extern tmsize_t TIFFReadEncodedTile(TIFF* tif, uint32 tile, void* buf, tmsize_t size);  
extern tmsize_t TIFFReadRawTile(TIFF* tif, uint32 tile, void* buf, tmsize_t size);  
extern int      TIFFReadFromUserBuffer(TIFF* tif, uint32 strile,
                                       void* inbuf, tmsize_t insize,
                                       void* outbuf, tmsize_t outsize);
extern tmsize_t TIFFWriteEncodedStrip(TIFF *tif, uint32 strip, void *data, tmsize_t cc);
extern tmsize_t TIFFWriteRawStrip(TIFF *tif, uint32 strip, void *data, tmsize_t cc);  
extern tmsize_t TIFFWriteEncodedTile(TIFF *tif, uint32 tile, void *data, tmsize_t cc);  
extern tmsize_t TIFFWriteRawTile(TIFF *tif, uint32 tile, void *data, tmsize_t cc);  
extern int TIFFDataWidth(TIFFDataType);    /* table of tag datatype widths */
extern void TIFFSetWriteOffset(TIFF *tif : itype(_Ptr<TIFF>), toff_t off);
extern void TIFFSwabShort(uint16 *);
extern void TIFFSwabLong(uint32 *);
extern void TIFFSwabLong8(uint64 *);
extern void TIFFSwabFloat(float *);
extern void TIFFSwabDouble(double *);
extern void TIFFSwabArrayOfShort(uint16 *wp, tmsize_t n);
extern void TIFFSwabArrayOfTriples(_Array_ptr<uint8> tp : count(n), tmsize_t n);
extern void TIFFSwabArrayOfLong(uint32 *lp, tmsize_t n);
extern void TIFFSwabArrayOfLong8(uint64 *lp, tmsize_t n);
extern void TIFFSwabArrayOfFloat(float *fp, tmsize_t n);
extern void TIFFSwabArrayOfDouble(double *dp, tmsize_t n);
extern void TIFFReverseBits(uint8 *cp : itype(_Array_ptr<uint8>) count(n), tmsize_t n);
extern const unsigned char *TIFFGetBitRevTable(int) : itype(_Array_ptr<const unsigned char>);

extern uint64 TIFFGetStrileOffset(TIFF *tif, uint32 strile);
extern uint64 TIFFGetStrileByteCount(TIFF *tif, uint32 strile);
extern uint64 TIFFGetStrileOffsetWithErr(TIFF *tif, uint32 strile, int *pbErr);
extern uint64 TIFFGetStrileByteCountWithErr(TIFF *tif, uint32 strile, int *pbErr);

#ifdef LOGLUV_PUBLIC
#define U_NEU		0.210526316
#define V_NEU		0.473684211
#define UVSCALE		410.
extern double LogL16toY(int);
extern double LogL10toY(int);
extern void XYZtoRGB24(_Array_ptr<float> xyz, uint8 *rgb : itype(_Array_ptr<uint8>));
extern int uv_decode(_Ptr<double> up, _Ptr<double> vp, int);
extern void LogLuv24toXYZ(uint32, float *XYZ : itype(_Array_ptr<float>));
extern void LogLuv32toXYZ(uint32, float *XYZ : itype(_Array_ptr<float>));
#if defined(c_plusplus) || defined(__cplusplus)
extern int LogL16fromY(double, int = SGILOGENCODE_NODITHER);
extern int LogL10fromY(double, int = SGILOGENCODE_NODITHER);
extern int uv_encode(double, double, int = SGILOGENCODE_NODITHER);
extern uint32 LogLuv24fromXYZ(float*, int = SGILOGENCODE_NODITHER);
extern uint32 LogLuv32fromXYZ(float*, int = SGILOGENCODE_NODITHER);
#else
extern int LogL16fromY(double, int);
extern int LogL10fromY(double, int);
extern int uv_encode(double, double, int);
extern uint32 LogLuv24fromXYZ(float *XYZ : itype(_Array_ptr<float>), int);
extern uint32 LogLuv32fromXYZ(float *XYZ : itype(_Array_ptr<float>), int);
#endif
#endif /* LOGLUV_PUBLIC */

int TIFFCIELabToRGBInit(TIFFCIELabToRGB *cielab : itype(_Ptr<TIFFCIELabToRGB>), const TIFFDisplay *display, _Array_ptr<float> refWhite);
void TIFFCIELabToXYZ(TIFFCIELabToRGB *cielab : itype(_Ptr<TIFFCIELabToRGB>), uint32 l, int32 a, int32 b, _Ptr<float> X, _Ptr<float> Y, _Ptr<float> Z);
void TIFFXYZToRGB(TIFFCIELabToRGB *cielab : itype(_Ptr<TIFFCIELabToRGB>), float X, float Y, float Z, _Ptr<uint32> r, _Ptr<uint32> g, _Ptr<uint32> b);

int TIFFYCbCrToRGBInit(TIFFYCbCrToRGB *ycbcr, _Array_ptr<float> luma, _Array_ptr<float> refBlackWhite);
void TIFFYCbCrtoRGB(TIFFYCbCrToRGB *ycbcr : itype(_Ptr<TIFFYCbCrToRGB>), uint32 Y, int32 Cb, int32 Cr, _Ptr<uint32> r, _Ptr<uint32> g, _Ptr<uint32> b);

/****************************************************************************
 *               O B S O L E T E D    I N T E R F A C E S
 *
 * Don't use this stuff in your applications, it may be removed in the future
 * libtiff versions.
 ****************************************************************************/
typedef	struct {
	ttag_t	field_tag;		/* field's tag */
	short	field_readcount;	/* read count/TIFF_VARIABLE/TIFF_SPP */
	short	field_writecount;	/* write count/TIFF_VARIABLE */
	TIFFDataType field_type;	/* type of associated data */
        unsigned short field_bit;	/* bit in fieldsset bit vector */
	unsigned char field_oktochange;	/* if true, can change while writing */
	unsigned char field_passcount;	/* if true, pass dir count on set */
	char	*field_name;		/* ASCII name */
} TIFFFieldInfo;

extern int TIFFMergeFieldInfo(TIFF *, _Array_ptr<const TIFFFieldInfo> info, uint32);
        
#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif /* _TIFFIO_ */

/* vim: set ts=8 sts=8 sw=8 noet: */
/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 8
 * fill-column: 78
 * End:
 */
