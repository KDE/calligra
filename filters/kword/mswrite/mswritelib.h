/* This file is part of the MSWriteView project
   Copyright (C) 2001, 2002 Clarence Dang <CTRL_CD@bigpond.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef MSWRITE_IMPORT_LIB_H
#define MSWRITE_IMPORT_LIB_H

// CORE LIB VERSION NUMBER //
#define MSWRITE_IMPORT_LIB_VERSION 				0.392
#define MSWRITE_IMPORT_LIB_VERSION_STRING		"0.392"		// TODO: there's got to be better way

// DATA TYPES //
typedef signed char BYTE;			// 1 byte
typedef signed short WORD;			// 2 bytes
typedef signed int DWORD;			// 4 bytes

typedef unsigned char UBYTE;		// 1 byte
typedef unsigned short UWORD;		// 2 bytes
typedef unsigned int UDWORD;		// 4 bytes

typedef WORD POINT;					// 2 bytes

// CONSTANTS //
const int MSWRITE_PAGE_SIZE = 128;			// size of each page
const int MSWRITE_BUFFER_SIZE = 1000;		// max. no. bytes passed to any function declared in core lib

enum MSWRITE_OBJECT
{
	MSWRITE_OBJECT_OLE,
	MSWRITE_OBJECT_WMF,	// (image)
	MSWRITE_OBJECT_BMP	// (image)
};

// ERROR CODES //
// TODO: actually make use of these
enum MSWRITE_ERR
{
	MSWRITE_ERR_SUCCESS 		= 0,		// = 0, just in case :)
	MSWRITE_ERR_FAIL			= 1,		// ditto; this return value is used for all other errors
	MSWRITE_ERR_INVFORMAT,
	MSWRITE_ERR_NOMEM,
	MSWRITE_ERR_INFILE,
	MSWRITE_ERR_OUTFILE
};

// TODO: wouldn't it be more efficient to not pack but to create
//	#define MSWRITE_<structure>_SIZEOF constants?
#pragma pack(1)

// Header for all MSWrite documents
class MSWRITE_HEADER
{
public:
	WORD magic;
	WORD zero;
	WORD magic2;
	WORD reserved [4];
	DWORD numBytes;
	WORD pnParaInfo;
	WORD pnFootnoteTable;
	WORD pnSectionProperty;
	WORD pnSectionTable;
	WORD pnPageTable;
	WORD pnFontTable;
	WORD reserved2 [33];
	WORD numPages;
};

// Section Property (page, border, margins... dimensions)
class MSWRITE_SECTION_PROPERTY
{
public:
	BYTE numDataBytes;		// number of bytes after this (seems to = 102)
	WORD reserved;				// "must be zero" (seems to = 512)
	WORD pageHeight;			// page height
	WORD pageWidth;			// page width
	WORD pageNumberStart;	//	[undocumented] starting counting page numbers
									// from this value (if its invalid, assume 1)
	WORD topMargin;			// top margin
	WORD textHeight;			// height of text
	WORD leftMargin;			// left margin
	WORD textWidth;			// width of text area
	// --- [undocumented] --- //
	WORD unknown;				// = 256
	WORD headerFromTop;		// distance header is from top of page
	WORD footerFromTop;		// distance footer is from top of page
	WORD unknown2 [5];		// 720, 0, 1080, ?, 0...
	BYTE pad [MSWRITE_PAGE_SIZE - 31];	// TODO: temporary

	MSWRITE_SECTION_PROPERTY ()
	{
		// set defaults
		numDataBytes		= 102;		// [undocumented]
		reserved 			= 512;		// [undocumented]
		pageHeight			= WORD (11 * 1440);
		pageWidth			= WORD (8.5 * 1440);
		pageNumberStart	= 1;
		topMargin			= WORD (1 * 1440);
		textHeight			= WORD (9 * 1440);
		leftMargin			= WORD (1.25 * 1440);
		textWidth			= WORD (6 * 1440);

		headerFromTop		= WORD (0.75 * 1440);
		footerFromTop		= pageHeight - WORD (0.75 * 1440);
	}

	POINT getPageHeight (void) const	{	return pageHeight / 20;	}
	POINT getPageWidth (void) const	{	return pageWidth / 20;	}
	POINT getTextHeight (void) const	{	return textHeight / 20;	}
	POINT getTextWidth (void) const	{	return textWidth / 20;	}

	int getFirstPageNumber (void) const
	{
		if (pageNumberStart >= 1)
			return pageNumberStart;
		else
			return 1;	// sources state that pageNumberStart could be -1...
	}

	POINT getTopMargin (void) const	{	return topMargin / 20;	}
	POINT getLeftMargin (void) const	{	return leftMargin / 20;	}
	POINT getBottomMargin (void) const
	{
		return getPageHeight () - getTopMargin () - getTextHeight ();
	}

	POINT getRightMargin (void) const
	{
		return getPageWidth () - getLeftMargin () - getTextWidth ();
	}

	POINT getHeaderFromTop (void) const	{	return headerFromTop / 20;	}
	POINT getFooterFromTop (void) const	{	return footerFromTop / 20;	}
};

class MSWRITE_SECTION_DESCRIPTOR
{
public:
	DWORD afterEndCharByte;		// byte of character just after end of section
	WORD undefined;				// undefined
	DWORD SEPlocation;			// byte address of corresponding MSWRITE_SECTION_PROPERTY
};

class MSWRITE_SECTION_TABLE
{
public:
	WORD numSections;				// aka. number of SEDs
	WORD undefined;				// undefined

	MSWRITE_SECTION_DESCRIPTOR sed [2];
};

class MSWRITE_FONT_TABLE_FFN
{
private:

public:
	MSWRITE_FONT_TABLE_FFN ()
	{
		numDataBytes = 0;
		fontFamily = 0;
		fontName = (BYTE *) NULL;
	}

	~MSWRITE_FONT_TABLE_FFN ()
	{
		delete [] fontName;
	}

	WORD numDataBytes;		// number of bytes after this
	BYTE fontFamily;
	BYTE *fontName;
};

// font family
enum MSWRITE_FF
{
	MSWRITE_FF_DONTCARE		= 0x00,
	MSWRITE_FF_ROMAN			= 0x10,
	MSWRITE_FF_SWISS			= 0x20,
	MSWRITE_FF_MODERN			= 0x30,
	MSWRITE_FF_SCRIPT			= 0x40,
	MSWRITE_FF_DECORATIVE	= 0x50
};

// font table (contains names of fonts used)
class MSWRITE_FONT_TABLE
{
public:
	MSWRITE_FONT_TABLE ()
	{
		numFFNs = 0;
		ffn = (MSWRITE_FONT_TABLE_FFN *) NULL;
	}

	~MSWRITE_FONT_TABLE ()
	{
		delete [] ffn;
	}

	WORD numFFNs;							// number of FFNs
	MSWRITE_FONT_TABLE_FFN *ffn;		// list of FFNs
};

class MSWRITE_FMT_INFOPAGE_FOD
{
public:
	DWORD afterEndCharByte;				// byte of first character after end of FOD's control
	WORD FPROPoffset; 					// byte offset of corresponding FPROP (from beginning of FOD array)
};

// Information pages for charInfo and paraInfo
// NOTE: sizeof (structure) to read in is MSWRITE_PAGE_SIZE -- not sizeof (MSWRITE_FMT_INFOPAGE)
class MSWRITE_FMT_INFOPAGE
{
public:
	DWORD firstCharByte;
	char data [126 - 4 + 1];
	BYTE numFODs;

	// --- DATA STRUCTURE ENDS HERE --- //

	// useful pointers
	MSWRITE_FMT_INFOPAGE_FOD *fod;	// array of FODs
												// (should point to data [0])
	void *fprop;							// group of fprops
												// (should point to data [n + 1 - 126])
};

// Character Information Property FPROP (in MSWRITE_FMT_INFOPAGE)
class MSWRITE_FPROP_CHP
{
public:
	BYTE numDataBytes;					// number of bytes following (= sizeof (MSWRITE_FROP_CHP) - sizeof (BYTE))

	// --- CHP BEINGS HERE ---
	BYTE reserved;
	BYTE generalMarkup;					// bold, italic, fontCodeLow
	BYTE fontSize;							// in half-points (standard=24)
	BYTE specialMarkup;					// underlined, res (x4), "(page)", res
	BYTE fontCodeHigh;					// fontCode (hi bits) (x3), res (x5)
	UBYTE position;						// 0=normal, 1-127=superscript, 128-255=subscript
	// --- CHP ENDS HERE ---

public:
	MSWRITE_FPROP_CHP ()
	{
		setDefaults ();
	}

	void setDefaults (void)
	{
		reserved = 1;
		generalMarkup = 0;
		fontSize = 24;
		specialMarkup = fontCodeHigh = position = 0;
	}

	UBYTE *getData (void) const
	{
		return (UBYTE *) &reserved;
	}

	int getFontCode (void) const
	{
		int fontCode;

		// get low bits from 123456ii (i = irrelevant)
		fontCode = ((UBYTE) generalMarkup >> 2);

		// get high bits from 123
		fontCode |= (((UWORD) fontCodeHigh & (1 + 2 + 4)) << 6);

		return fontCode;
	}

	bool isBold (void) const
	{
		// byte 1: bit 0 set
		return (generalMarkup & 1);
	}

	bool isItalic (void) const
	{
		// byte 1: bit 1 set
		return (generalMarkup & 2);
	}

	bool isUnderlined (void) const
	{
		// byte 3: bit 0 set
		return (specialMarkup & 1);
	}

	// In MSWrite, this is represented as "(page)"
	// Note: only applies to headers & footers
	bool isPageNumber (void) const
	{
		// byte 3: bit 6 set
		return (specialMarkup & (1 << 6));
	}

	int getFontSize (void) const
	{
		return fontSize;
	}

	int getFontPointSize (void) const
	{
		return fontSize / 2;
	}

	bool isNormalPosition (void) const
	{
		// byte 5: equals 0
		return (position == 0);
	}

	bool isSuperscript (void) const
	{
		// byte 5: >= 1 <= 127
		return (position >= 1 && position <= 127);
	}

	bool isSubscript (void) const
	{
		// byte 5: >= 128
		return (position >= 128);
	}
};

// tabulator (in MSWRITE_FPROP_PAP, obviously :))
class MSWRITE_FPROP_PAP_TBD
{
public:
	WORD tabNumPoints;
	BYTE type;
	BYTE reserved;

	MSWRITE_FPROP_PAP_TBD ()
	{
		setDefaults ();
	}

	void setDefaults (void)
	{
		tabNumPoints = type = reserved = 0;
	}

	WORD getTabNumPoints (void) const
	{
		return tabNumPoints / 20;
	}

	bool isTabEntry (void) const
	{
		return getTabNumPoints () > 0;
	}

	bool isTabNormal (void) const
	{
		return type == 0;
	}

	bool isTabDecimal (void) const
	{
		return type == 3;
	}
};

// types of justification
enum MSWRITE_JUSTIFY
{
	MSWRITE_JUSTIFY_LEFT		=	0,
	MSWRITE_JUSTIFY_CENTRE	=	1,
	MSWRITE_JUSTIFY_CENTER	=	MSWRITE_JUSTIFY_CENTRE,		// allow American spelling
	MSWRITE_JUSTIFY_RIGHT	=	2,
	MSWRITE_JUSTIFY_BOTH		=	3
};

// Paragraph Information Property FPROP (in MSWRITE_FMT_INFOPAGE)
class MSWRITE_FPROP_PAP
{
public:
	BYTE numDataBytes;		// number of bytes following

	// --- PAP starts here ---
	BYTE reserved;
	BYTE justification;
	WORD reserved2;
	WORD rightIndent, leftIndent, leftIndentFirstLine;
	WORD lineSpacing;
	WORD reserved3 [2];
	BYTE type;
	BYTE reserved4 [5];
public:
	MSWRITE_FPROP_PAP_TBD tbd [14];		// ends at byte 77, not 78 :)

public:
	MSWRITE_FPROP_PAP ()
	{
		setDefaults ();
	}

	void setDefaults (void)
	{
		// set defaults
		reserved = 61;				// could be wrong...
		justification = 0;
		reserved2 = 0;
		rightIndent = leftIndent = leftIndentFirstLine = 0;
		lineSpacing = 240;
		reserved3 [0] = reserved3 [1] = type = 0;
		memset (reserved4, 0, 5 * sizeof (BYTE));
		memset (tbd, 0, 14 * sizeof (MSWRITE_FPROP_PAP_TBD));
	}

	UBYTE *getData (void) const
	{
		return (UBYTE *) &reserved;
	}

	BYTE getJustification (void) const
	{
		return justification;
	}

	bool isObject (void) const
	{
		// byte 16: bit 4 set?
		return (type & (1 << 4));
	}

	bool isText (void) const
	{
		// byte 16: bit 4 not set?
		return (!isObject ());
	}

	WORD getLeftIndent (void) const
	{
		return leftIndent / 20;
	}

	// note: this is relative to getLeftIndex ()
	WORD getLeftIndentFirstLine (void) const
	{
		return leftIndentFirstLine / 20;
	}

	WORD getRightIndent (void) const
	{
		return rightIndent / 20;
	}

	// returns linespacing: 10=normal, 15, 20=double
	int getLineSpacing (void) const
	{
		return (int) (lineSpacing * 10 / 240);
	}

	bool isParagraph (void) const
	{
		// byte 16: bit 1-2 not set?
		return (((type >> 1) & (1 + 2)) == 0);
	}

	bool isHeader (void) const
	{
		// not paragraph and byte 16: bit 0 not set?
		return (!isParagraph () && ((type & 1) == 0));
	}

	bool isFooter (void) const
	{
		// not paragraph and byte 16: bit 0 set?
		return (!isParagraph () && (type & 1));
	}

	// whether or not a header or footer is on the first page
	bool isOnFirstPage (void) const
	{
		return (type & (1 << 3));
	}

};

// page descriptors
// NOTE: MSWRITE_PAGE_TABLE doesn't exist -- we instead use an array of these PGDs
class MSWRITE_PAGE_TABLE_PGD
{
public:
	WORD pageNumber;			// printed page number
	DWORD firstCharByte;		// byte of first character on page
};

// actual first header of a BMP (stored in .BMP file)
class MSWRITE_BITMAP_FILE_HEADER
{
public:
	WORD magic;
	DWORD totalBytes;
	WORD reserved [2];
	DWORD actualImageOffset;
};

// actual second header of a BMP (stored in .BMP file)
class MSWRITE_BITMAP_INFO_HEADER
{
public:
	DWORD size;
	long width;
	long height;
	WORD planes;
	WORD bitCount;
	DWORD compression;
	DWORD sizeImage;
	long xPixelsPerMetre;
	long yPixelsPerMetre;
	DWORD coloursUsed;
	DWORD coloursImportant;
};

// colour table is made of MSWRITE_BITMAP_COLOUR_INDEX's
// (number of elements = 1 << MSWRITE_BITMAP_INFO_HEADER::bitCount)
class MSWRITE_BITMAP_COLOUR_INDEX
{
public:
	UBYTE blue, green, red, reserved;
};

// 14-byte header for bitmaps INSIDE MSWrite documents only
class MSWRITE_IMAGE_BITMAP_HEADER
{
public:
	WORD zero;
	WORD width;
	WORD height;
	WORD widthBytes;
	BYTE planes;
	BYTE bitsPerPixel;
	DWORD nullUselessPointer;
};

// major header for images in .wri documents
class MSWRITE_IMAGE_HEADER
{
public:
	// --- METAFILEPICT beigns ---
	WORD mappingMode;
	WORD MFPwidth;
	WORD MFPheight;
	WORD MFPignored;
	// --- METAFILEPICT ends ---

	WORD offsetFromLeftMargin;		// offset from left margin (pt / 20)
	WORD width;							// length (pt / 20)
	WORD height;						// height (pt / 20)
	WORD cbOldSize;					// TODO: = 0 ???

	// bitmap header
	union
	{
		BYTE bm [14];	// TODO: remove this byte array once bitmap debugging is complete
		MSWRITE_IMAGE_BITMAP_HEADER bmh;
	};

	WORD numHeaderBytes;				// number of bytes in this header
	DWORD numDataBytes;				// bytes of actual data
	WORD widthScaledRel1000;		// scaling (/1000)*100%
	WORD heightScaledRel1000;		// scaling (/1000)*100%
};

// major header for OLE objects in .wri documents
class MSWRITE_OLE_HEADER
{
public:
	WORD mappingMode;					// 0xe4
	DWORD unused;
	WORD objectType;					// 1=static, 2=embedded, 3=link

	WORD offsetFromLeftMargin;		// offset from left margin (pt / 20)
	WORD width;
	WORD height;
	WORD unused2;
	DWORD numDataBytes;				// bytes of object data
	DWORD unused3;
	DWORD objectName;
	WORD unused4;
	WORD numHeaderBytes;				// number of bytes in this header
	DWORD unused5;
	WORD widthScaledRel1000;		// scaling (/1000)*100%
	WORD heightScaledRel1000;		// scaling (/1000)*100%
};

// MSWRITE_IMPORT_LIB: this is the actual class that does all the work
// derive from this class and implement all the required methods, then call MSWRITE_IMPORT_LIB::filter()
class MSWRITE_IMPORT_LIB
{
public:
	MSWRITE_SECTION_PROPERTY *sectionProperty;

private:
	bool hasHeaderVar, hasFooterVar;
	bool isHeaderOnFirstPageVar, isFooterOnFirstPageVar;

	int numObjects;

	MSWRITE_FONT_TABLE *fontTable;
	int numFontTablePages;

	MSWRITE_PAGE_TABLE_PGD *pageTableDescriptor;
	WORD numPageTableDescriptors;
	int pageTableUpto;

	MSWRITE_FMT_INFOPAGE *charInfo;
	int numcharInfoPages;

	MSWRITE_FMT_INFOPAGE *paraInfo;
	int numparaInfoPages;

	// paragraph information upto
	int paraInfoPageUpto;
	int paraInfoFodUpto;

	// character information upto
	int charInfoPageUpto;
	int charInfoFodUpto;

	// increase paragraph and character upto
	bool paraInfoNext (void);
	bool charInfoNext (void);

	// actually load paragraph and character info
	int paraInfoGet (MSWRITE_FPROP_PAP &pap);
	int charInfoGet (MSWRITE_FPROP_CHP &chp);

	int pageTableDescriptorSignal (void);
	bool pageTableDescriptorNext (void);

	// functions involved in filtering
	int headerRead (void);
	int sectionTableRead (void);
	int sectionPropertyRead (void);
	int pageTableRead (void);
	int fontTableRead (void);
	int charInfoRead (void);
	int paraInfoRead (void);
	int documentFilter (void);		// actual "backend" filter function

	// processing objects in documentFilter()
	int getBytesPerScanLine (const int width, const int bitsPerPixel, const int padBytes);
	int processWMF (const MSWRITE_IMAGE_HEADER *image);
	int processBMP (const MSWRITE_IMAGE_HEADER *image);
	int processOLE (const MSWRITE_OLE_HEADER *ole);
protected:
	MSWRITE_HEADER header;
	WORD pnCharInfo;		// TODO: put in header class
	#define header_pnCharInfo pnCharInfo

	char *getFontName (const int fontCode)
	{
		return (char *) fontTable->ffn [fontCode].fontName;
	}

	// messages
	virtual void debug (const char *format, ...);
	virtual void warning (const char *format, ...);
	virtual void error (const char *format, ...);

	// .WRI file operations
	virtual int infileRead (void *ptr, const size_t size, const size_t memb) = 0;
	virtual int infileSeek (const long offset, const int whence) = 0;
	virtual long infileTell (void) const = 0;

	virtual int paraInfoStartWrite (const MSWRITE_FPROP_PAP &pap) = 0;
	virtual int paraInfoEndWrite (const MSWRITE_FPROP_PAP &pap) = 0;
	virtual int charInfoStartWrite (const MSWRITE_FPROP_CHP &chp) = 0;
	virtual int charInfoEndWrite (const MSWRITE_FPROP_CHP &chp) = 0;

	// output
	virtual int textWrite (const char *string) = 0;
	virtual int imageStartWrite (const int imageType, const int outputLength,
											const int widthTwips, const int heightTwips,
											const int widthScaledRel1000, const int heightScaledRel1000,
											const int horizOffsetTwips) = 0;
	virtual int imageWrite (const char *buffer, const unsigned length) = 0;
	virtual int imageEndWrite (void) = 0;

	// document hierarchy -- signals
	virtual int documentStartWrite (const int _firstPageNumber) = 0;
	virtual int documentEndWrite (void) = 0;
		virtual int footerStartWrite (void) = 0;
		virtual int footerEndWrite (void) = 0;
		virtual int headerStartWrite (void) = 0;
		virtual int headerEndWrite (void) = 0;
		virtual int bodyStartWrite (void) = 0;
		virtual int bodyEndWrite (void) = 0;

	virtual int pageNewWrite (const int pageNumberClaimed = 0) = 0;
	virtual int pageBreakWrite (void)
	{
		return textWrite ("\xc");	// i.e. (char)12
	}
	virtual int pageNumberWrite (void)
	{
		return textWrite ("\x1");	// i.e. (char)1
	}
	/*virtual int carriageReturn Write (void)
	{
		return textWrite ("\xd");	// i.e. (char)13
	}*/
	virtual int newLineWrite (const bool endOfParagraph = true)
	{
		return textWrite ("\n");	// i.e. (char)10
	}

	/**
	 * Optional/Soft Hyphen
	 * @return 0 for success or nonzero for failure
	 *
	 */
	virtual int optionalHyphenWrite (void)
	{
		return textWrite ("-");
	}

	// implement this signal to show the progress of MSWRITE_IMPORT_LIB::filter to the user
	// (0 <= value <= 100)
	virtual void sigProgress (const int)
	{
	}

public:
	// constructor & destructor
	MSWRITE_IMPORT_LIB ();
	virtual ~MSWRITE_IMPORT_LIB ();

	char *getVersion () const;

	bool hasHeader (void) const
	{
		return hasHeaderVar;
	}

	bool hasFooter (void) const
	{
		return hasFooterVar;
	}

	bool isHeaderOnFirstPage (void) const
	{
		return isHeaderOnFirstPageVar;
	}

	bool isFooterOnFirstPage (void) const
	{
		return isFooterOnFirstPageVar;
	}

	int getNumObjects (void) const
	{
		return numObjects;
	}

	int filter (void);	// "frontend" filter function
};

#endif	// MSWRITE_IMPORT_LIB_H
