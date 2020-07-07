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

/*
 * TIFF Library.
 *
 * Directory Printing Support
 */
#include "tiffiop.h"
#include <stdio.h>

#include <ctype.h>

static void _TIFFprintAsciiBounded(FILE *fd, const char *cp, size_t max_chars);

static const char * const photoNames[] = {
    "min-is-white",				/* PHOTOMETRIC_MINISWHITE */
    "min-is-black",				/* PHOTOMETRIC_MINISBLACK */
    "RGB color",				/* PHOTOMETRIC_RGB */
    "palette color (RGB from colormap)",	/* PHOTOMETRIC_PALETTE */
    "transparency mask",			/* PHOTOMETRIC_MASK */
    "separated",				/* PHOTOMETRIC_SEPARATED */
    "YCbCr",					/* PHOTOMETRIC_YCBCR */
    "7 (0x7)",
    "CIE L*a*b*",				/* PHOTOMETRIC_CIELAB */
    "ICC L*a*b*",				/* PHOTOMETRIC_ICCLAB */
    "ITU L*a*b*" 				/* PHOTOMETRIC_ITULAB */
};
#define	NPHOTONAMES	(sizeof (photoNames) / sizeof (photoNames[0]))

static const char * const orientNames[] = {
    "0 (0x0)",
    "row 0 top, col 0 lhs",			/* ORIENTATION_TOPLEFT */
    "row 0 top, col 0 rhs",			/* ORIENTATION_TOPRIGHT */
    "row 0 bottom, col 0 rhs",			/* ORIENTATION_BOTRIGHT */
    "row 0 bottom, col 0 lhs",			/* ORIENTATION_BOTLEFT */
    "row 0 lhs, col 0 top",			/* ORIENTATION_LEFTTOP */
    "row 0 rhs, col 0 top",			/* ORIENTATION_RIGHTTOP */
    "row 0 rhs, col 0 bottom",			/* ORIENTATION_RIGHTBOT */
    "row 0 lhs, col 0 bottom",			/* ORIENTATION_LEFTBOT */
};
#define	NORIENTNAMES	(sizeof (orientNames) / sizeof (orientNames[0]))

static void _TIFFPrintField(FILE *fd, const TIFFField *fip : itype(_Ptr<const TIFFField>), uint32 value_count, void *raw_data)
{
	uint32 j;
		
	fprintf(fd, ((const char *)"  %s: "), fip->field_name);

	for(j = 0; j < value_count; j++) {
		if(fip->field_type == TIFF_BYTE)
			fprintf(fd, ((const char *)"%u"), ((uint8 *) raw_data)[j]);
		else if(fip->field_type == TIFF_UNDEFINED)
			fprintf(fd, ((const char *)"0x%x"),
			    (unsigned int) ((unsigned char *) raw_data)[j]);
		else if(fip->field_type == TIFF_SBYTE)
			fprintf(fd, ((const char *)"%d"), ((int8 *) raw_data)[j]);
		else if(fip->field_type == TIFF_SHORT)
			fprintf(fd, ((const char *)"%u"), ((uint16 *) raw_data)[j]);
		else if(fip->field_type == TIFF_SSHORT)
			fprintf(fd, ((const char *)"%d"), ((int16 *) raw_data)[j]);
		else if(fip->field_type == TIFF_LONG)
			fprintf(fd, ((const char *)"%lu"),
			    (unsigned long)((uint32 *) raw_data)[j]);
		else if(fip->field_type == TIFF_SLONG)
			fprintf(fd, ((const char *)"%ld"), (long)((int32 *) raw_data)[j]);
		else if(fip->field_type == TIFF_IFD)
			fprintf(fd, ((const char *)"0x%lx"),
				(unsigned long)((uint32 *) raw_data)[j]);
		else if(fip->field_type == TIFF_RATIONAL
			|| fip->field_type == TIFF_SRATIONAL
			|| fip->field_type == TIFF_FLOAT)
			fprintf(fd, ((const char *)"%f"), ((float *) raw_data)[j]);
		else if(fip->field_type == TIFF_LONG8)
#if defined(__WIN32__) && (defined(_MSC_VER) || defined(__MINGW32__))
			fprintf(fd, "%I64u",
			    (unsigned __int64)((uint64 *) raw_data)[j]);
#else
			fprintf(fd, ((const char *)"%llu"),
			    (unsigned long long)((uint64 *) raw_data)[j]);
#endif
		else if(fip->field_type == TIFF_SLONG8)
#if defined(__WIN32__) && (defined(_MSC_VER) || defined(__MINGW32__))
			fprintf(fd, "%I64d", (__int64)((int64 *) raw_data)[j]);
#else
			fprintf(fd, ((const char *)"%lld"), (long long)((int64 *) raw_data)[j]);
#endif
		else if(fip->field_type == TIFF_IFD8)
#if defined(__WIN32__) && (defined(_MSC_VER) || defined(__MINGW32__))
			fprintf(fd, "0x%I64x",
				(unsigned __int64)((uint64 *) raw_data)[j]);
#else
			fprintf(fd, ((const char *)"0x%llx"),
				(unsigned long long)((uint64 *) raw_data)[j]);
#endif
		else if(fip->field_type == TIFF_FLOAT)
			fprintf(fd, ((const char *)"%f"), ((float *)raw_data)[j]);
		else if(fip->field_type == TIFF_DOUBLE)
			fprintf(fd, ((const char *)"%f"), ((double *) raw_data)[j]);
		else if(fip->field_type == TIFF_ASCII) {
			fprintf(fd, ((const char *)"%s"), (char *) raw_data);
			break;
		}
		else {
			fprintf(fd, ((const char *)"<unsupported data type in TIFFPrint>"));
			break;
		}

		if(j < value_count - 1)
			fprintf(fd, ((const char *)","));
	}

	fprintf(fd, ((const char *)"\n"));
}

