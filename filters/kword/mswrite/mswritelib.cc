/* $Id$ */

/* NOTE: This is based on a discontinued version of MSWriteLib (0.3).
			So please avoid adding new features until this is updated to MSWriteLib 0.4.
			This will take a few months due to a lack of time.
*/

/* This file is part of the KDE and MSWriteView projects
   Copyright (C) 2001, 2002 Clarence Dang <dang@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License Version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "mswritelib.h"


#define RELEASE

#if !defined (RELEASE)

#define MSWRITE_DEBUG_HEADER
#define MSWRITE_DEBUG_FONT_TABLE
//#define MSWRITE_DEBUG_PAGE_TABLE
#define MSWRITE_DEBUG_SECTION_PROPERTY
//#define MSWRITE_DEBUG_SECTION_TABLE
#define MSWRITE_DEBUG_PARAINFO
#define MSWRITE_DEBUG_CHARINFO
#define MSWRITE_DEBUG_IMAGE
#define MSWRITE_DEBUG_OLE
//#define checkpoint(str) debug("checkpoint: " str "\n")
#define checkpoint(str)

#else

#define checkpoint(str)

#endif


// constructor (destructor is at bottom of source)
MSWRITE_IMPORT_LIB::MSWRITE_IMPORT_LIB ()
{
	hasHeaderVar = hasFooterVar = false;
	isHeaderOnFirstPageVar = isFooterOnFirstPageVar = false;

	// set pointers to NULL to avoid seg-fault if destructor is called unexpectedly
	sectionProperty = NULL;
	fontTable = NULL;
	pageTableDescriptor = NULL;
	charInfo = NULL;
	paraInfo = NULL;

	numObjects = 0;
}

// debugging and user ouput functions
//

#define MSWRITE_ERROR_STREAM stderr
void MSWRITE_IMPORT_LIB::debug (const char *format, ...)
{
	va_list list;

	va_start (list, format);
	vfprintf (MSWRITE_ERROR_STREAM, format, list);
	va_end (list);
}

void MSWRITE_IMPORT_LIB::warning (const char *format, ...)
{
	va_list list;

	va_start (list, format);
	fprintf (MSWRITE_ERROR_STREAM, "warning: ");
	vfprintf (MSWRITE_ERROR_STREAM, format, list);
	va_end (list);
}

void MSWRITE_IMPORT_LIB::error (const char *format, ...)
{
	va_list list;

	va_start (list, format);
	fprintf (MSWRITE_ERROR_STREAM, "error: ");
	vfprintf (MSWRITE_ERROR_STREAM, format, list);
	va_end (list);
}

// returns a string, containing core lib version info
char *MSWRITE_IMPORT_LIB::getVersion (void) const
{
	return (char *) "MSWriteImportLib (part of MSWriteView) " MSWRITE_IMPORT_LIB_VERSION_STRING "\n"
							"Build: " __TIME__ " " __DATE__ "\n"
							"Copyright (c) Clarence Dang 2001, 2002\n";
}

// front-end filter function
int MSWRITE_IMPORT_LIB::filter (void)
{

// gcc 2.95.3 seems to experience an internal compiler error so we use an inelegant workaround :(
#define I_HAVE_A_BROKEN_COMPILER

#if !defined (I_HAVE_A_BROKEN_COMPILER)
	// list of functions to call in turn only if the previous one succeeds
	// (no one can get trickier than this definition! :)
	int (MSWRITE_IMPORT_LIB:: *action []) (void) =
	{
		&MSWRITE_IMPORT_LIB::headerRead,
		&MSWRITE_IMPORT_LIB::sectionTableRead,		// doesn't need to be called!
		&MSWRITE_IMPORT_LIB::sectionPropertyRead,	// must be called before pageTableRead()
		&MSWRITE_IMPORT_LIB::pageTableRead,
		&MSWRITE_IMPORT_LIB::fontTableRead,
		&MSWRITE_IMPORT_LIB::charInfoRead,
		&MSWRITE_IMPORT_LIB::paraInfoRead,
		&MSWRITE_IMPORT_LIB::documentFilter			// the actual filter function
	};

	// go through list of functions (like this: f1() && f2() && f3() ...)
	// (got this idea from some debian cron script!)
	for (int i = 0; i < (int) (sizeof (action) / sizeof (int (MSWRITE_IMPORT_LIB:: *) (void))); i++)
	{
		// execute function
		if ((this->*action [i]) ())
		{
			// failed...
			error ("could not execute filter() function list\n");
			return 1;
		}
	}
#else
	bool fail = false;

	if (headerRead ())
		fail = true;
	else if (sectionTableRead ())
		fail = true;
	else if (sectionPropertyRead ())
		fail = true;
	else if (pageTableRead ())
		fail = true;
	else if (fontTableRead ())
		fail = true;
	else if (charInfoRead ())
		fail = true;
	else if (paraInfoRead ())
		fail = true;
	else if (documentFilter ())
		fail = true;

	if (fail)
	{
		error ("could not execute filter() function list (compiler workaround version)\n");
		return 1;
	}
#endif

	return 0;
}

// Reads the header of the mswrite file
int MSWRITE_IMPORT_LIB::headerRead (void)
{
#if defined (MSWRITE_DEBUG_HEADER)
	// check sizeof header structure i.e. sizeof (MSWRITE_HEADER)
	debug ("sizeof (header structure) = %i (should be 98)\n", sizeof (header));
#endif

	// read in header
	if (infileRead (&header, 1, sizeof (header)))
	{
		error ("header read error\n");
		return 3;
	}

	// check magic (word 0);
	switch (header.magic)
	{
	case (WORD) 0137061:
#if defined (MSWRITE_DEBUG_HEADER)
		debug ("normal write file\n");
#endif
		break;
	case (WORD) 0137062:
#if defined (MSWRITE_DEBUG_HEADER)
		debug ("normal write file with OLE objects\n");
#endif
		break;
	default:
		error ("invalid format (header.magic=%i)\n", header.magic);
		return 4;
	}

	// more checks (word 1-2)
	if (header.zero != 0 || header.magic2 != (WORD) 0125400)
	{
		error ("invalid format: header.zero != 0 || head.magic2 != 0125400\n");
		return 4;
	}

	// yet more checks (word 3-6)
	for (int i = 0; i < 4; i++)
	{
		if (header.reserved [i] != 0)
		{
			error ("invalid format: reserved != 0\n");
			return 4;
		}
	}

	// (word 7-8)
	header.numBytes -= MSWRITE_PAGE_SIZE;		// numBytes is always 128 bytes too many

#if defined (MSWRITE_DEBUG_HEADER)
 	debug ("num bytes of data (text+images+OLE): %li\n", header.numBytes);
#endif

	if (header.numBytes < 0)
	{
		error ("invalid format: header.numBytes < 0\n", header.numBytes);
		return 4;
	}

	// consistency check (word 9-14)
	for (int i = 9; i <= 14; i++)
	{
		if (((WORD *) &header) [i] < 0)
		{
			error ("header->word [%i] < 0\n", i);
			return 4;
		}
	}

#if defined (MSWRITE_DEBUG_HEADER)
	debug ("page number for start of paragraph info: %i\n", header.pnParaInfo);
#endif

#if defined (MSWRITE_DEBUG_HEADER)
	debug ("page number of footnote table: %i\n", header.pnFootnoteTable);
#endif

	// check for footnoteTable (there shouldn't be one)
	if (header.pnFootnoteTable != header.pnSectionProperty)
		warning ("document shouldn't have footnote tables (%i != %i)\n",
					header.pnFootnoteTable, header.pnSectionProperty);

#if defined (MSWRITE_DEBUG_HEADER)
	debug ("page number of section property (SEP): %i\n", header.pnSectionProperty);
#endif

	// some more checks:
	// cannot have a SEP without a SETB and vice-versa

	// no SEP?
	if (header.pnSectionProperty == header.pnPageTable)
	{
		// must have no SETB
		if (header.pnSectionTable != header.pnPageTable)
			warning ("section table without section property\n");
	}
	// have SEP?
	else
	{
		if (header.pnSectionTable != header.pnSectionProperty + 1)
			warning ("section table is not immediately after section property\n");

		if (header.pnSectionTable == header.pnPageTable)
			warning ("section property with no section table\n");
	}

#if defined (MSWRITE_DEBUG_HEADER)
	debug ("page number of section table (SETB): %i\n", header.pnSectionTable);
#endif

#if defined (MSWRITE_DEBUG_HEADER)
	debug ("page number of page table: %i\n", header.pnPageTable);
#endif

#if defined (MSWRITE_DEBUG_HEADER)
	debug ("page number of font table: %i\n", header.pnFontTable);
#endif

	// check (word 15-47)
	for (int i = 15; i <= 47; i++)
	{
		if (header.reserved2 [i - 15] != 0)
		{
			// [undocumented] check not 0
			warning ("header.reserved2 [%i] != 0 (%x)\n",
						i - 15, header.reserved2 [i - 15]);
		}
	}

	// check number of pages (word 48)
	if (header.numPages <= 0)
	{
		error ("invalid format: header.numPages < 0\n");

		// just curiosity but it shouldn't be true because of header.magic
		if (header.numPages == 0)
			error ("is msword file, not mswrite\n");

		return 4;
	}

#if defined (MSWRITE_DEBUG_HEADER)
	debug ("Number of pages in file: %i\n", header.numPages);
#endif

	// calculate starting page of character information
	header_pnCharInfo = ((header.numBytes + MSWRITE_PAGE_SIZE) + MSWRITE_PAGE_SIZE - 1) / MSWRITE_PAGE_SIZE;
	if (header_pnCharInfo < 0 || header_pnCharInfo > header.pnParaInfo)
	{
		error ("inconsist: header_pnCharInfo (%i), header.pnParaInfo (%i)\n",
					header_pnCharInfo, header.pnParaInfo);
		return 4;
	}
#if defined (MSWRITE_DEBUG_HEADER)
	debug ("page number for start of character info: %i\n", header_pnCharInfo);
#endif

	return 0;
}

// Reads Section Table
// Note: this function is actually quite useless because of the format of
//			MSWrite -- you can actually skip calling it!
//			(although it does do some checks)
int MSWRITE_IMPORT_LIB::sectionTableRead (void)
{
	int numSectionTablePages = header.pnPageTable - header.pnSectionTable;

	// no section table
	if (numSectionTablePages == 0)
	{
#if defined (MSWRITE_DEBUG_SECTION_TABLE)
		debug ("no section table page\n");
#endif
		return 0;
	}
	// too many section tables
	else if (numSectionTablePages > 1)
	{
		error ("too many section table pages (%i)\n", numSectionTablePages);
		return 1;
	}

	// allocate memory for section table
	MSWRITE_SECTION_TABLE *sectionTable = new MSWRITE_SECTION_TABLE;
	if (sectionTable == NULL)
	{
		error ("out of mem for sectionTable\n");
		return 1;
	}

	// seek to Section Table Page
	if (infileSeek (header.pnSectionTable * MSWRITE_PAGE_SIZE, SEEK_SET))
	{
		error ("sectionTable page seek error\n");
		return 1;
	}

	// read section table
	if (infileRead (sectionTable, sizeof (MSWRITE_SECTION_TABLE), 1))
	{
		error ("couldn't read sectionTable\n");
		return 1;
	}

#if defined (MSWRITE_DEBUG_SECTION_TABLE)
	for (int i = 0; i < 2; i++)
	{
		MSWRITE_SECTION_DESCRIPTOR *sed = &sectionTable->sed [i];

		debug ("sed #%i:  byte address of after section: %i  byte address of SEP: %li\n",
					i, sed->afterEndCharByte, (long) sed->SEPlocation);
	}
#endif

	if (sectionTable->numSections != 2)
	{
		warning ("sectionTable->numSections != 2s (%i) -- ignored any extra sections\n",
					sectionTable->numSections);
	}

	// check if 1st SED covers all characters
	if (sectionTable->sed [0].afterEndCharByte != header.numBytes)
	{
		warning ("1st SED doesn't cover entire document (%i != %i)\n",
					sectionTable->sed [0].afterEndCharByte, header.numBytes);
	}

	// check that 1st SED points to the correct SEP
	if (sectionTable->sed [0].SEPlocation != header.pnSectionProperty * MSWRITE_PAGE_SIZE)
	{
		warning ("1st SED doesn't point to SEP (%i != %i * 128)\n",
					sectionTable->sed [0].SEPlocation, header.pnSectionProperty);
	}

	// check that 2nd SED covers after the end of the document
	if (sectionTable->sed [1].afterEndCharByte != header.numBytes + 1)
	{
		warning ("2nd SED: doesn't cover after document (%i != %i + 1)\n",
					sectionTable->sed [1].afterEndCharByte, header.numBytes);
	}

	// check that 2nd/last SED entry is really dummy
	if (sectionTable->sed [1].SEPlocation != (DWORD) 0xFFFFFFFF)
	{
		warning ("2nd SED: byte address of sectionProperty != dummy (%i)\n",
					sectionTable->sed [1].SEPlocation);
	}

	// sectionTable (and even this function) is really quite useless so, we can delete it :)
	delete (sectionTable);

	return 0;
}

// Reads the SEP
int MSWRITE_IMPORT_LIB::sectionPropertyRead (void)
{
	// allocate memory for section property
	sectionProperty = new MSWRITE_SECTION_PROPERTY;
	if (sectionProperty == NULL)
	{
		error ("out of mem for sectionProperty\n");
		return 1;
	}

	int numSectionPropertyPages = header.pnPageTable - header.pnSectionTable;

	// no section property
	if (numSectionPropertyPages == 0)
	{
#if defined (MSWRITE_DEBUG_SECTION_PROPERTY)
		debug ("no section property page\n");
#endif
		return 0;
	}
	// too many section property pages
	else if (numSectionPropertyPages > 1)
	{
		error ("too many section property pages (%i)\n", numSectionPropertyPages);
		return 1;
	}

	// seek to SEP
	if (infileSeek (header.pnSectionProperty * MSWRITE_PAGE_SIZE, SEEK_SET))
	{
		error ("cannot seek to section property page\n");
		return 1;
	}

	// read in num bytes
	if (infileRead (&sectionProperty->numDataBytes, sizeof (BYTE), 1))
	{
		error ("cannot read sectionProperty->numDataBytes\n");
		return 1;
	}

#if defined (MSWRITE_DEBUG_SECTION_PROPERTY)
	debug ("sectionProperty->numDataBytes = %i\n", sectionProperty->numDataBytes);
#endif

	// [undocumented] numDataBytes seems to be 102
	if (sectionProperty->numDataBytes != 102)
	{
		warning ("sectionProperty->numDataBytes != 102 (%i)\n", sectionProperty->numDataBytes);
	}

	// read in sectionProperty
	if (infileRead (&sectionProperty->numDataBytes + sizeof (BYTE),
							sizeof (BYTE), sectionProperty->numDataBytes))
	{
		error ("couldn't read %i bytes of sectionProperty\n",
					sectionProperty->numDataBytes);
		return 1;
	}

#if defined (MSWRITE_DEBUG_SECTION_PROPERTY)
	debug ("sectionProperty byte dump:");
	debug ("\na:");
	for (int i = 0; i < sectionProperty->numDataBytes / 2; i++)
	{
		if (i == 8) debug ("\nb:");
		debug (" %i", (int) ((WORD *) &sectionProperty->reserved) [i]);
	}
	debug ("\n");
#endif

	return 0;
}

// Reads the Page Table
int MSWRITE_IMPORT_LIB::pageTableRead (void)
{
	numPageTableDescriptors = 0;

	// note: numPageTablePages is for internal use ONLY (i.e. just this function)
	int numPageTablePages = header.pnFontTable - header.pnPageTable;
	if (numPageTablePages == 0)
	{
#if defined (MSWRITE_DEBUG_PAGE_TABLE)
		debug ("no page table\n");
#endif
		return 0;
	}

	// seek to start of PageTable pages
	if (infileSeek (header.pnPageTable * MSWRITE_PAGE_SIZE, SEEK_SET))
	{
		error ("couldn't seek to start of page table\n");
		return 1;
	}

	// read in number of PGDs
	if (infileRead (&numPageTableDescriptors, sizeof (numPageTableDescriptors), 1))
	{
		error ("couldn't read number of PGDs\n");
		return 1;
	}

#if defined (MSWRITE_DEBUG_PAGE_TABLE)
	debug ("#PGDs = %i\n", numPageTableDescriptors);
#endif

	// some checks
	if (numPageTableDescriptors == 0)
	{
		warning ("0 PGDs\n");
	}
	else if (numPageTableDescriptors < 0)
	{
		error ("#PGDs < 0 (%i)\n", numPageTableDescriptors);
		return 1;
	}

	// read in dummy
	WORD cpgdMac;  // undefined
	if (infileRead (&cpgdMac, sizeof (WORD), 1))
	{
		error ("couldn't read in dummy, after #PGDs\n");
		return 1;
	}

	// allocate memory for PGDs
	pageTableDescriptor = new MSWRITE_PAGE_TABLE_PGD [numPageTableDescriptors];
	if (!pageTableDescriptor)
	{
		error ("couldn't alloc PGDs\n");
		return 1;
	}

	// load all the PGDs
	// [undocumented] there is no padding!
	int lastCharByte = -1;  // used for checking
	int lastPage = -1;		// ditto
	for (int i = 0; i < numPageTableDescriptors; i++)
	{
		// read in PGD
		if (infileRead (&pageTableDescriptor [i], sizeof (MSWRITE_PAGE_TABLE_PGD), 1))
		{
			error ("couldn't read PGD %i\n", i);
			return 1;
		}

#if defined (MSWRITE_DEBUG_PAGE_TABLE)
		debug ("page number: %i  byte address of first char: %i\n",
					pageTableDescriptor [i].pageNumber, pageTableDescriptor [i].firstCharByte);
#endif

		// page number < 1
		if (pageTableDescriptor [i].pageNumber < 1)
		{
			error ("pageTableDescript [%i].pageNumber (%i) out of range\n",
						i, pageTableDescriptor [i].pageNumber);
			return 1;
		}

		// there was a last page...
		if (lastPage != -1)
		{
			// page didn't follow last
			if (pageTableDescriptor [i].pageNumber != lastPage + 1)
			{
				// the pageTable can get really out of sync with reality
				// if the user doesn't repaginate after deleting a few pages
				// -- so this is just a warning, not an error
				warning ("pageTableDescript [%i].pageNumber (%i) != %i + 1\n",
							i, pageTableDescriptor [i].pageNumber, lastPage);
			}
		}

		if (pageTableDescriptor [i].firstCharByte <= lastCharByte)
		{
			error ("page table doesn't flow (%i <= %i)\n",
						pageTableDescriptor [i].firstCharByte, lastCharByte);
		}

		// update these for checking purposes, on the next iteration
		lastPage = pageTableDescriptor [i].pageNumber;
		lastCharByte = pageTableDescriptor [i].firstCharByte;

	}  // for (int i = 0; i < numPageTableDescriptors; i++)

	// check that pageTable and SectionProperty agree on the first page number
	if (pageTableDescriptor [0].pageNumber != sectionProperty->getFirstPageNumber ())
	{
		error ("pageTable says that %i is starting page, sectionProperty says %i\n",
					pageTableDescriptor [0].pageNumber, sectionProperty->getFirstPageNumber ());
		return 1;
	}

	return 0;
}

// Reads in the Font Table
// NOTE: fontTable is stored differently compared to charInfo & paraInfo
int MSWRITE_IMPORT_LIB::fontTableRead (void)
{
	// variable, holding file offset, for easy seeking
	long offset;

	numFontTablePages = header.numPages - header.pnFontTable;

	// allocate memory for fontTable
	fontTable = new MSWRITE_FONT_TABLE;
	if (fontTable == NULL)
	{
		error ("out of mem for fontTable\n");
		return 1;
	}

	// no Font Table pages?
	if (numFontTablePages == 0)
		return 0;

	// seek to start of font table pages
	if (infileSeek (offset = header.pnFontTable * MSWRITE_PAGE_SIZE, SEEK_SET))
	{
		error ("font table page seek error\n");
		return 1;
	}

	// read in number of FFNs
	if (infileRead (&fontTable->numFFNs, 1, sizeof (WORD)))
	{
		error ("font table: #FFNs read fail\n");
		return 2;
	}

#if defined (MSWRITE_DEBUG_FONT_TABLE)
	debug ("Number of FFNs: %i\n", fontTable->numFFNs);
#endif

	// allocate memory for FFNs
	fontTable->ffn = new MSWRITE_FONT_TABLE_FFN [fontTable->numFFNs];
	if (fontTable->ffn == NULL)
	{
		error ("can't alloc mem for fontTable->ffn\n");
		return 1;
	}

	// loop through FFNs and read them in
	for (int i = 0; i < fontTable->numFFNs; i++)
	{
#if defined (MSWRITE_DEBUG_FONT_TABLE)
		debug ("FFB #%i\n", i);
#endif

		MSWRITE_FONT_TABLE_FFN *ffn = &fontTable->ffn [i];

		// read in FFN's numDataBytes, fontFamily
		if (infileRead (ffn, 1, sizeof (WORD) + sizeof (BYTE)))
		{
			error ("head of ffn read fail\n");
			return 2;
		}

#if defined (MSWRITE_DEBUG_FONT_TABLE)
		debug ("\tbytes after this (ffn->numDataBytes) = %i\tfontFamily = %i\n",
					ffn->numDataBytes, ffn->fontFamily);
#endif

		// this FFN entry is on the next page?
		if (ffn->numDataBytes == (WORD) 0xFFFF)
		{
			// advance to next page
			if (infileSeek (offset += MSWRITE_PAGE_SIZE, SEEK_SET))
			{
				error ("font table: fseek new page fail\n");
				return 1;
			}

			// continue loop
			i--; continue;
		}

		// should be last entry ever
		if (ffn->numDataBytes == (WORD) 0)
		{
			if (i != fontTable->numFFNs - 1)
			{
				warning ("ffn is marked as last but is not (i(%i) != fontTable->numFFNs-1(%i))\n",
							i, fontTable->numFFNs - 1);
			}

			// end of table (few documents end with such an FFN entry but it's in the spec)
			break;
		}

		// calculate fontName string length
		int stringlen;
		stringlen = ffn->numDataBytes
						- 1	// -1 for sizeof (fontFamily)
						- 1	// stored length includes NUL
						;

		// font name can't exceed a bit less than MSWRITE_PAGE_SIZE (128) anyway...
		if (stringlen <= 0 || stringlen >= MSWRITE_PAGE_SIZE)
		{
		 	error ("string length (%i) is invalid\n", stringlen);
			return 1;
		}

		// allocate memory for string (including NUL) in font table
		ffn->fontName = new BYTE [stringlen + 1];
		if (!ffn->fontName)
		{
			error ("font name mem alloc fail\n");
			return 1;
		}

		// read in font name (including NUL)
		if (infileRead ((char *) ffn->fontName, stringlen + 1, sizeof (BYTE)))
		{
			error ("font name read fail\n");
			return 1;
		}

		if (ffn->fontName [stringlen] != '\0')
		{
			error ("font name doesn't end in NUL\n");
			return 1;
		}

#if defined (MSWRITE_DEBUG_FONT_TABLE)
		debug ("\tFont name = %s\n", ffn->fontName);
#endif
	}	// for (int i = 0; i < fontTable->numFFNs; i++)	{

	return 0;
}

// process character information pages
int MSWRITE_IMPORT_LIB::charInfoRead (void)
{
	DWORD nextChar = 0;			// used for consistency checking

	numcharInfoPages = header.pnParaInfo - header_pnCharInfo;

#if defined (MSWRITE_DEBUG_CHARINFO)
	debug ("number of pages of character information: %i\n", numcharInfoPages);
#endif

	// allocate memory for character information pages
	charInfo = new MSWRITE_FMT_INFOPAGE [numcharInfoPages];
	if (charInfo == NULL)
	{
		error ("can't allocate mem for charInfo *\n");
		return 1;
	}

	// seek to start of character information pages
	if (infileSeek (header_pnCharInfo * MSWRITE_PAGE_SIZE, SEEK_SET))
	{
		error ("char info page seek error\n");
		return 1;
	}

	for (int i = 0; i < numcharInfoPages; i++)
	{
#if defined (MSWRITE_DEBUG_CHARINFO)
		debug ("\tpage %i...\n", i);
#endif

		// read page
		if (infileRead (&charInfo [i], 1, MSWRITE_PAGE_SIZE))
		{
			error ("character page info read fail\n");
			return 1;
		}

		// set up some pointers
		charInfo [i].fod = (MSWRITE_FMT_INFOPAGE_FOD *) charInfo [i].data;
		charInfo [i].fprop = &charInfo [i].data [charInfo [i].numFODs * sizeof (MSWRITE_FMT_INFOPAGE_FOD)];

#if defined (MSWRITE_DEBUG_CHARINFO)
		debug ("\tnumber of FODs on this page: %i\n", charInfo [i].numFODs);
		debug ("\tbyte number of first character covered by this page: %li\n", charInfo [i].firstCharByte - 128);
#endif

		int firstCharByte = charInfo [i].firstCharByte - 128;

		// some checkss
		//
		if (firstCharByte < 0)
		{
			error ("charInfo [%i].firstCharByte has invalid value < 0 (%i)\n",
						i, firstCharByte);
			return 1;
		}
		else if (firstCharByte != nextChar)
		{
			warning ("charInfo [%i].firstCharByte does not flow (%i, %i)\n",
						i, firstCharByte, nextChar);
		}

		// loop through each FOD on page
		for (int j = 0; j < charInfo [i].numFODs; j++)
		{
			int afterEndCharByte = charInfo [i].fod [j].afterEndCharByte - 128;

			// more checks
			//
			if (afterEndCharByte < 0)
			{
				warning ("charInfo [%i].fod [%i].afterEndCharByte has invalid value < 0 (%i)\n",
							i, j, afterEndCharByte);
				return 1;
			}
			else if (afterEndCharByte <= nextChar)
			{
				warning ("charInfo [%i].afterEndCharByte does not flow (%i, %i)\n",
							i, afterEndCharByte, nextChar);
			}

			nextChar = afterEndCharByte;

#if defined (MSWRITE_DEBUG_CHARINFO)
			debug ("\n\tbyte number after last char covered by this FOD: %i\n\tbyte offset of corresponding FPROP: %i\n",
						charInfo [i].fod [j].afterEndCharByte - 128, charInfo [i].fod [j].FPROPoffset);
#endif

			// get offset of FPROP
			int byteoffset = charInfo [i].fod [j].FPROPoffset;
			if (byteoffset < 0)
			{
				if (byteoffset != -1)	// no FPROP?
					warning ("byteoffset of FPROP < 0 (%i)\n", byteoffset);
				continue;
			}

			MSWRITE_FPROP_CHP *chp = (MSWRITE_FPROP_CHP *) &(((char *) charInfo [i].data) [byteoffset]);

#if defined (MSWRITE_DEBUG_CHARINFO)
			debug ("\tnumber of bytes in this fprop: %i (<= %i)\n",
						chp->numDataBytes, sizeof (MSWRITE_FPROP_CHP) - sizeof (BYTE));
#endif

			// check validy of CHP
			//
			if (chp->numDataBytes < 0)
			{
				warning ("invalid chp->numDataBytes val < 0 (%i)\n", chp->numDataBytes);
			}
			else if (chp->numDataBytes < 1)
			{
				warning ("chp->numDataBytes < 1 (%i)\n", chp->numDataBytes);
			}
			else if (chp->numDataBytes > (BYTE) (sizeof (MSWRITE_FPROP_CHP) - sizeof (BYTE)))
			{
				warning ("invalid chp->numDataBytes val > %i (%i)\n",
							sizeof (MSWRITE_FPROP_CHP) - sizeof (BYTE), chp->numDataBytes);
			}

#if defined (MSWRITE_DEBUG_CHARINFO)
			debug ("\tbyte dump");
			unsigned char *data = (unsigned char *) chp->getData ();
			for (int b = 0; b < chp->numDataBytes; b++)
			{
				debug (" %i", data [b]);
			}
			debug ("\n");
#endif
		}	// for (int j = 0; j < charInfo [i].numFODs; j++)	{
	}	// for (int i = 0; i < numcharInfoPages; i++)	{

	return 0;
}

// read in paragraph information pages
int MSWRITE_IMPORT_LIB::paraInfoRead (void)
{
	DWORD nextChar = 0;		// used for consistency checking

	numparaInfoPages = header.pnFootnoteTable - header.pnParaInfo;

#if defined (MSWRITE_DEBUG_PARAINFO)
	debug ("number of pages of paragraph information: %i\n", numparaInfoPages);
#endif

	// allocate memory for paragraph information pages
	paraInfo = new MSWRITE_FMT_INFOPAGE [numparaInfoPages];
	if (paraInfo == NULL)
	{
		error ("can't alloc mem for paraInfo *\n");
		return 1;
	}

	// seek to start of paragraph information pages
	if (infileSeek (header.pnParaInfo * MSWRITE_PAGE_SIZE, SEEK_SET))
	{
		error ("para info page seek error\n");
		return 1;
	}

	for (int i = 0; i < numparaInfoPages; i++)
	{
#if defined (MSWRITE_DEBUG_PARAINFO)
		debug ("\tpage %i...\n", i);
#endif

		// read page
		if (infileRead (&paraInfo [i], 1, MSWRITE_PAGE_SIZE))
		{
			error ("paragraph page info read fail\n");
			return 1;
		}

		// set up some pointers
		paraInfo [i].fod = (MSWRITE_FMT_INFOPAGE_FOD *) paraInfo [i].data;
		paraInfo [i].fprop = &paraInfo [i].data [paraInfo [i].numFODs * sizeof (MSWRITE_FMT_INFOPAGE_FOD)];

#if defined (MSWRITE_DEBUG_PARAINFO)
	debug ("\tnumber of FODs on this page: %i\n", paraInfo [i].numFODs);
	debug ("\tbyte number of first character covered by this page: %i\n", paraInfo [i].firstCharByte - 128);
#endif

		int firstCharByte = paraInfo [i].firstCharByte - 128;

		// some checks
		//
		if (firstCharByte < 0)
		{
			warning ("paraInfo [%i].firstCharByte has invalid value < 0 (%i)\n",
						i, firstCharByte);
			return 1;
		}
		else if (firstCharByte != nextChar)
		{
			warning ("paraInfo [%i].firstCharByte does not flow (%i, %i)\n",
						i, firstCharByte, nextChar);
		}

		// loop through each FOD on page
		for (int j = 0; j < paraInfo [i].numFODs; j++)
		{
			int afterEndCharByte = paraInfo [i].fod [j].afterEndCharByte - 128;

			// more checks
			//
			if (afterEndCharByte < 0)
			{
				warning ("paraInfo [%i].fod [%i].afterEndCharByte has invalid value < 0 (%i)\n",
							i, j, afterEndCharByte);
				return 1;
			}
			else if (afterEndCharByte <= nextChar)
			{
				warning ("paraInfo [%i].afterEndCharByte does not flow (%i, %i)\n",
							i, afterEndCharByte, nextChar);
			}

			nextChar = afterEndCharByte;

#if defined (MSWRITE_DEBUG_PARAINFO)
			debug ("\n\tbyte number after last char covered by this FOD: %i\n\tbyte offset of corresponding FPROP: %i\n",
						paraInfo [i].fod [j].afterEndCharByte - 128, paraInfo [i].fod [j].FPROPoffset);
#endif

			// get offset of FPROP
			int byteoffset = paraInfo [i].fod [j].FPROPoffset;
			if (byteoffset < 0)
			{
				if (byteoffset != -1)	// no FPROP?
					warning ("byteoffset of FPROP < 0 (%i)\n", byteoffset);
				continue;
			}

			MSWRITE_FPROP_PAP *pap = (MSWRITE_FPROP_PAP *) &(((char *) paraInfo [i].data) [byteoffset]);

#if defined (MSWRITE_DEBUG_PARAINFO)
			debug ("\tnumber of bytes in this fprop: %i (<= %i)\n",
						pap->numDataBytes, sizeof (MSWRITE_FPROP_PAP) - sizeof (BYTE));
#endif

			// check validity of PAP
			//
			if (pap->numDataBytes < 0)
			{
				warning ("invalid pap->numDataBytes val < 0 (%i)\n", pap->numDataBytes);
			}
			else if (pap->numDataBytes < 1)
			{
				warning ("pap->numDataBytes < 1 (%i)\n", pap->numDataBytes);
			}
			else if (pap->numDataBytes > (BYTE) (sizeof (MSWRITE_FPROP_PAP) - sizeof (BYTE)))
			{
				warning ("invalid pap->numDataBytes val > %i (%i)\n",
							sizeof (MSWRITE_FPROP_PAP) - sizeof (BYTE), pap->numDataBytes);
			}

			// type byte actually exists
			if (pap->numDataBytes >= 16 + 1)
			{
				// is there a header?
				if (pap->isHeader ())
				{
					this->hasHeaderVar = true;

					// is it on the first page?
					if (pap->isOnFirstPage ())
						this->isHeaderOnFirstPageVar = true;
				}

				// is there a footer?
				if (pap->isFooter ())
				{
					this->hasFooterVar = true;

					// it it on the first page?
					if (pap->isOnFirstPage ())
						this->isFooterOnFirstPageVar = true;
				}

				if (pap->isObject ())
					numObjects++;
			}

#if defined (MSWRITE_DEBUG_PARAINFO)
	if (pap->numDataBytes >= 16 + 1)	// byte actually exists
			if (pap->type & (1 << 4) /* fgraphics */)
				debug ("\tThis is a picture!!!\n");

			debug ("\tbyte dump");
			unsigned char *data = (unsigned char *) pap->getData ();
			for (int b = 0; b < pap->numDataBytes; b++)
			{
				if (b == 22)
					debug (" tbd:");
				debug (" %i", data [b]);
			}
			debug ("\n");
