/* This file is part of the KDE project
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

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <qregexp.h>
#include <qtextstream.h>
#include <kdebug.h>

#include <assert.h>
#include <stdio.h>		// much better than iostream :)
#include <stdarg.h>

#include <mswriteimport.h>
#include <mswriteimport.moc>

// kdDebug type functions
//

void MSWRITE_PROJECT::debug (const char *format, ...)
{
	va_list list;

	va_start (list, format);
	vsnprintf (debugStr, MSWRITE_BUFFER_SIZE, format, list);
	kdDebug (MSWRITE_DEBUG_AREA) << debugStr;
	va_end (list);
}

void MSWRITE_PROJECT::warning (const char *format, ...)
{
	va_list list;

	va_start (list, format);
	vsnprintf (debugStr, MSWRITE_BUFFER_SIZE, format, list);
	kdWarning (MSWRITE_DEBUG_AREA) << debugStr;
	va_end (list);
}

void MSWRITE_PROJECT::error (const char *format, ...)
{
	va_list list;

	va_start (list, format);
	vsnprintf (debugStr, MSWRITE_BUFFER_SIZE, format, list);
	kdError (MSWRITE_DEBUG_AREA) << debugStr;
	va_end (list);
}

// file operations
//

int MSWRITE_PROJECT::openFiles (const char *_infilename, const char *_outfilename)
{
	// open input file
	strcpy (infilename, _infilename);
	infile = fopen (infilename, "rb");
	if (!infile)
	{
		error ("input file open error\n");
		return 1;
	}

	// opens the output store
	strcpy (outfilename, _outfilename);
	outfile = new KoStore (QString (outfilename), KoStore::Write);

	return 0;
}

void MSWRITE_PROJECT::closeFiles (void)
{
	if (outfile)
	{
		delete (outfile);
		outfile = (KoStore *) NULL;
	}

	if (infile)
	{
		fclose (infile);
		infile = (FILE *) NULL;
	}
}

int MSWRITE_PROJECT::infileRead (void *ptr, size_t size, size_t memb)
{
	int cread;
	if ((cread = fread (ptr, size, memb, infile)) != (int) memb)
	{
		error ("could not read (%u < %u)\n", cread, memb);
		return 1;
	}
	return 0;
}

int MSWRITE_PROJECT::infileSeek (long offset, int whence)
{
	return (fseek (infile, offset, whence) == -1) ? 1 : 0;
}

long MSWRITE_PROJECT::infileTell (void) const
{
	return ftell (infile);
}

// get some document stats
int MSWRITE_PROJECT::documentGetStats (void)
{
	// page width & height
	pageWidth = sectionProperty->getPageWidth ();
	pageHeight = sectionProperty->getPageHeight ();

	// offset of margins
	left = sectionProperty->getLeftMargin ();
	right = left + sectionProperty->getTextWidth ();
	top = sectionProperty->getTopMargin ();
	bottom = top + sectionProperty->getTextHeight ();

	// size of margins
	leftMargin = left;
	rightMargin = sectionProperty->getRightMargin ();
	topMargin = top;
	bottomMargin = sectionProperty->getBottomMargin ();

	debug ("leftMargin: %i  rightMargin: %i  topMargin: %i  bottomMargin: %i\n",
				leftMargin, rightMargin, topMargin, bottomMargin);

	// offset of header & footer
	headerFromTop = sectionProperty->getHeaderFromTop ();
	footerFromTop = sectionProperty->getFooterFromTop ();

	// adjust margins/PAPERBORDERS to ensure that the header & footer are within them
	// TODO: stop header & footer from changing body's location
	if (hasHeader ())
		if (headerFromTop < topMargin) topMargin = headerFromTop;
	if (hasFooter ())
		if (pageHeight - footerFromTop < bottomMargin) bottomMargin = pageHeight - footerFromTop;

	debug ("adjusted::: leftMargin: %i  rightMargin: %i  topMargin: %i  bottomMargin: %i\n",
				leftMargin, rightMargin, topMargin, bottomMargin);

	return 0;
}

int MSWRITE_PROJECT::documentStartWrite (const int _firstPageNumber)
{
	// TODO: implement _firstPageNumber

	// get dimensions of paper, borders, margins...
	if (documentGetStats ())
	{
		error ("can't documentGetStats()\n");
		return 1;
	}

	// allocate memory for array of object data
	objectData = new MSWRITE_OBJECT_DATA [getNumObjects ()];
	if (!objectData)
	{
		error ("cannot allocate memory for objectData [%i]\n", getNumObjects ());
		return 1;
	}

	// open maindoc.xml
	if (!outfile->open ("root"))
	{
		error ("Cannot open root in store\n");
		return 1;
	}

	// start document
	// TODO: error checking
	tagWrite ("<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DOC >");
	tagWrite ("<DOC editor=\"KWord\" mime=\"application/x-kword\">");

	tagWrite ("<PAPER format=\"1\" width=\"%i\" height=\"%i\" orientation=\"0\" columns=\"1\" "
					"hType=\"%i\" fType=\"%i\">",
					pageWidth, pageHeight,
					isHeaderOnFirstPage () ? 0 : 2,
					isFooterOnFirstPage () ? 0 : 2);

	tagWrite ("<PAPERBORDERS left=\"%i\" right=\"%i\" top=\"%i\" bottom=\"%i\"/>",
					leftMargin, rightMargin, topMargin, bottomMargin);

	tagWrite ("</PAPER>");

	tagWrite ("<ATTRIBUTES processing=\"0\" standardpage=\"1\" unit=\"mm\" hasHeader=\"%i\" hasFooter=\"%i\"/>",
					hasHeader () ? 1 : 0, hasFooter () ? 1 : 0);

	tagWrite ("<FRAMESETS>");

	return 0;
}

int MSWRITE_PROJECT::documentEndWrite (void)
{
	// write framesets for the objects
	tagWrite (objectFrameset);

	tagWrite ("</FRAMESETS>");
	tagWrite ("<STYLES>");
	tagWrite ("<STYLE>");
		tagWrite ("<NAME value=\"Standard\"/>");
		tagWrite ("<FLOW align=\"left\"/>");
		tagWrite ("<FOLLOWING name=\"Standard\"/>");
		tagWrite ("<FORMAT id=\"1\">");
			tagWrite ("<WEIGHT value=\"50\"/>");
			tagWrite ("<COLOR blue=\"0\" red=\"0\" green=\"0\"/>");
			tagWrite ("<FONT name=\"helvetica\"/>");
			tagWrite ("<SIZE value=\"11\"/>");
			tagWrite ("<ITALIC value=\"0\"/>");
			tagWrite ("<UNDERLINE value=\"0\"/>");
			tagWrite ("<STRIKEOUT value=\"0\"/>");
			tagWrite ("<VERTALIGN value=\"0\"/>");
		tagWrite ("</FORMAT>");
	tagWrite ("</STYLE>");
	tagWrite ("</STYLES>");

	// write out image keys
	tagWrite ("<PIXMAPS>"); tagWrite (pixmaps); tagWrite ("</PIXMAPS>");
	tagWrite ("<CLIPARTS>"); tagWrite (cliparts); tagWrite ("</CLIPARTS>");

	// end document
	tagWrite ("</DOC>");

	// close maindoc.xml
	outfile->close ();

	//
	// output object data

	if (objectUpto != getNumObjects ())
		warning ("objectUpto (%i) != getNumObjects() (%i) -- this is probably because OLE is unimplemented\n",
					objectUpto, getNumObjects ());

	for (int i = 0; i < objectUpto; i++)
	{
		debug ("outputting: objectData [%i] (\"%s/%s\")   (length: %i)\n",
					i,
					(const char *) objectPrefix.utf8 (), (const char *) objectData [i].nameInStore.utf8 (),
					objectData [i].dataLength);

		// open file for object in store
		if (!outfile->open (objectPrefix + (QString) "/" + objectData [i].nameInStore))
		{
			error ("can't open image in store (%s)\n", (const char *) objectData [i].nameInStore.utf8 ());
			return 1;
		}

		if (outfile->write (objectData [i].data, objectData [i].dataLength) == false)
		{
			error ("cannot write objectData [%i].data to store (len: %i)\n",
						i, objectData [i].dataLength);
			return 1;
		}

		// close object in store
		outfile->close ();
	}

	delete [] objectData;
	objectData = (MSWRITE_OBJECT_DATA *) NULL;

	return 0;
}

// pageNewWrite() is called due to entries in the pageTable
// -- however, pageTable can be very inaccurate, so we ignore it
int MSWRITE_PROJECT::pageNewWrite (const int)
{
	return 0;
}

// handles explicit page breaks
int MSWRITE_PROJECT::pageBreakWrite (void)
{
	pageBreak = true;		// later used in paraEndWrite()
	return 0;
}

// handle "(page)" number
int MSWRITE_PROJECT::pageNumberWrite (void)
{
	return tagWrite ("#");
}

// ignore newlines, because paraEndWrite() is implemented anyway
int MSWRITE_PROJECT::newLineWrite (void)
{
	return 0;
}

int MSWRITE_PROJECT::bodyStartWrite (void)
{
	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"0\" name=\"Text Frameset 1\" visible=\"1\">");
	tagWrite ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehaviour=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
					top, bottom, left, right);

	return 0;
}

int MSWRITE_PROJECT::bodyEndWrite (void)
{
	// <PAGEBREAKING hardFrameBreakAfter=\"true\"/>" may have been in the last paragraph
	// and for "hardFrameBreakAfter" to do its work, we need one more final paragraph!
	if (needAnotherParagraph)
	{
		debug ("needAnotherParagraph==true in bodyEndWrite()\n");
		tagWrite ("<PARAGRAPH><TEXT></TEXT><LAYOUT></LAYOUT></PARAGRAPH>");
		needAnotherParagraph = false;
	}

	tagWrite ("</FRAMESET>\n");

	// since "Text Frameset 1" has ended, we can output the header & footer, now
	delayOutputFlush ();

	return 0;
}

int MSWRITE_PROJECT::headerStartWrite (void)
{
	// headers must go after body in KWord
	delayOutput (true);

	// dummy header frames
	//

	// except, if the header is NOT on the first page, then make an empty "First Page Header"
	// by setting "visible=1"
	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"1\" name=\"First Page Header\" visible=\"%i\">",
						(!isHeaderOnFirstPage ()) ? 1 : 0);
	tagWrite ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehaviour=\"2\" autoCreateNewFrame=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
					headerFromTop, headerFromTop, left, right);
	tagWrite ("</FRAMESET>");

	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"2\" name=\"Odd Pages Header\" visible=\"0\">");
	tagWrite ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehaviour=\"2\" autoCreateNewFrame=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
						headerFromTop, headerFromTop, left, right);
	tagWrite ("</FRAMESET>");

	// real header frame
	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"3\" name=\"Even Pages Header\" visible=\"1\">");
	tagWrite ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehaviour=\"2\" autoCreateNewFrame=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
						headerFromTop, headerFromTop, left, right);
	return 0;
}

int MSWRITE_PROJECT::headerEndWrite (void)
{
	tagWrite ("</FRAMESET>\n");
	delayOutput (false);

	return 0;
}

int MSWRITE_PROJECT::footerStartWrite (void)
{
	// footers must go after body in KWord
	delayOutput (true);

	// dummy footer frames
	//

	// except, if the footer is NOT on the first page, then make an empty "First Page Footer"
	// by setting "visible=1"
	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"4\" name=\"First Page Footer\" visible=\"%i\">",
					(!isFooterOnFirstPage ()) ? 1 : 0);
	tagWrite ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehaviour=\"2\" autoCreateNewFrame=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
					footerFromTop, footerFromTop, left, right);
	tagWrite ("</FRAMESET>");

	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"5\" name=\"Odd Pages Footer\" visible=\"0\">");
	tagWrite ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehaviour=\"2\" autoCreateNewFrame=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
					footerFromTop, footerFromTop, left, right);
	tagWrite ("</FRAMESET>");

	// real footer frame
	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"6\" name=\"Even Pages Footer\" visible=\"1\">");
	tagWrite ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehaviour=\"2\" autoCreateNewFrame=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
					footerFromTop, footerFromTop, left, right);
	return 0;
}

int MSWRITE_PROJECT::footerEndWrite (void)
{
	tagWrite ("</FRAMESET>\n");
	delayOutput (false);

	return 0;
}

int MSWRITE_PROJECT::paraInfoStartWrite (const MSWRITE_FPROP_PAP &)
{
	if (tagWrite ("<PARAGRAPH><TEXT>"))
	{
		error ("tagWrite (\"<p\") error\n");
		return 1;
	}

	// reset charInfo counters
	charInfoCountStart = 0;
	charInfoCountLen = 0;

	return 0;
}

int MSWRITE_PROJECT::paraInfoEndWrite (const MSWRITE_FPROP_PAP &pap)
{
	QString output;
	output += "</TEXT>";

	output += "<LAYOUT>";
		output += "<NAME value=\"Standard\"/>";

		int justify = pap.getJustification ();

		if (justify != MSWRITE_JUSTIFY_LEFT)
		{
			output += "<FLOW align=\"";
			switch (justify)
			{
				/*case MSWRITE_JUSTIFY_LEFT:
					output += "left";
					break;*/
				case MSWRITE_JUSTIFY_CENTRE:
					output += "center";
					break;
				case MSWRITE_JUSTIFY_RIGHT:
					output += "right";
					break;
				case MSWRITE_JUSTIFY_BOTH:
					output += "justify";
					break;
			}
			output += "\"/>";
		}

		int indentFirst = pap.getLeftIndentFirstLine ();
		int indentLeft = pap.getLeftIndent ();
		int indentRight = pap.getRightIndent ();

		// hopefully these test operations will be cheaper than the XML ones :)
		if (indentFirst || indentLeft || indentRight)
		{
			output += "<INDENTS";
			if (indentFirst) output += " first=\"" + QString::number (indentFirst) + "\"";
			if (indentLeft) output += " left=\"" + QString::number (indentLeft) + "\"";
			if (indentRight) output += " right=\"" + QString::number (indentRight) + "\"";
			output += "/>";
		}

		/*debug ("indent:  first: %i  left: %i  right: %i\n",
					pap.getLeftIndentFirstLine (),
					pap.getLeftIndent (),
					pap.getRightIndent ());*/

		int lineSpacing = pap.getLineSpacing ();

		if (lineSpacing != 10)
		{
			output += "<LINESPACING value=\"";
			switch (lineSpacing)
			{
				//case 10:
				//	output += "0";
				//	break;
				case 15:
					output += "oneandhalf";
					break;
				case 20:
					output += "double";
					break;
				default:
					warning ("unknown linespacing value: %i\n", lineSpacing);
					output += "0";
					break;
			}
			output += "\" />";
		}

		// emulate Write's linespacing (aligned to bottom)
		// by using varying amounts of space before the paragraph
		// TODO: test if it works nicely enough (what if you have several different sized fonts in paragraph?)
		if (lineSpacing != 10)		// if not normal linespacing...
		{
			output += "<OFFSETS before=\"";

			int amount = 0;
			switch (lineSpacing)
			{
				/*case 10:
					break;*/
				case 15:
					amount = 7;
					break;
				case 20:
					amount = 14;
					break;
				default:
					// already warned above
					//warning ("unknown linespacing value: %i\n", lineSpacing);
					break;
			}

			// subtract the amount of trailing linespace from last paragraph
			amount -= lineSpacingFromAbove;
			if (amount <= 0) amount = 0;		// no emulation can be perfect...

			output += QString::number (amount);
			output += "\" />";
		}

		// GUESS (TODO: fix) the amount of trailing linespace
		switch (lineSpacing)
		{
			case 10:
				lineSpacingFromAbove = 0;
				break;
			case 15:
				lineSpacingFromAbove = 7;
				break;
			case 20:
				lineSpacingFromAbove = 14;
				break;
			default:		// unknown
				lineSpacingFromAbove = 0;
				break;
		}

		// TODO:	check if pagebreaking behaviour is similar to mswrite
		if (pageBreak)
		{
			output += "<PAGEBREAKING hardFrameBreakAfter=\"true\"/>";
			pageBreak = false;				// reset flag
			needAnotherParagraph = true;	// need another paragraph for hardFrameBreakAfter to work
		}
		else
			needAnotherParagraph = false;
		//else
		//	output += "<PAGEBREAKING/>";
		//debug ("pageBreak: %i\n", pageBreak);


		/*output += "<LEFTBORDER width=\"0\" style=\"0\" />";
		output += "<RIGHTBORDER width=\"0\" style=\"0\" />";
		output += "<TOPBORDER width=\"0\" style=\"0\" />";
		output += "<BOTTOMBORDER width=\"0\" style=\"0\" />";
		output += "<COUNTER/>";*/

		// Tabulators
		for (int i = 0; i < 14; i++)
		{
			if (!pap.tbd [i].isTabEntry ()) break;

			output += "<TABULATOR";

			output += " type=\"";
			if (pap.tbd [i].isTabDecimal ())
				output += "3";
			else
				output += "0";
			output += "\"";

			output += " ptpos=\"" + QString::number (pap.tbd [i].getTabNumPoints ()) + "\"/>";

			debug ("Tab: isNormal: %i  ptPos: %i\n", pap.tbd [i].isTabNormal (),
																	pap.tbd [i].getTabNumPoints ());
		}

	output += "</LAYOUT>";

	output += "<FORMATS>";
		output += formatOutput; formatOutput = "";		// output all the charInfo for this paragraph
	output += "</FORMATS>";

	output += "</PARAGRAPH>";

	if (tagWrite (output)) return 1;
	//output = "";		// [cheap optimisation!] local output, so don't have to null out

	return 0;
}