static int _TIFFPrettyPrintField(TIFF *tif : itype(_Ptr<TIFF>), const TIFFField *fip : itype(_Ptr<const TIFFField>), FILE *fd, uint32 tag, uint32 value_count, void *raw_data)
{
        (void) tif;

	/* do not try to pretty print auto-defined fields */
	if (strncmp(fip->field_name,((const char *)"Tag "), 4) == 0) {
		return 0;
	}
        
	switch (tag)
	{
		case TIFFTAG_INKSET:
			if (value_count == 2 && fip->field_type == TIFF_SHORT) {
				fprintf(fd, ((const char *)"  Ink Set: "));
				switch (*((uint16*)raw_data)) {
				case INKSET_CMYK:
					fprintf(fd, ((const char *)"CMYK\n"));
					break;
				default:
					fprintf(fd, ((const char *)"%u (0x%x)\n"),
						*((uint16*)raw_data),
						*((uint16*)raw_data));
					break;
				}
				return 1;
			}
			return 0;

		case TIFFTAG_DOTRANGE:
			if (value_count == 2 && fip->field_type == TIFF_SHORT) {
				fprintf(fd, ((const char *)"  Dot Range: %u-%u\n"),
					((uint16*)raw_data)[0], ((uint16*)raw_data)[1]);
				return 1;
			}
			return 0;

		case TIFFTAG_WHITEPOINT:
			if (value_count == 2 && fip->field_type == TIFF_RATIONAL) {
				fprintf(fd, ((const char *)"  White Point: %g-%g\n"),
					((float *)raw_data)[0], ((float *)raw_data)[1]);
				return 1;
			} 
			return 0;

		case TIFFTAG_XMLPACKET:
		{
			uint32 i;

			fprintf(fd, ((const char *)"  XMLPacket (XMP Metadata):\n") );
			for(i = 0; i < value_count; i++)
				fputc(((char *)raw_data)[i], fd);
			fprintf( fd, ((const char *)"\n") );
			return 1;
		}
		case TIFFTAG_RICHTIFFIPTC:
			/*
			 * XXX: for some weird reason RichTIFFIPTC tag
			 * defined as array of LONG values.
			 */
			fprintf(fd,
			    ((const char *)"  RichTIFFIPTC Data: <present>, %lu bytes\n"),
			    (unsigned long) value_count * 4);
			return 1;

		case TIFFTAG_PHOTOSHOP:
			fprintf(fd, ((const char *)"  Photoshop Data: <present>, %lu bytes\n"),
			    (unsigned long) value_count);
			return 1;

		case TIFFTAG_ICCPROFILE:
			fprintf(fd, ((const char *)"  ICC Profile: <present>, %lu bytes\n"),
			    (unsigned long) value_count);
			return 1;

		case TIFFTAG_STONITS:
			if (value_count == 1 && fip->field_type == TIFF_DOUBLE) { 
				fprintf(fd,
					((const char *)"  Sample to Nits conversion factor: %.4e\n"),
					*((double*)raw_data));
				return 1;
			}
			return 0;
	}

	return 0;
}