#endif
		}
	}

#if defined (MSWRITE_DEBUG_PARAINFO)
	debug ("Document:  Header: %i (1st page: %i)  Footer: %i (1st page: %i)\n",
				hasHeader (), isHeaderOnFirstPage (),
				hasFooter (), isFooterOnFirstPage ());
#endif

	return 0;
}

// increments paraInfo upto variables
// returns whether this next PAP is valid
bool MSWRITE_IMPORT_LIB::paraInfoNext (void)
{
	// move on to next paragraph FOD (TODO: handle empty FODs)
	paraInfoFodUpto++;

	// no more FODs on page?
	if (paraInfoFodUpto >= paraInfo [paraInfoPageUpto].numFODs)
	{
		// move on to next page
		paraInfoPageUpto++;
		paraInfoFodUpto = 0;

		// no more pages?
		if (paraInfoPageUpto >= numparaInfoPages)
			return false;
	}

	// PAP valid
	return true;
}

// increments charInfo upto variables
// returns whether this next CHP is valid
bool MSWRITE_IMPORT_LIB::charInfoNext (void)
{
	// move on to next character FOD (TODO: handle empty FODs)
	charInfoFodUpto++;	// NOTE: this is correct for first FOD since we set it to -1

	// no more FODs on page?
	if (charInfoFodUpto >= charInfo [charInfoPageUpto].numFODs)
	{
		// move on to next page
		charInfoPageUpto++;
		charInfoFodUpto = 0;

		// no more pages?
		if (charInfoPageUpto >= numcharInfoPages)
			return false;
	}

	// CHP valid
	return true;
}