int MSWRITE_PROJECT::charInfoStartWrite (const MSWRITE_FPROP_CHP &)
{
	// note: this function is not needed because
	//       paraInfoStartWrite (), paraInfoEndWrite () and charInfoEndWrite () does it all!
	return 0;
}

// outputs character formatting tags
int MSWRITE_PROJECT::charInfoEndWrite (const MSWRITE_FPROP_CHP &chp)
{
	// output type of format information (page number or normal text)
	formatOutput += "<FORMAT id=\"";
	if (chp.isPageNumber ()) formatOutput += "4"; else formatOutput += "1";
	formatOutput += "\" ";

	formatOutput += "pos=\""; formatOutput += QString::number (charInfoCountStart); formatOutput += "\" ";
	formatOutput += "len=\""; formatOutput += QString::number (charInfoCountLen); formatOutput += "\">";

	charInfoCountStart += charInfoCountLen;
	charInfoCountLen = 0;

	if (chp.isPageNumber ())
	{
		formatOutput += "<TYPE type=\"4\"/>";
		formatOutput += "<PGNUM subtype=\"0\" value=\"1\"/>";
	}

	int fontCode = chp.getFontCode ();
	if (fontCode >= 0)
	{
		formatOutput += "<FONT name=\"";
			formatOutput += getFontName (fontCode);
			formatOutput += "\"/>";;
			formatOutput += "<SIZE value=\"";
			formatOutput += QString::number (chp.getFontPointSize ());
			formatOutput += "\"/>";
	}
	else
	{
		// TODO: can we depend on the style instead?
		debug ("no font\n");
		formatOutput += "<FONT name=\"helvetica\"/>";
		formatOutput += "<SIZE value=\"12\"/>";
	}

	if (chp.isBold ())
	  	formatOutput += "<WEIGHT value=\"75\"/>";
//else
//  		formatOutput += "<WEIGHT value=\"50\" />";

	if (chp.isItalic ())
		formatOutput += "<ITALIC value=\"1\"/>";
//	else
//		formatOutput += "<ITALIC value=\"0\" />";

	if (chp.isUnderlined ())
		formatOutput += "<UNDERLINE value=\"1\"/>";
//	else
//		formatOutput += "<UNDERLINE value=\"0\" />";

	/*if (chp.isNormalPosition ())
		formatOutput += "<VERTALIGN value=\"0\" />";
	else*/ if (chp.isSubscript ())
		formatOutput += "<VERTALIGN value=\"1\"/>";
	else if (chp.isSuperscript ())
		formatOutput += "<VERTALIGN value=\"2\"/>";
	/*else
		error ("unknown valign\n");*/

/*	formatOutput += "<STRIKEOUT value=\"0\" />";
	formatOutput += "<CHARSET value=\"0\" />";*/

	formatOutput += "</FORMAT>";

	return 0;
}