/*
 * Print the contents of the current directory
 * to the specified stdio file stream.
 */
void TIFFPrintDirectory(TIFF *tif, FILE *fd, long flags)
{
	_Ptr<TIFFDirectory> td =  &tif->tif_dir;
	char *sep;
	long l, n;

#if defined(__WIN32__) && (defined(_MSC_VER) || defined(__MINGW32__))
	fprintf(fd, "TIFF Directory at offset 0x%I64x (%I64u)\n",
		(unsigned __int64) tif->tif_diroff,
		(unsigned __int64) tif->tif_diroff);
#else
	fprintf(fd, ((const char *)"TIFF Directory at offset 0x%llx (%llu)\n"),
		(unsigned long long) tif->tif_diroff,
		(unsigned long long) tif->tif_diroff);
#endif
	if (TIFFFieldSet(tif,FIELD_SUBFILETYPE)) {
		fprintf(fd, ((const char *)"  Subfile Type:"));
		sep = " ";
		if (td->td_subfiletype & FILETYPE_REDUCEDIMAGE) {
			fprintf(fd, ((const char *)"%sreduced-resolution image"), sep);
			sep = "/";
		}
		if (td->td_subfiletype & FILETYPE_PAGE) {
			fprintf(fd, ((const char *)"%smulti-page document"), sep);
			sep = "/";
		}
		if (td->td_subfiletype & FILETYPE_MASK)
			fprintf(fd, ((const char *)"%stransparency mask"), sep);
		fprintf(fd, ((const char *)" (%lu = 0x%lx)\n"),
		    (unsigned long) td->td_subfiletype, (long) td->td_subfiletype);
	}
	if (TIFFFieldSet(tif,FIELD_IMAGEDIMENSIONS)) {
		fprintf(fd, ((const char *)"  Image Width: %lu Image Length: %lu"),
		    (unsigned long) td->td_imagewidth, (unsigned long) td->td_imagelength);
		if (TIFFFieldSet(tif,FIELD_IMAGEDEPTH))
			fprintf(fd, ((const char *)" Image Depth: %lu"),
			    (unsigned long) td->td_imagedepth);
		fprintf(fd, ((const char *)"\n"));
	}
	if (TIFFFieldSet(tif,FIELD_TILEDIMENSIONS)) {
		fprintf(fd, ((const char *)"  Tile Width: %lu Tile Length: %lu"),
		    (unsigned long) td->td_tilewidth, (unsigned long) td->td_tilelength);
		if (TIFFFieldSet(tif,FIELD_TILEDEPTH))
			fprintf(fd, ((const char *)" Tile Depth: %lu"),
			    (unsigned long) td->td_tiledepth);
		fprintf(fd, ((const char *)"\n"));
	}
	if (TIFFFieldSet(tif,FIELD_RESOLUTION)) {
		fprintf(fd, ((const char *)"  Resolution: %g, %g"),
		    td->td_xresolution, td->td_yresolution);
		if (TIFFFieldSet(tif,FIELD_RESOLUTIONUNIT)) {
			switch (td->td_resolutionunit) {
			case RESUNIT_NONE:
				fprintf(fd, ((const char *)" (unitless)"));
				break;
			case RESUNIT_INCH:
				fprintf(fd, ((const char *)" pixels/inch"));
				break;
			case RESUNIT_CENTIMETER:
				fprintf(fd, ((const char *)" pixels/cm"));
				break;
			default:
				fprintf(fd, ((const char *)" (unit %u = 0x%x)"),
				    td->td_resolutionunit,
				    td->td_resolutionunit);
				break;
			}
		}
		fprintf(fd, ((const char *)"\n"));
	}
	if (TIFFFieldSet(tif,FIELD_POSITION))
		fprintf(fd, ((const char *)"  Position: %g, %g\n"),
		    td->td_xposition, td->td_yposition);
	if (TIFFFieldSet(tif,FIELD_BITSPERSAMPLE))
		fprintf(fd, ((const char *)"  Bits/Sample: %u\n"), td->td_bitspersample);
	if (TIFFFieldSet(tif,FIELD_SAMPLEFORMAT)) {
		fprintf(fd, ((const char *)"  Sample Format: "));
		switch (td->td_sampleformat) {
		case SAMPLEFORMAT_VOID:
			fprintf(fd, ((const char *)"void\n"));
			break;
		case SAMPLEFORMAT_INT:
			fprintf(fd, ((const char *)"signed integer\n"));
			break;
		case SAMPLEFORMAT_UINT:
			fprintf(fd, ((const char *)"unsigned integer\n"));
			break;
		case SAMPLEFORMAT_IEEEFP:
			fprintf(fd, ((const char *)"IEEE floating point\n"));
			break;
		case SAMPLEFORMAT_COMPLEXINT:
			fprintf(fd, ((const char *)"complex signed integer\n"));
			break;
		case SAMPLEFORMAT_COMPLEXIEEEFP:
			fprintf(fd, ((const char *)"complex IEEE floating point\n"));
			break;
		default:
			fprintf(fd, ((const char *)"%u (0x%x)\n"),
			    td->td_sampleformat, td->td_sampleformat);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_COMPRESSION)) {
		const TIFFCodec* c = TIFFFindCODEC(td->td_compression);
		fprintf(fd, ((const char *)"  Compression Scheme: "));
		if (c)
			fprintf(fd, ((const char *)"%s\n"), c->name);
		else
			fprintf(fd, ((const char *)"%u (0x%x)\n"),
			    td->td_compression, td->td_compression);
	}
	if (TIFFFieldSet(tif,FIELD_PHOTOMETRIC)) {
		fprintf(fd, ((const char *)"  Photometric Interpretation: "));
		if (td->td_photometric < NPHOTONAMES)
			fprintf(fd, ((const char *)"%s\n"), photoNames[td->td_photometric]);
		else {
			switch (td->td_photometric) {
			case PHOTOMETRIC_LOGL:
				fprintf(fd, ((const char *)"CIE Log2(L)\n"));
				break;
			case PHOTOMETRIC_LOGLUV:
				fprintf(fd, ((const char *)"CIE Log2(L) (u',v')\n"));
				break;
			default:
				fprintf(fd, ((const char *)"%u (0x%x)\n"),
				    td->td_photometric, td->td_photometric);
				break;
			}
		}
	}
	if (TIFFFieldSet(tif,FIELD_EXTRASAMPLES) && td->td_extrasamples) {
		uint16 i;
		fprintf(fd, ((const char *)"  Extra Samples: %u<"), td->td_extrasamples);
		sep = "";
		for (i = 0; i < td->td_extrasamples; i++) {
			switch (td->td_sampleinfo[i]) {
			case EXTRASAMPLE_UNSPECIFIED:
				fprintf(fd, ((const char *)"%sunspecified"), sep);
				break;
			case EXTRASAMPLE_ASSOCALPHA:
				fprintf(fd, ((const char *)"%sassoc-alpha"), sep);
				break;
			case EXTRASAMPLE_UNASSALPHA:
				fprintf(fd, ((const char *)"%sunassoc-alpha"), sep);
				break;
			default:
				fprintf(fd, ((const char *)"%s%u (0x%x)"), sep,
				    td->td_sampleinfo[i], td->td_sampleinfo[i]);
				break;
			}
			sep = ", ";
		}
		fprintf(fd, ((const char *)">\n"));
	}
	if (TIFFFieldSet(tif,FIELD_INKNAMES)) {
		char* cp;
		uint16 i;
		fprintf(fd, ((const char *)"  Ink Names: "));
		i = td->td_samplesperpixel;
		sep = "";
		for (cp = td->td_inknames; 
		     i > 0 && cp < td->td_inknames + td->td_inknameslen; 
		     cp = strchr(cp,'\0')+1, i--) {
			size_t max_chars = 
				td->td_inknameslen - (cp - td->td_inknames);
			fputs(sep, fd);
			_TIFFprintAsciiBounded(fd, cp, max_chars);
			sep = ", ";
		}
                fputs(((const char *)"\n"), fd);
	}
	if (TIFFFieldSet(tif,FIELD_THRESHHOLDING)) {
		fprintf(fd, ((const char *)"  Thresholding: "));
		switch (td->td_threshholding) {
		case THRESHHOLD_BILEVEL:
			fprintf(fd, ((const char *)"bilevel art scan\n"));
			break;
		case THRESHHOLD_HALFTONE:
			fprintf(fd, ((const char *)"halftone or dithered scan\n"));
			break;
		case THRESHHOLD_ERRORDIFFUSE:
			fprintf(fd, ((const char *)"error diffused\n"));
			break;
		default:
			fprintf(fd, ((const char *)"%u (0x%x)\n"),
			    td->td_threshholding, td->td_threshholding);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_FILLORDER)) {
		fprintf(fd, ((const char *)"  FillOrder: "));
		switch (td->td_fillorder) {
		case FILLORDER_MSB2LSB:
			fprintf(fd, ((const char *)"msb-to-lsb\n"));
			break;
		case FILLORDER_LSB2MSB:
			fprintf(fd, ((const char *)"lsb-to-msb\n"));
			break;
		default:
			fprintf(fd, ((const char *)"%u (0x%x)\n"),
			    td->td_fillorder, td->td_fillorder);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_YCBCRSUBSAMPLING))
        {
		fprintf(fd, ((const char *)"  YCbCr Subsampling: %u, %u\n"),
			td->td_ycbcrsubsampling[0], td->td_ycbcrsubsampling[1] );
	}
	if (TIFFFieldSet(tif,FIELD_YCBCRPOSITIONING)) {
		fprintf(fd, ((const char *)"  YCbCr Positioning: "));
		switch (td->td_ycbcrpositioning) {
		case YCBCRPOSITION_CENTERED:
			fprintf(fd, ((const char *)"centered\n"));
			break;
		case YCBCRPOSITION_COSITED:
			fprintf(fd, ((const char *)"cosited\n"));
			break;
		default:
			fprintf(fd, ((const char *)"%u (0x%x)\n"),
			    td->td_ycbcrpositioning, td->td_ycbcrpositioning);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_HALFTONEHINTS))
		fprintf(fd, ((const char *)"  Halftone Hints: light %u dark %u\n"),
		    td->td_halftonehints[0], td->td_halftonehints[1]);
	if (TIFFFieldSet(tif,FIELD_ORIENTATION)) {
		fprintf(fd, ((const char *)"  Orientation: "));
		if (td->td_orientation < NORIENTNAMES)
			fprintf(fd, ((const char *)"%s\n"), orientNames[td->td_orientation]);
		else
			fprintf(fd, ((const char *)"%u (0x%x)\n"),
			    td->td_orientation, td->td_orientation);
	}
	if (TIFFFieldSet(tif,FIELD_SAMPLESPERPIXEL))
		fprintf(fd, ((const char *)"  Samples/Pixel: %u\n"), td->td_samplesperpixel);
	if (TIFFFieldSet(tif,FIELD_ROWSPERSTRIP)) {
		fprintf(fd, ((const char *)"  Rows/Strip: "));
		if (td->td_rowsperstrip == (uint32) -1)
			fprintf(fd, ((const char *)"(infinite)\n"));
		else
			fprintf(fd, ((const char *)"%lu\n"), (unsigned long) td->td_rowsperstrip);
	}
	if (TIFFFieldSet(tif,FIELD_MINSAMPLEVALUE))
		fprintf(fd, ((const char *)"  Min Sample Value: %u\n"), td->td_minsamplevalue);
	if (TIFFFieldSet(tif,FIELD_MAXSAMPLEVALUE))
		fprintf(fd, ((const char *)"  Max Sample Value: %u\n"), td->td_maxsamplevalue);
	if (TIFFFieldSet(tif,FIELD_SMINSAMPLEVALUE)) {
		int i;
		int count = (tif->tif_flags & TIFF_PERSAMPLE) ? td->td_samplesperpixel : 1;
		fprintf(fd, ((const char *)"  SMin Sample Value:"));
		for (i = 0; i < count; ++i)
			fprintf(fd, ((const char *)" %g"), td->td_sminsamplevalue[i]);
		fprintf(fd, ((const char *)"\n"));
	}
	if (TIFFFieldSet(tif,FIELD_SMAXSAMPLEVALUE)) {
		int i;
		int count = (tif->tif_flags & TIFF_PERSAMPLE) ? td->td_samplesperpixel : 1;
		fprintf(fd, ((const char *)"  SMax Sample Value:"));
		for (i = 0; i < count; ++i)
			fprintf(fd, ((const char *)" %g"), td->td_smaxsamplevalue[i]);
		fprintf(fd, ((const char *)"\n"));
	}
	if (TIFFFieldSet(tif,FIELD_PLANARCONFIG)) {
		fprintf(fd, ((const char *)"  Planar Configuration: "));
		switch (td->td_planarconfig) {
		case PLANARCONFIG_CONTIG:
			fprintf(fd, ((const char *)"single image plane\n"));
			break;
		case PLANARCONFIG_SEPARATE:
			fprintf(fd, ((const char *)"separate image planes\n"));
			break;
		default:
			fprintf(fd, ((const char *)"%u (0x%x)\n"),
			    td->td_planarconfig, td->td_planarconfig);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_PAGENUMBER))
		fprintf(fd, ((const char *)"  Page Number: %u-%u\n"),
		    td->td_pagenumber[0], td->td_pagenumber[1]);
	if (TIFFFieldSet(tif,FIELD_COLORMAP)) {
		fprintf(fd, ((const char *)"  Color Map: "));
		if (flags & TIFFPRINT_COLORMAP) {
			fprintf(fd, ((const char *)"\n"));
			n = 1L<<td->td_bitspersample;
			for (l = 0; l < n; l++)
				fprintf(fd, ((const char *)"   %5ld: %5u %5u %5u\n"),
				    l,
				    td->td_colormap[0][l],
				    td->td_colormap[1][l],
				    td->td_colormap[2][l]);
		} else
			fprintf(fd, ((const char *)"(present)\n"));
	}
	if (TIFFFieldSet(tif,FIELD_REFBLACKWHITE)) {
		int i;
		fprintf(fd, ((const char *)"  Reference Black/White:\n"));
		for (i = 0; i < 3; i++)
		fprintf(fd, ((const char *)"    %2d: %5g %5g\n"), i,
			td->td_refblackwhite[2*i+0],
			td->td_refblackwhite[2*i+1]);
	}
	if (TIFFFieldSet(tif,FIELD_TRANSFERFUNCTION)) {
		fprintf(fd, ((const char *)"  Transfer Function: "));
		if (flags & TIFFPRINT_CURVES) {
			fprintf(fd, ((const char *)"\n"));
			n = 1L<<td->td_bitspersample;
			for (l = 0; l < n; l++) {
				uint16 i;
				fprintf(fd, ((const char *)"    %2ld: %5u"),
				    l, td->td_transferfunction[0][l]);
				for (i = 1; i < td->td_samplesperpixel - td->td_extrasamples && i < 3; i++)
					fprintf(fd, ((const char *)" %5u"),
					    td->td_transferfunction[i][l]);
				fputc('\n', fd);
			}
		} else
			fprintf(fd, ((const char *)"(present)\n"));
	}
	if (TIFFFieldSet(tif, FIELD_SUBIFD) && (td->td_subifd)) {
		uint16 i;
		fprintf(fd, ((const char *)"  SubIFD Offsets:"));
		for (i = 0; i < td->td_nsubifd; i++)
#if defined(__WIN32__) && (defined(_MSC_VER) || defined(__MINGW32__))
			fprintf(fd, " %5I64u",
				(unsigned __int64) td->td_subifd[i]);
#else
			fprintf(fd, ((const char *)" %5llu"),
				(unsigned long long) td->td_subifd[i]);
#endif
		fputc('\n', fd);
	}

	/*
	** Custom tag support.
	*/
	{
		int  i;
		short count;

		count = (short) TIFFGetTagListCount(tif);
		for(i = 0; i < count; i++) {
			uint32 tag = TIFFGetTagListEntry(tif, i);
			const TIFFField *fip;
			uint32 value_count;
			int mem_alloc = 0;
			void *raw_data;

			fip = TIFFFieldWithTag(tif, tag);
			if(fip == NULL)
				continue;

			if(fip->field_passcount) {
				if (fip->field_readcount == TIFF_VARIABLE2 ) {
					if(TIFFGetField(tif, tag, &value_count, &raw_data) != 1)
						continue;
				} else if (fip->field_readcount == TIFF_VARIABLE ) {
					uint16 small_value_count;
					if(TIFFGetField(tif, tag, &small_value_count, &raw_data) != 1)
						continue;
					value_count = small_value_count;
				} else {
					assert (fip->field_readcount == TIFF_VARIABLE
						|| fip->field_readcount == TIFF_VARIABLE2);
					continue;
				} 
			} else {
				if (fip->field_readcount == TIFF_VARIABLE
				    || fip->field_readcount == TIFF_VARIABLE2)
					value_count = 1;
				else if (fip->field_readcount == TIFF_SPP)
					value_count = td->td_samplesperpixel;
				else
					value_count = fip->field_readcount;
				if (fip->field_tag == TIFFTAG_DOTRANGE
				    && strcmp(fip->field_name,((const char *)"DotRange")) == 0) {
					/* TODO: This is an evil exception and should not have been
					   handled this way ... likely best if we move it into
					   the directory structure with an explicit field in 
					   libtiff 4.1 and assign it a FIELD_ value */
					static uint16 dotrange[2];
					raw_data = dotrange;
					TIFFGetField(tif, tag, dotrange+0, dotrange+1);
				} else if (fip->field_type == TIFF_ASCII
					   || fip->field_readcount == TIFF_VARIABLE
					   || fip->field_readcount == TIFF_VARIABLE2
					   || fip->field_readcount == TIFF_SPP
					   || value_count > 1) {
					if(TIFFGetField(tif, tag, &raw_data) != 1)
						continue;
				} else {
					raw_data = _TIFFmalloc(
					    _TIFFDataSize(fip->field_type)
					    * value_count);
					mem_alloc = 1;
					if(TIFFGetField(tif, tag, raw_data) != 1) {
						_TIFFfree(raw_data);
						continue;
					}
				}
			}

			/*
			 * Catch the tags which needs to be specially handled
			 * and pretty print them. If tag not handled in
			 * _TIFFPrettyPrintField() fall down and print it as
			 * any other tag.
			 */
			if (!_TIFFPrettyPrintField(tif, fip, fd, tag, value_count, raw_data))
				_TIFFPrintField(fd, fip, value_count, raw_data);

			if(mem_alloc)
				_TIFFfree(raw_data);
		}
	}
        
	if (tif->tif_tagmethods.printdir)
		(*tif->tif_tagmethods.printdir)(tif, fd, flags);

	if ((flags & TIFFPRINT_STRIPS) &&
	    TIFFFieldSet(tif,FIELD_STRIPOFFSETS)) {
		uint32 s;

		fprintf(fd, ((const char *)"  %lu %s:\n"),
		    (unsigned long) td->td_nstrips,
		    isTiled(tif) ? "Tiles" : "Strips");
		for (s = 0; s < td->td_nstrips; s++)
#if defined(__WIN32__) && (defined(_MSC_VER) || defined(__MINGW32__))
			fprintf(fd, "    %3lu: [%8I64u, %8I64u]\n",
			    (unsigned long) s,
			    (unsigned __int64) TIFFGetStrileOffset(tif, s),
			    (unsigned __int64) TIFFGetStrileByteCount(tif, s));
#else
			fprintf(fd, ((const char *)"    %3lu: [%8llu, %8llu]\n"),
			    (unsigned long) s,
			    (unsigned long long) TIFFGetStrileOffset(tif, s),
			    (unsigned long long) TIFFGetStrileByteCount(tif, s));
#endif
	}
}

void _TIFFprintAscii(FILE *fd, const char *cp)
{
	_TIFFprintAsciiBounded( fd, cp, strlen(cp));
}

static void _TIFFprintAsciiBounded(FILE *fd, const char *cp, size_t max_chars)
{
	for (; max_chars > 0 && *cp != '\0'; cp++, max_chars--) {
		const char* tp;

		if (isprint((int)*cp)) {
			fputc(*cp, fd);
			continue;
		}
		for (tp = "\tt\bb\rr\nn\vv"; *tp; tp++)
			if (*tp++ == *cp)
				break;
		if (*tp)
			fprintf(fd, ((const char *)"\\%c"), *tp);
		else
			fprintf(fd, ((const char *)"\\%03o"), *cp & 0xff);
	}
}

void _TIFFprintAsciiTag(FILE *fd, _Ptr<const char> name, const char *value)
{
	fprintf(fd, ((const char *)"  %s: \""), name);
	_TIFFprintAscii(fd, value);
	fprintf(fd, ((const char *)"\"\n"));
}

/* vim: set ts=8 sts=8 sw=8 noet: */
/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 8
 * fill-column: 78
 * End:
 */