// signal entry in the pageTable
int MSWRITE_IMPORT_LIB::pageTableDescriptorSignal (void)
{
	return pageNewWrite (pageTableDescriptor [pageTableUpto].pageNumber);
}

// advance pageTable upto variable
// returns whether this next PGD is valid
bool MSWRITE_IMPORT_LIB::pageTableDescriptorNext (void)
{
	// advance pageTable
	pageTableUpto++;

	// upto past end?
	if (pageTableUpto >= numPageTableDescriptors)
		return false;		// PGD invalid
	else
		return true;		// PGD valid
}

// write out a full PAP (based on defaults, overridden with small PAP stored in .wri file)
int MSWRITE_IMPORT_LIB::paraInfoGet (MSWRITE_FPROP_PAP &pap)
{
	// get pointer to stored, compressed PAP
	MSWRITE_FPROP_PAP *compressed_pap = (MSWRITE_FPROP_PAP *) NULL;

	// well, first of all, there better be an FPROP containing para info
	int byteOffset = paraInfo [paraInfoPageUpto].fod [paraInfoFodUpto].FPROPoffset;
	//debug ("got FPROP byteOffset = %i\n", byteOffset);
	if (byteOffset >= 0)
	{
		// get pointer to compressed PAP
		compressed_pap = (MSWRITE_FPROP_PAP *) &(((char *) paraInfo [paraInfoPageUpto].data) [byteOffset]);
	}

	// now copy compressed PAP onto normal PAP, so that we get default variables where the compressed PAP doesn't specify a value
	pap.setDefaults ();
	if (compressed_pap)
	{
//		debug ("copying %X to %X (amount=%i)\n", compressed_pap->getData (), pap.getData (), compressed_pap->numDataBytes);
		memcpy (pap.getData (), (BYTE *) compressed_pap->getData (), compressed_pap->numDataBytes);
//		debug ("I got here!\n");
	}

	return 0;
}