// delayed output functions
// (for Headers and Footers which must come after the Body in KWord)

void MSWRITE_PROJECT::delayOutput (const bool yes)
{
	delayOutputVar = yes;
}

int MSWRITE_PROJECT::delayOutputFlush (void)
{
	return (outfile->write ((const char *) heldOutput.utf8 (), heldOutput.length ()) == false);
}

// text output functions
//

int MSWRITE_PROJECT::textWrite_lowLevel (const char *str)
{
	if (delayOutputVar)
	{
		heldOutput += str;
		return 0;
	}
	else
	{
		//debug ("%s", str);
		return (outfile->write (str, strlen (str)) == false);
	}
}

int MSWRITE_PROJECT::textWrite (const char *inStr)
{
	int ret;

	char outStr [(MSWRITE_BUFFER_SIZE + 3) * 6];

	charInfoCountLen += strlen (inStr);

	// convert from ASCII to XML strings
	int outUpto = 0;
	int inUpto = 0;
	for (; inUpto < (int) strlen (inStr); inUpto++)
	{
		switch (inStr [inUpto])
		{
			// handle unfriendly XML entities
			case '<':
				strcpy (outStr + outUpto, "&lt;");
				outUpto += 4;
				break;
			case '>':
				strcpy (outStr + outUpto, "&gt;");
				outUpto += 4;
				break;
			case '&':
				strcpy (outStr + outUpto, "&amp;");
				outUpto += 5;
				break;
			case '\'':
				strcpy (outStr + outUpto, "&apos;");
				outUpto += 6;
				break;
			case '\"':
				strcpy (outStr + outUpto, "&quot;");
				outUpto += 6;
				break;

			// print character
			default:
				outStr [outUpto++] = inStr [inUpto];
				break;
		}

	}
	outStr [outUpto++] = '\0';
	ret = textWrite_lowLevel (outStr);

	return ret;
}