// write out a full CHP (based on defaults, overridden with small CHP stored in .wri file)
int MSWRITE_IMPORT_LIB::charInfoGet (MSWRITE_FPROP_CHP &chp)
{
	// get pointer to stored, compressed CHP
	MSWRITE_FPROP_CHP *compressed_chp = (MSWRITE_FPROP_CHP *) NULL;

	// well, first of all, there better be an FPROP containing char info
	int byteOffset = charInfo [charInfoPageUpto].fod [charInfoFodUpto].FPROPoffset;
	//printf ("byteOffset = %i\n", byteOffset);
	if (byteOffset >= 0)
	{
		// get pointer to compressed CHP
		compressed_chp = (MSWRITE_FPROP_CHP *) &(((char *) charInfo [charInfoPageUpto].data) [byteOffset]);
	}

	// now copy compressed CHP onto normal CHP, so that we get default variables where the compressed CHP doesn't specify a value
	chp.setDefaults ();
	if (compressed_chp)
	{
		//debug ("copying %X to %X (amount=%i)\n", compressed_chp->getData (), chp.getData (), compressed_chp->numDataBytes);
		memcpy (chp.getData (), (BYTE *) compressed_chp->getData (), compressed_chp->numDataBytes);
		//debug ("I got here!\n");
	}

	return 0;
}

// filters document (back-end functon)
int MSWRITE_IMPORT_LIB::documentFilter (void)
{
	// indicate start of document
	if (documentStartWrite (sectionProperty->getFirstPageNumber ()))
	{
		error ("documentStartWrite() failed\n");
		return 1;
	}


	// variables
	//

	// variables for what part of the document we're in
	enum {MSWRITE_IN_NOTHING, MSWRITE_IN_BODY, MSWRITE_IN_HEADER, MSWRITE_IN_FOOTER};
	int inWhat = MSWRITE_IN_NOTHING;

	// we need this variable so that if the document doesn't have a body,
	// we still know to call bodyStartWrite() and bodyEndWrite()
	bool bodyCreated = false;

	// pageTable uptos
	pageTableUpto = 0;
	bool pageTableExist = numPageTableDescriptors ? true : false;	// in case bool really only is 1 bit... :)

	// paragraph info stuff
	DWORD paraStartByte = 0;

	// character info stuff
	MSWRITE_FPROP_CHP chp;
	bool chpExist = true;	// "= true" to get rid of compiler warnings
	int chpLoadByte = 0, chpEndByte = -1;

	// reset info page counters
	paraInfoPageUpto	= 0;
	paraInfoFodUpto	= 0;
	charInfoPageUpto	= 0;
	charInfoFodUpto	= -1;


	// init
	//

	// seek to start of text
	if (infileSeek (1 * MSWRITE_PAGE_SIZE, SEEK_SET))
	{
		error ("text page seek error\n");
		return 1;
	}

	// allocate memory for buffer
	unsigned char *buffer;
	buffer = (unsigned char *) new char [MSWRITE_BUFFER_SIZE + 1];	// + 1 for nul
	if (buffer == NULL)
	{
		error ("can't allocate mem for buffer\n");
		return 1;
	}


	// let's filter
	//

	debug ("<!-- TEXT STARTS -->\n");
	sigProgress (0);

	// loop through each paragraph
	for (;;)
	{
		checkpoint ("start");

		// read in paragraph information
		MSWRITE_FPROP_PAP pap;
		if (paraInfoGet (pap))
		{
			error ("cannot get pap\n");
			return 1;
		}

		// paragraph starts after end of file?
		if (paraStartByte >= header.numBytes)
		{
			warning ("paragraph starts after EOF (%i >= %i)\n",
						paraStartByte, header.numBytes);
			break;
		}

		checkpoint ("paraGot");

		int inWhatNext;
		if (pap.isParagraph ())
			inWhatNext = MSWRITE_IN_BODY;
		else if (pap.isHeader ())
			inWhatNext = MSWRITE_IN_HEADER;
		else if (pap.isFooter ())
			inWhatNext = MSWRITE_IN_FOOTER;
		else
		{
			error ("Not normal paragraph, header or footer\n");
			return 1;
		}

		if (inWhatNext != inWhat)
		{
			// end last thing that we were in
			switch (inWhat)
			{
			case MSWRITE_IN_BODY:		bodyEndWrite ();		break;
			case MSWRITE_IN_HEADER:		headerEndWrite ();	break;
			case MSWRITE_IN_FOOTER:		footerEndWrite ();	break;
			}

			// start new thing
			inWhat = inWhatNext;
			switch (inWhat)
			{
			case MSWRITE_IN_BODY:
				bodyStartWrite ();

				// if the pageTable doesn't exist, we still have to generate
				// pageNewWrite(firstPageNumber) signal
				// if it does exist, then the first page is handled later
				if (!pageTableExist)
					pageNewWrite ();

				bodyCreated = true;

				break;
			case MSWRITE_IN_HEADER:		headerStartWrite ();		break;
			case MSWRITE_IN_FOOTER:		footerStartWrite ();		break;
			}
		}

		checkpoint ("inWhat done");

		// output paragraph info
		if (paraInfoStartWrite (pap))
		{
			error ("cannot output start of paragraph info\n");
			return 1;
		}

		int paraLoadByte = paraInfo [paraInfoPageUpto].fod [paraInfoFodUpto].afterEndCharByte - 128;
		int paraEndByte = paraLoadByte - 1;

		// paragraph ends after end of file?
		if (paraEndByte >= header.numBytes)
		{
			warning ("paragraph ends after EOF, limiting paraEndByte (%i >= %i)\n",
						paraEndByte, header.numBytes);
			paraEndByte = header.numBytes - 1;
		}

		checkpoint ("para mess done");

		if (pap.isText ())
		{
			int uptoByte = paraStartByte;

			// loop until we hit end of paragraph (getting one CHP on every iteration)
			while (uptoByte <= paraEndByte)
			{
				checkpoint ("want chp?");

				// need to get NEW chp?
				if (uptoByte == chpLoadByte)
				{
					checkpoint ("will get chp");
					chpExist = charInfoNext ();

					if (chpExist)
					{
						charInfoGet (chp);

						chpLoadByte = charInfo [charInfoPageUpto].fod [charInfoFodUpto].afterEndCharByte - 128;
						chpEndByte = chpLoadByte - 1;
					}

					checkpoint ("got chp");
				}

				if (chpExist)
					charInfoStartWrite (chp);

				// ultimately aim for end of CHP block; if that's not possible, aim for end of paragraph
				int aimUltimateByte = chpEndByte > paraEndByte ? paraEndByte : chpEndByte;

				checkpoint ("enter uptoByte <= aimUltimateByte loop");

				while (uptoByte <= aimUltimateByte)
				{
					// flag to tell us to generate pageNewWrite/pageTable signal, after writing some normal text
					bool pageTableAck = false;

					// short-term goal (before end of CHP, PAP or page)
					int aimByte = aimUltimateByte;

					// we want to efficiently send pageNewWrite signals which might be sandwhiched inside
					// a CHP block (note: we do NOT end and then restart a CHP block because of a pageNewWrite())
					if (pageTableExist)
					{
						checkpoint ("pageTableExist");

						// note: the -1 is because we want the newpage signal called before the character
						if (pageTableDescriptor [pageTableUpto].firstCharByte <= aimByte)
						{
							checkpoint ("pageTableAck");

							pageTableAck = true;
							aimByte = pageTableDescriptor [pageTableUpto].firstCharByte - 1;

							checkpoint ("just before debug");

#if defined (MSWRITE_DEBUG_PAGE_TABLE)
							debug ("pageTable[%i] (%i) forces aimByte=%i, instead of %i\n",
									pageTableUpto, pageTableDescriptor [pageTableUpto].firstCharByte - 1,
									aimByte, aimUltimateByte);
#endif
						}
					}

					checkpoint ("read text");

					// write text (using buffering)
					while (uptoByte <= aimByte)
					{
						const int numBytesToGet = aimByte - uptoByte + 1 > MSWRITE_BUFFER_SIZE
														? MSWRITE_BUFFER_SIZE : aimByte - uptoByte + 1;

						// read buffer
						if (infileRead (buffer, 1, numBytesToGet))
						{
							error ("text read fail\n");
							delete [] buffer;	// TODO: destruct at other points
							return 1;
						}

						// null terminate (I did leave space for this!)
						buffer [numBytesToGet] = '\0';

						// prune characters:
						//		1	(pageNumber),
						//		10	(newLine),
						//		13	(ignored),
						//		12	(pageBreak),
						//		31 (optionalHyphen)
						// (using a tricky method to recycle the buffer in only one pass!)
						// and generate signals

						unsigned char *inBufferStart = buffer;
						unsigned char *inBuffer = buffer;
						unsigned char *outBuffer = buffer;
						unsigned char *outBufferStart = buffer;

						checkpoint ("prune");

						for (; *inBuffer; inBuffer++)
						{
							// we need this because our buffer recycling tricks could overwrite it first...
							unsigned char c = *inBuffer;

							switch (c)
							{
							// write text, generate signals for special characters, write more text...
							case 1:		// pageNumber anchor
							case 12:		// pageBreak (some silly document might have this in the middle of a paragraph!)
							case 10:		// newLine (some silly document _does_ have a newline in the middle of a paragraph!)
							case 31:    // optionalHyphen (undocumented -- is an invisible hyphen, unless at end of line)
								// output text before this character
								*outBuffer = '\0';
								if (outBufferStart != outBuffer)
									textWrite ((const char *) outBufferStart);
								//debug ("\n****INSIDE WRITE: \"%s\" *****\n", outBufferStart);

								// start next string here (recycling buffer, remember? :)
								outBufferStart = outBuffer;

								// generate signal
								switch (c)
								{
								case 1:	pageNumberWrite ();	break;
								case 12:	pageBreakWrite ();	break;
								case 10:	newLineWrite ((uptoByte + (inBuffer - inBufferStart)) == paraEndByte);		break;
								case 31: optionalHyphenWrite (); break;
								}

								break;

							// carriage return: ignore
							case 13:
								break;

							// normal text character
							default:
								*outBuffer = c; outBuffer++;
								break;
							}	// switch (*inBuffer)	{
						}	// for (; *inBuffer; inBuffer++)	{

						// null terminate (yes, I did allocate space for this!)
						*outBuffer = '\0';
						if (outBufferStart != outBuffer)
							textWrite ((const char *) outBufferStart);	// output buffer in one large block
						//debug ("\n****OUTSIDE WRITE: \"%s\" *****\n", outBufferStart);

						uptoByte += numBytesToGet;
					}		// while (uptoByte <= aimByte) {

					checkpoint ("out of prune loop");

#if 1
					// consistency check
					if (uptoByte != aimByte + 1)
					{
						error ("internal consistency error: uptoByte != aimByte + 1 (%i != %i)\n",
									uptoByte, aimByte + 1);
						return 1;
					}
#endif

					// generate pageNewWrite/pageTable signal, if requested
					if (pageTableAck)
					{
						if (pageTableDescriptorSignal ())
						{
							error ("couldn't signal new PGD\n");
							return 1;
						}

						pageTableExist = pageTableDescriptorNext ();
					}
				}		// while (uptoByte <= aimUltimateByte) {

				// end char info
				if (chpExist)
					charInfoEndWrite (chp);

			}		// while (uptoByte <= paraEndByte) {
		}	// if (pap.isText ())	{

		else if (pap.isObject ())
		{
			// save the location, for an easy seek past OLE (until we implement OLE)
			int location = infileTell ();
			if (location == -1)
			{
				error ("cannot tell location (image)\n");
				return 1;
			}

			union tagOBJECT
			{
				MSWRITE_IMAGE_HEADER image;
				MSWRITE_OLE_HEADER ole;
			} object;

			assert (sizeof (object) == 40);

			// read in object header
			if (infileRead (&object, sizeof (tagOBJECT), 1))
			{
				error ("couldn't read in object header\n");
				return 1;
			}

			// [temporary] error message such as "[cannot import OLE]"
			char *message = (char *) "";

			// OLE (temporary code)
			if (object.ole.mappingMode == 0xe4)
			{
				debug ("OLE detected\n");

				if (pageTableExist)
				{
					// if there's a relevant PGD
					if (pageTableDescriptor [pageTableUpto].firstCharByte <= paraEndByte)
					{
						// if it's OLE, then you can't have a page starting in the middle of an object
						// TODO: can you?
						if (pageTableDescriptor [pageTableUpto].firstCharByte != paraStartByte)
						{
							warning ("PGD [%i].firstCharByte (%i) != paraStartByte (%i)\n",
										pageTableUpto, pageTableDescriptor [pageTableUpto].firstCharByte, paraStartByte);
						}

						// generate signal
						if (pageTableDescriptorSignal ())
						{
							error ("couldn't signal new PGD\n");
							return 1;
						}

						// advance PGD
						pageTableExist = pageTableDescriptorNext ();
					}
				}

				message = (char *) "[cannot import OLE]";

				MSWRITE_OLE_HEADER *ole = &object.ole;
				if (processOLE (ole))
				{
					error ("processOLE() failed\n");
					return 1;
				}
			}

			// image
			else
			{
				MSWRITE_IMAGE_HEADER *image = &object.image;

#if defined (MSWRITE_DEBUG_IMAGE)
				debug ("METAFILEPICT:\n");
				debug ("\tmapping mode: %i\n", image->mappingMode);
				debug ("\twidth: %i\n", image->MFPwidth);
				debug ("\theight: %i\n", image->MFPheight);
				debug ("\tignored: %i\n", image->MFPignored);

				debug ("\nREST:\n");
				debug ("offsetFromLeftMargin: %i  width: %i  height: %i\n",
							image->offsetFromLeftMargin, image->width, image->height);
				debug ("cbOldSize: %i\n", image->cbOldSize);
				debug ("bitmap info (byte):");
				for (int i = 0; i < 14; i++)
				{
					debug (" %i (%c)", image->bm [i], image->bm [i] >= 32 ? image->bm [i] : '?');
				}
				debug ("\n");
				debug ("bitmap info (word):");
				WORD *info = (WORD *) image->bm;
				for (int i = 0; i < (int) (14 / sizeof (WORD)); i++)
				{
					debug (" %i (%c)", info [i], info [i] >= 32 ? info [i] : '?');
				}
				debug ("\n");

				debug ("numHeaderBytes: %i  numDataBytes: %i  widthScaledRel1000: %i  heightScaledRel1000: %i\n",
							image->numHeaderBytes, image->numDataBytes, image->widthScaledRel1000, image->heightScaledRel1000);
#endif
				int expectedSize = paraEndByte - paraStartByte + 1 - image->numHeaderBytes;

#if defined (MSWRITE_DEBUG_IMAGE)
				debug ("expected size of data: %i\n", paraEndByte - paraStartByte + 1 - image->numHeaderBytes);
#endif

				if (image->numHeaderBytes != sizeof (MSWRITE_IMAGE_HEADER))
				{
					error ("image->numHeaderBytes (%i) != sizeof (MSWRITE_IMAGE_HEADER) (%i)\n",
								image->numHeaderBytes, sizeof (MSWRITE_IMAGE_HEADER));
					return 1;
				}

				if (image->numDataBytes != image->cbOldSize)
					warning ("image->numDataBytes (%i) != image->cbOldSize (%i)\n",
								image->numDataBytes, image->cbOldSize);

				if (image->numDataBytes != paraEndByte - paraStartByte + 1 - image->numHeaderBytes)
					warning ("image->numDataBytes (%i) doesn't = expected (%i)\n", image->numDataBytes, expectedSize);

				// BMP?
				if (image->mappingMode == 0xe3)
				{
					if (processBMP (image))
					{
						error ("processBMP() failed\n");
						return 1;
					}
				}
				// WMF
				else
				{
					if (processWMF (image))
					{
						error ("processWMF() failed\n");
						return 1;
					}
				}	// if WMF
			}

			int uptoByte = paraStartByte;

			// "eat" CHPs that have gone past (didn't used to do this but...)
			//debug ("uptoByte: %i  paraEndByte: %i\n", uptoByte, paraEndByte);
			bool firstTime = true;
			while (uptoByte <= paraEndByte)
			{
				checkpoint ("eat CHP");

				if(chpExist)
					chpExist = charInfoNext ();

				if (chpExist)
				{
					checkpoint ("before get chp");
					charInfoGet (chp);
					checkpoint ("after get chp");

					chpLoadByte = charInfo [charInfoPageUpto].fod [charInfoFodUpto].afterEndCharByte - 128;
					chpEndByte = chpLoadByte - 1;

					//debug ("uptoByte: %i  chpLoadByte: %i  paraLoadByte: %i\n",
					//			uptoByte, chpLoadByte, paraLoadByte);

					if (chpLoadByte > paraLoadByte)
						uptoByte = paraLoadByte;
					else
						uptoByte = chpLoadByte;

					checkpoint ("before charinfo write");

					// this hack here is until OLE is figured out
					charInfoStartWrite (chp);

					checkpoint ("after charinfo write");

					// write "unable to import <whatever>" using the first available CHP
					if (firstTime)
					{
						//debug ("writing message: \"%s\"\n", message);
						textWrite (message);
						newLineWrite ();

						firstTime = false;
					}

					checkpoint ("before charinfoend write");
					charInfoEndWrite (chp);
					checkpoint ("after charinfoend write");

					checkpoint ("end of chpExist");
				}
				else
				{
					if (strlen (message))
					{
						textWrite (message);
						newLineWrite ();
					}
					break;
				}
			}

			checkpoint ("ate chp");

			// [temporary]
			if (infileSeek (location + paraLoadByte - paraStartByte, SEEK_SET))
			{
				error ("seek past image failed\n");
				return 1;
			}
			debug ("seeked past object (%i + %i)\n", location, paraLoadByte - paraStartByte);
		}

		// not text or object?
		else	// if (pap.isText ())	{
		{
			error ("unknown paragraph type\n");
			return 1;
		}

		// end paragraph
		paraInfoEndWrite (pap);
		paraStartByte = paraLoadByte;

		checkpoint ("paraEndWrite");

		// inform user of progess (being careful of div by 0)
		sigProgress ((header.numBytes) ? (paraStartByte * 100 / header.numBytes) : (100));

		// no more paragraphs?
		if (!paraInfoNext ())
			break;	// finish filtering!

	}		// for (;;) {

	// end last thing that we were in
	switch (inWhat)
	{
	case MSWRITE_IN_BODY:		bodyEndWrite ();		break;
	case MSWRITE_IN_HEADER:		headerEndWrite ();	break;
	case MSWRITE_IN_FOOTER:		footerEndWrite ();	break;
	}

	// if a document body wasn't created, then make one (usually done in the case of a blank document)
	if (!bodyCreated)
	{
		bodyStartWrite ();
		pageNewWrite ();
		bodyEndWrite ();
	}

	sigProgress (100);
	debug ("<!-- TEXT ENDS -->\n");

	delete [] buffer;

	// indicate end of document
	if (documentEndWrite ())
	{
		error ("documentEndWrite () failed\n");
		return 1;
	}

	return 0;
}