// same as textWrite (+va_arg) but without XML translation
int MSWRITE_PROJECT::tagWrite (const char *format, ...)
{
	int ret;

	char string [(MSWRITE_BUFFER_SIZE + 3)];

	va_list list;
	va_start (list, format);

	vsprintf (string, format, list);

	ret = textWrite_lowLevel (string);

	va_end (list);
	return ret;
}

int MSWRITE_PROJECT::tagWrite (const int num)
{
	return tagWrite ("%i", num);
}

int MSWRITE_PROJECT::tagWrite (const QString &str)
{
	return (textWrite_lowLevel ((const char *) str.utf8 ()));
}

int MSWRITE_PROJECT::imageStartWrite (const int imageType, const int outputLength,
													const int widthTwips, const int heightTwips,
													const int widthScaledRel1000, const int heightScaledRel1000,
													const int horizOffsetTwips)
{
	QString imageName;
	QString fileInStore;

	textWrite ("#");

	formatOutput += "<FORMAT id=\"6\" pos=\"0\" len=\"1\">";
	formatOutput += "<ANCHOR type=\"frameset\" instance=\"";
	if (imageType == MSWRITE_OBJECT_BMP)
	{
		imageName = "Picture ";
		imageName += QString::number (numPixmap);

		formatOutput += imageName;

		fileInStore += "pictures/picture" + QString::number (numPixmap) + ".bmp";
	}
	else if (imageType == MSWRITE_OBJECT_WMF)
	{
		imageName += "Clipart ";
		imageName += QString::number (numClipart);

		formatOutput += imageName;

		fileInStore += "cliparts/clipart" + QString::number (numClipart) + ".wmf";
	}
	else
	{
		error ("unsupported picture type %i\n", imageType);
		return 1;
	}
	formatOutput += "\"/>";
	formatOutput += "</FORMAT>";

	if (imageType == MSWRITE_OBJECT_BMP)
	{
		objectFrameset += "<FRAMESET frameType=\"2\" frameInfo=\"0\" name=\"";
		objectFrameset += imageName;
		objectFrameset += "\" visible=\"1\">";

		objectFrameset += "<FRAME runaround=\"1\" copy=\"0\" newFrameBehaviour=\"1\"";
		objectFrameset += " left=\"";
			objectFrameset += QString::number (left + horizOffsetTwips / 20);
			objectFrameset += "\"";
		objectFrameset += " right=\"";
			objectFrameset += QString::number (left + (horizOffsetTwips + widthTwips * widthScaledRel1000 / 1000) / 20);
			objectFrameset += "\"";
		objectFrameset += " top=\"";
			objectFrameset += QString::number (top);
			objectFrameset += "\"";
		objectFrameset += " bottom=\"";
			objectFrameset += QString::number (top + (heightTwips * heightScaledRel1000 / 1000) / 20);
			objectFrameset += "\"/>";

		objectFrameset += "<IMAGE keepAspectRatio=\"false\">";
		// TODO: proper dates
		objectFrameset += "<KEY msec=\"0\" hour=\"5\" second=\"5\" minute=\"5\" day=\"5\" month=\"5\" year=\"2001\"";
		objectFrameset += " filename=\"";
		objectFrameset += fileInStore;
		objectFrameset += "\"/>";
		objectFrameset += "</IMAGE>";

		objectFrameset += "</FRAMESET>";

		pixmaps += "<KEY msec=\"0\" hour=\"5\" second=\"5\" minute=\"5\" day=\"5\" month=\"5\" year=\"2001\"";
		pixmaps += " name=\"";
		pixmaps += fileInStore;
		pixmaps += "\"";
		pixmaps += " filename=\"";
		// TODO: proper filename
		pixmaps += fileInStore;
		pixmaps += "\"/>";

		numPixmap++;
	}
	else if (imageType == MSWRITE_OBJECT_WMF)
	{
		objectFrameset += "<FRAMESET frameType=\"5\" frameInfo=\"0\" name=\"";
		objectFrameset += imageName;
		objectFrameset += "\" visible=\"1\">";

		objectFrameset += "<FRAME runaround=\"1\" copy=\"0\" newFrameBehaviour=\"1\"";
		objectFrameset += " left=\"";
			objectFrameset += QString::number (left + horizOffsetTwips / 20);
			objectFrameset += "\"";
		objectFrameset += " right=\"";
			objectFrameset += QString::number (left + (horizOffsetTwips + widthTwips * widthScaledRel1000 / 1000) / 20);
			objectFrameset += "\"";
		objectFrameset += " top=\"";
			objectFrameset += QString::number (top);
			objectFrameset += "\"";
		objectFrameset += " bottom=\"";
			objectFrameset += QString::number (top + (heightTwips * heightScaledRel1000 / 1000) / 20);
			objectFrameset += "\"/>";

		objectFrameset += "<CLIPART>";
		objectFrameset += "<KEY msec=\"0\" hour=\"5\" second=\"5\" minute=\"5\" day=\"5\" month=\"5\" year=\"2001\"";
		objectFrameset += " filename=\"";
		objectFrameset += fileInStore;
		objectFrameset += "\"/>";
		objectFrameset += "</CLIPART>";

		objectFrameset += "</FRAMESET>";

		cliparts += "<KEY msec=\"0\" hour=\"5\" second=\"5\" minute=\"5\" day=\"5\" month=\"5\" year=\"2001\"";
		cliparts += " name=\"";
		cliparts += fileInStore;
		cliparts += "\"";
		cliparts += " filename=\"";
		cliparts += fileInStore;
		cliparts += "\"/>";

		numClipart++;
	}

	objectData [objectUpto].nameInStore = fileInStore;
	objectData [objectUpto].dataLength = outputLength;
	objectData [objectUpto].data = new char [outputLength];
	if (!objectData [objectUpto].data)
	{
		error ("cannot allocate memory for objectData [%i].data (len: %i)\n",
					objectUpto, outputLength);
		return 1;
	}

	return 0;
}