int MSWRITE_IMPORT_LIB::processOLE (const MSWRITE_OLE_HEADER *ole)
{
	warning ("OLE not implemented (yet)\n");

#if defined (MSWRITE_DEBUG_OLE)
	debug ("objectType: ");
#endif
	switch (ole->objectType)
	{
	case 1:
#if defined (MSWRITE_DEBUG_OLE)
		debug ("static\n");
#endif
		break;
	case 2:
#if defined (MSWRITE_DEBUG_OLE)
		debug ("embedded\n");
#endif
		break;
	case 3:
#if defined (MSWRITE_DEBUG_OLE)
		debug ("link\n");
#endif
		break;
	default:
		error ("unknown objectType (%i)\n", ole->objectType);
		return 1;
	}

#if defined (MSWRITE_DEBUG_OLE)
	debug ("offsetFromLeftMargin: %i\n", ole->offsetFromLeftMargin);
	debug ("width (%i) x height (%i)\n", ole->width, ole->height);
	debug ("numDataBytes: %i\n", ole->numDataBytes);
	debug ("objectName: %X (%i)\n", ole->objectName, ole->objectName);
#endif

#if defined (MSWRITE_DEBUG_OLE)
	debug ("numHeaderBytes: %i\n", ole->numHeaderBytes);
#endif
	if (ole->numHeaderBytes != sizeof (MSWRITE_OLE_HEADER))
	{
		error ("ole->numHeaderBytes (%i) != sizeof (MSWRITE_OLE_HEADER) (%i)\n",
					ole->numHeaderBytes, sizeof (MSWRITE_OLE_HEADER));
		return 1;
	}

#if defined (MSWRITE_DEBUG_OLE)
	debug ("widthScaledRe11000: %i    heightScaledRel1000: %i\n",
				ole->widthScaledRel1000, ole->heightScaledRel1000);
#endif

#if 1
#if defined (MSWRITE_DEBUG_OLE)
	// dump OLE to a file
#endif
#endif

	return 0;
}

int MSWRITE_IMPORT_LIB::processWMF (const MSWRITE_IMAGE_HEADER *image)
{
	debug ("WMF detected\n");

	// allocate memory for image
	char *imageBuffer = new char [image->numDataBytes];
	if (!imageBuffer)
	{
		error ("out of memory for image (%i bytes)\n", image->numDataBytes);
		return 1;
	}

	// read in image
	if (infileRead (imageBuffer, image->numDataBytes, 1))
	{
		error ("can't read image\n");
		return 1;
	}

	// check if contents of buffer are indeed that of a "Standard Metafile" (NOT a "Clipboard Metafile")
	class MSWRITE_METAFILE_HEADER
	{
	public:
		WORD fileType;
		WORD headerSize;
		WORD winVersion;
		DWORD fileSize;
		WORD numObjects;
		DWORD maxRecordSize;
		WORD reserved;
	};
	MSWRITE_METAFILE_HEADER *wmf = (MSWRITE_METAFILE_HEADER *) imageBuffer;

	if (wmf->fileType != 1)
		warning ("wmf->fileType != 1 (%i)\n", wmf->fileType);
	if (wmf->headerSize != 9)
		warning ("wmf->headerSize != 9 (%i)\n", wmf->headerSize);
	if (wmf->winVersion > 0x0300)
		warning ("wmf->winVersion > 0x0300 (%i)\n", wmf->winVersion);
	if ((int) (wmf->fileSize * sizeof (WORD)) != image->numDataBytes)
		warning ("wmf->fileSize (%i) != image->numDataBytes (%i)\n",
					(int) (wmf->fileSize * sizeof (WORD)), image->numDataBytes);
	if (wmf->reserved != 0)
		warning ("wmf->reserved != 0 (%i)\n", wmf->reserved);

	if (imageStartWrite (MSWRITE_OBJECT_WMF, image->numDataBytes,
								image->width, image->height,
								image->widthScaledRel1000, image->heightScaledRel1000,
								image->offsetFromLeftMargin))
	{
		error ("imageStartWrite() failed\n");
		return 1;
	}

	if (imageWrite (imageBuffer, image->numDataBytes))
	{
		error ("imageWrite() failed\n");
		return 1;
	}

	if (imageEndWrite ())
	{
		error ("imageEndWrite () failed\n");
		return 1;
	}

	delete [] imageBuffer;

	debug ("WMF exported\n");
	return 0;
}