int MSWRITE_PROJECT::imageWrite (const char *buffer, const unsigned length)
{
	// consistency check: aren't going to write too much data, past end of array?
	if (objectData [objectUpto].dataUpto + (int) length > objectData [objectUpto].dataLength)
	{
		error ("objectData [%i].dataUpto (%i) + length (%i) > objectData [%i].dataLength (%i)\n",
					objectUpto, objectData [objectUpto].dataUpto, length,
					objectUpto, objectData [objectUpto].dataLength);
		return 1;
	}

	memcpy (objectData  [objectUpto].data + objectData [objectUpto].dataUpto, buffer, length);
	objectData [objectUpto].dataUpto += length;

	return 0;
}

int MSWRITE_PROJECT::imageEndWrite (void)
{
	// consistency check: wrote exactly the right amount of data?
	if (objectData [objectUpto].dataUpto != objectData [objectUpto].dataLength)
		warning ("objectData [%i].dataUpto (%i) != objectData [%i].dataLength (%i)\n",
					objectUpto, objectData [objectUpto].dataUpto,
					objectUpto, objectData [objectUpto].dataLength);

	objectUpto++;

	return 0;
}

// constructor
MSWRITE_PROJECT::MSWRITE_PROJECT (KoFilter *parent, const char *name)
						: KoFilter(parent, name)
{
	pageBreak = false;
	needAnotherParagraph = false;

	lineSpacingFromAbove = 0;

	delayOutput (false);

	numPixmap = 0;
	numClipart = 0;
	objectUpto = 0;

	infile = (FILE *) NULL;
	outfile = (KoStore *) NULL;
}

// destructor
MSWRITE_PROJECT::~MSWRITE_PROJECT ()
{
	closeFiles ();
}

// front-end filter
bool MSWRITE_PROJECT::filter (const QString &fileIn, const QString &fileOut,
										const QString &prefixOut,
										const QString &from, const QString &to,
										const QString &)
{
	if (to != "application/x-kword" || from != "application/x-mswrite")
		return false;

	objectPrefix = prefixOut;
	debug ("prefixOut: \"%s\"\n", (const char *) prefixOut.utf8 ());

	if (openFiles (fileIn.utf8 (), fileOut.utf8 ()))
	{
		error ("could not open files\n");
		return false;
	}

	// output version info of core lib
	// (so when bug reports come in, we know what lib it was using)
	debug (MSWRITE_IMPORT_LIB::getVersion ());

	if (MSWRITE_IMPORT_LIB::filter ())
	{
		error ("could not filter document\n");
		return false;
	}

	return true;
}