// internal function that returns how many bytes are needed for each scanline (with pixel & byte padding)
int MSWRITE_IMPORT_LIB::getBytesPerScanLine (const int width, const int bitsPerPixel, const int padBytes)
{
	int bitWidth;
	int byteWidth;

	// figure out how many bytes it would take to represent "width" pixels, each taking "bitsPerPixel" bits
	bitWidth = width * bitsPerPixel;
	byteWidth = bitWidth / 8;
	if (bitWidth % 8)
		byteWidth++;

	// pad the number of bytes taken to the next multiple of "padBytes"
	// (if not divisible by "padBytes")
	byteWidth = (byteWidth + padBytes - 1) / padBytes * padBytes;

/*	debug ("getBytePerScanLine: width=%i, bitsPerPixel=%i, padBytes=%i\n",
			width, bitsPerPixel, padBytes);
	debug ("process: bitWidth=%i, byteWidth(result)=%i\n",
			bitWidth, byteWidth);*/
	return byteWidth;
}

// TODO: find out how to process BMPs properly!
// (currently using undocumented algorithm)
int MSWRITE_IMPORT_LIB::processBMP (const MSWRITE_IMAGE_HEADER *image)
{
	debug ("BMP detected\n");
	warning ("BMP import is EXPERIMENTAL but seems stable!\n");

	// allocate memory for image
	char *imageBuffer = new char [image->numDataBytes];
	if (!imageBuffer)
	{
		error ("out of memory for image (%i bytes)\n", image->numDataBytes);
		return 1;
	}

	// read in image
	if (infileRead (imageBuffer, image->numDataBytes, 1))
	{
		error ("can't read image\n");
		return 1;
	}

	// with bitmaps, METAFILEPICT isn't relevant and width/height
	// are usually 0 so, we have to use the special 14-byte bitmap header
	// given to us :)
	debug ("width: %i  height: %i\n", image->bmh.width, image->bmh.height);

	int colourTableSize = (1 << image->bmh.planes) * sizeof (MSWRITE_BITMAP_COLOUR_INDEX);

	// figure out file header
	MSWRITE_BITMAP_FILE_HEADER fileHeader;
	fileHeader.magic = (WORD) 'B' + (((WORD) 'M') << 8);
	fileHeader.totalBytes = sizeof (MSWRITE_BITMAP_FILE_HEADER)
									+ sizeof (MSWRITE_BITMAP_INFO_HEADER)
									+ colourTableSize
									+ image->bmh.height
										* getBytesPerScanLine (image->bmh.width, image->bmh.bitsPerPixel, 4);
	fileHeader.reserved [0] = fileHeader.reserved [1] = 0;
	fileHeader.actualImageOffset = sizeof (MSWRITE_BITMAP_FILE_HEADER)
												+ sizeof (MSWRITE_BITMAP_INFO_HEADER)
												+ colourTableSize;

	// figure out BMP info header
	MSWRITE_BITMAP_INFO_HEADER infoHeader;
	infoHeader.size = sizeof (MSWRITE_BITMAP_INFO_HEADER);
	infoHeader.width = image->bmh.width;
	infoHeader.height = image->bmh.height;
	if (image->bmh.planes != 1)
	{
		error ("image->bmh.planes != 1 (%i)\n", image->bmh.planes);
		return 1;
	}
	infoHeader.planes = image->bmh.planes;
	infoHeader.bitCount = image->bmh.bitsPerPixel;
	infoHeader.compression = 0;									// i.e. BI_RGB (uncompressed)
	infoHeader.sizeImage = 0;										// don't need to specify size in bytes
	infoHeader.xPixelsPerMetre = infoHeader.yPixelsPerMetre = 0;
	infoHeader.coloursUsed = 1 << infoHeader.bitCount;		// used later -- don't set to 0
	infoHeader.coloursImportant = infoHeader.coloursUsed;

	// create colour table
	MSWRITE_BITMAP_COLOUR_INDEX *colourIndex = new MSWRITE_BITMAP_COLOUR_INDEX [infoHeader.coloursUsed];
	if (!colourIndex)
	{
		error ("couldn't allocate memory for %i colourIndex's\n", infoHeader.coloursUsed);
		return 1;
	}

	if (infoHeader.coloursUsed != 2)
	{
		// TODO: does Write only support monochrome bitmaps anyway?
		warning ("colour bitmaps not supported (yet)\n");
	}

	// black and white...
	colourIndex [0].red = 0, colourIndex [0].green = 0, colourIndex [0].blue = 0, colourIndex [0].reserved = 0;
	colourIndex [1].red = 0xff, colourIndex [1].green = 0xff, colourIndex [1].blue = 0xff, colourIndex [1].reserved = 0;

	// write out each scanline
	// (padded to 4 bytes vs input bitmap which is actually padded to 2)
	int scanLineInLength = getBytesPerScanLine (infoHeader.width, infoHeader.bitCount, 2);
	int scanLineOutLength = getBytesPerScanLine (infoHeader.width, infoHeader.bitCount, 4);

	debug ("scanLineInLength: %i\n", scanLineInLength);
	debug ("scanLineOutLength: %i\n", scanLineOutLength);

	// output image
	//

	if (imageStartWrite (MSWRITE_OBJECT_BMP, fileHeader.totalBytes,
								image->bmh.width * 20, image->bmh.height * 20,		// width/height converted to twips
								image->widthScaledRel1000, image->heightScaledRel1000,
								image->offsetFromLeftMargin))
	{
		error ("imageStartWrite() failed\n");
		return 1;
	}

	if (imageWrite ((const char *) &fileHeader, sizeof (fileHeader)))
	{
		error ("couldn't write fileHeader\n");
		return 1;
	}
	if (imageWrite ((const char *) &infoHeader, sizeof (infoHeader)))
	{
		error ("couldn't write BMP infoHeader\n");
		return 1;
	}

	if (imageWrite ((const char *) colourIndex, infoHeader.coloursUsed * sizeof (MSWRITE_BITMAP_COLOUR_INDEX)))
	{
		error ("couldn't write colourTable (colourIndex[])\n");
		return 1;
	}

	char *padding = new char [scanLineOutLength - scanLineInLength];
	if (!padding)
	{
		error ("can't allocate memory for padding [%i]\n",
					scanLineOutLength - scanLineInLength);
		return 1;
	}
	memset (padding, 0, scanLineOutLength - scanLineInLength);

	// either the buffer or the DIB is upside-down...
	for (int i = infoHeader.height - 1; i >= 0; i--)
	{
		// write bitmap scanline
		if (imageWrite (imageBuffer + i * scanLineInLength, scanLineInLength * sizeof (char)))
		{
			error ("can't write scanLine [%i]\n", i);
			return 1;
		}

		// write padding for scanline
		if (imageWrite (padding, (scanLineOutLength - scanLineInLength) * sizeof (char)))
		{
			error ("can't write padding (len: %i)\n", scanLineOutLength - scanLineInLength);
			return 1;
		}
	}

	if (imageEndWrite ())
	{
		error ("imageEndWrite () failed\n");
		return 1;
	}

	checkpoint ("before dealloc\n");
	delete [] padding;
	delete [] colourIndex;
	delete [] imageBuffer;
	checkpoint ("after dealloc\n");

	debug ("BMP exported\n");
	return 0;
}

// destructor (constructor is at top of source)
MSWRITE_IMPORT_LIB::~MSWRITE_IMPORT_LIB (void)
{
	delete sectionProperty;
	delete [] pageTableDescriptor;
	delete [] paraInfo;
	delete [] charInfo;
	delete fontTable;
}
