/* $Id$ */

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

#include <qfile.h>
#include <qregexp.h>
#include <qtextstream.h>

#include <kgenericfactory.h>
#include <kdebug.h>

#include <koFilterChain.h>

#include <assert.h>
#include <stdio.h>		// much better than iostream :)
#include <stdarg.h>

#include <mswriteimport.h>


typedef KGenericFactory <MSWRITEImport, KoFilter> MSWRITEImportFactory;
K_EXPORT_COMPONENT_FACTORY (libmswriteimport, MSWRITEImportFactory ("mswriteimport"));


// kdDebug type functions
//

void MSWRITEImport::debug (const char *format, ...)
{
	va_list list;

	va_start (list, format);
	vsnprintf (m_debugStr, MSWRITE_BUFFER_SIZE, format, list);
	kdDebug (MSWRITE_DEBUG_AREA) << m_debugStr;
	va_end (list);
}

void MSWRITEImport::warning (const char *format, ...)
{
	va_list list;

	va_start (list, format);
	vsnprintf (m_debugStr, MSWRITE_BUFFER_SIZE, format, list);
	kdWarning (MSWRITE_DEBUG_AREA) << m_debugStr;
	va_end (list);
}

void MSWRITEImport::error (const char *format, ...)
{
	va_list list;

	va_start (list, format);
	vsnprintf (m_debugStr, MSWRITE_BUFFER_SIZE, format, list);
	kdError (MSWRITE_DEBUG_AREA) << m_debugStr;
	va_end (list);
}

// file operations
//

int MSWRITEImport::openFiles (const char *infilename, const char *outfilename)
{
	// open input file
	strcpy (m_infilename, infilename);
	m_infile = fopen (m_infilename, "rb");
	if (!m_infile)
	{
		error ("input file open error\n");
		return 1;
	}

	// opens the output store
	strcpy (m_outfilename, outfilename);
	m_outfile = KoStore::createStore (QFile::encodeName (m_outfilename), KoStore::Write, "application/x-kword");

	return 0;
}

void MSWRITEImport::closeFiles (void)
{
	if (m_outfile)
	{
		delete (m_outfile);
		m_outfile = (KoStore *) NULL;
	}

	if (m_infile)
	{
		fclose (m_infile);
		m_infile = (FILE *) NULL;
	}
}

int MSWRITEImport::infileRead (void *ptr, size_t size, size_t memb)
{
	int cread;
	if ((cread = fread (ptr, size, memb, m_infile)) != (int) memb)
	{
		error ("could not read (%u < %u)\n", cread, memb);
		return 1;
	}
	return 0;
}

int MSWRITEImport::infileSeek (long offset, int whence)
{
	return (fseek (m_infile, offset, whence) == -1) ? 1 : 0;
}

long MSWRITEImport::infileTell (void) const
{
	return ftell (m_infile);
}

// get some document stats
int MSWRITEImport::documentGetStats (void)
{
	// page width & height
	m_pageWidth = sectionProperty->getPageWidth ();
	m_pageHeight = sectionProperty->getPageHeight ();

	// offset of margins
	m_left = sectionProperty->getLeftMargin ();
	m_right = m_left + sectionProperty->getTextWidth () - 1;
	m_top = sectionProperty->getTopMargin ();
	m_bottom = m_top + sectionProperty->getTextHeight () - 1;

	// size of margins
	m_leftMargin = m_left;
	m_rightMargin = sectionProperty->getRightMargin ();
	m_topMargin = m_top;
	m_bottomMargin = sectionProperty->getBottomMargin ();

	debug ("leftMargin: %i  rightMargin: %i  topMargin: %i  bottomMargin: %i\n",
				m_leftMargin, m_rightMargin, m_topMargin, m_bottomMargin);

	// offset of header & footer
	m_headerFromTop = sectionProperty->getHeaderFromTop ();
	m_footerFromTop = sectionProperty->getFooterFromTop ();

	debug ("headerFromTop: %i   footerFromTop: %i\n", m_headerFromTop, m_footerFromTop);
	
	// adjust margins/PAPERBORDERS to ensure that the header & footer are within them
	// TODO: stop header & footer from changing body's location
	// TODO: recompute offset of margins after recomputing margins
	if (hasHeader ())
		if (m_headerFromTop < m_topMargin) m_topMargin = m_headerFromTop;
	if (hasFooter ())
		if (m_pageHeight - m_footerFromTop < m_bottomMargin) m_bottomMargin = m_pageHeight - m_footerFromTop;

	debug ("adjusted::: leftMargin: %i  rightMargin: %i  topMargin: %i  bottomMargin: %i\n",
				m_leftMargin, m_rightMargin, m_topMargin, m_bottomMargin);

	return 0;
}

int MSWRITEImport::documentStartWrite (const int firstPageNumber)
{
	// get dimensions of paper, borders, margins...
	if (documentGetStats ())
	{
		error ("can't documentGetStats()\n");
		return 1;
	}

	// allocate memory for array of object data
	m_objectData = new MSWRITE_OBJECT_DATA [getNumObjects ()];
	if (!m_objectData)
	{
		error ("cannot allocate memory for m_objectData [%i]\n", getNumObjects ());
		return 1;
	}

	// open maindoc.xml
	if (!m_outfile->open ("root"))
	{
		error ("Cannot open root in store\n");
		return 1;
	}

	// start document
	// TODO: error checking
	tagWrite ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	tagWrite ("<!DOCTYPE DOC PUBLIC \"-//KDE//DTD kword 1.1//EN\" \"http://www.koffice.org/DTD/kword-1.1.dtd\">");
	tagWrite ("<DOC xmlns=\"http://www.koffice.org/DTD/kword\" mime=\"application/x-kword\" syntaxVersion=\"2\" editor=\"KWord\">");

	tagWrite ("<PAPER format=\"1\" width=\"%i\" height=\"%i\" orientation=\"0\" columns=\"1\" "
					"hType=\"%i\" fType=\"%i\">",
					m_pageWidth, m_pageHeight,
					isHeaderOnFirstPage () ? 0 : 2,
					isFooterOnFirstPage () ? 0 : 2);

	tagWrite ("<PAPERBORDERS left=\"%i\" right=\"%i\" top=\"%i\" bottom=\"%i\"/>",
					m_leftMargin, m_rightMargin, m_topMargin, m_bottomMargin);

	tagWrite ("</PAPER>");

	tagWrite ("<ATTRIBUTES processing=\"0\" standardpage=\"1\" unit=\"mm\" hasHeader=\"%i\" hasFooter=\"%i\"/>",
					hasHeader () ? 1 : 0, hasFooter () ? 1 : 0);

	// handle page numbering not starting from 1
	if (firstPageNumber != 1)
		tagWrite ("<VARIABLESETTINGS startingPageNumber=\"%i\"/>", firstPageNumber);

	tagWrite ("<FRAMESETS>");

	return 0;
}

int MSWRITEImport::documentEndWrite (void)
{
	// write framesets for the objects
	tagWrite (m_objectFrameset);

	tagWrite ("</FRAMESETS>");
	tagWrite ("<STYLES>");
	tagWrite ("<STYLE>");
		tagWrite ("<NAME value=\"Standard\"/>");
		tagWrite ("<FLOW align=\"left\"/>");
		tagWrite ("<INDENTS first=\"0\" left=\"0\" right=\"0\"/>");
		tagWrite ("<OFFSETS before=\"0\" after=\"0\"/>");
		tagWrite ("<LINESPACING value=\"0\"/>");

		tagWrite ("<FORMAT id=\"1\">");
			tagWrite ("<COLOR blue=\"0\" red=\"0\" green=\"0\"/>");
			tagWrite ("<FONT name=\"helvetica\"/>");
			tagWrite ("<SIZE value=\"12\"/>");
			tagWrite ("<WEIGHT value=\"50\"/>");
			tagWrite ("<ITALIC value=\"0\"/>");
			tagWrite ("<UNDERLINE value=\"0\"/>");
			tagWrite ("<STRIKEOUT value=\"0\"/>");
			// CHARSET?
			tagWrite ("<VERTALIGN value=\"0\"/>");
		tagWrite ("</FORMAT>");

		tagWrite ("<FOLLOWING name=\"Standard\"/>");
	tagWrite ("</STYLE>");
	tagWrite ("</STYLES>");

	// write out image keys
	tagWrite ("<PIXMAPS>"); tagWrite (m_pixmaps); tagWrite ("</PIXMAPS>");
	tagWrite ("<CLIPARTS>"); tagWrite (m_cliparts); tagWrite ("</CLIPARTS>");

	// end document
	tagWrite ("</DOC>");

	// close maindoc.xml
	m_outfile->close ();

	//
	// output object data

	if (m_objectUpto != getNumObjects ())
		warning ("m_objectUpto (%i) != getNumObjects() (%i) -- this is probably because OLE is unimplemented\n",
					m_objectUpto, getNumObjects ());

	for (int i = 0; i < m_objectUpto; i++)
	{
		debug ("outputting: m_objectData [%i] (\"%s/%s\")   (length: %i)\n",
					i,
					(const char *) m_objectPrefix.utf8 (), (const char *) m_objectData [i].m_nameInStore.utf8 (),
					m_objectData [i].m_dataLength);

		// open file for object in store
		if (!m_outfile->open (m_objectPrefix + (QString) "/" + m_objectData [i].m_nameInStore))
		{
			error ("can't open image in store (%s)\n", (const char *) m_objectData [i].m_nameInStore.utf8 ());
			return 1;
		}

		if (m_outfile->write (m_objectData [i].m_data, m_objectData [i].m_dataLength) == false)
		{
			error ("cannot write m_objectData [%i].data to store (len: %i)\n",
						i, m_objectData [i].m_dataLength);
			return 1;
		}

		// close object in store
		m_outfile->close ();
	}

	delete [] m_objectData;
	m_objectData = (MSWRITE_OBJECT_DATA *) NULL;

	return 0;
}

// pageNewWrite() is called due to entries in the pageTable
// -- however, pageTable can be very inaccurate, so we ignore it
int MSWRITEImport::pageNewWrite (const int)
{
	return 0;
}

// handles explicit page breaks
int MSWRITEImport::pageBreakWrite (void)
{
	// later used in paraEndWrite
	m_pageBreak = true;
	m_pageBreakOffset = m_charInfoCountLen;
	
#if 0
	debug ("pageBreak with %i\n", m_charInfoCountLen);
#endif
	return 0;
}

// handle "(page)" number
int MSWRITEImport::pageNumberWrite (void)
{
	m_charInfoCountLen++;	// not incremented by tagWrite()
	return tagWrite ("#");
}

// write newline unless end-of-paragraph
// (this is the support for paragraphs with multiple newlines)
int MSWRITEImport::newLineWrite (const bool endOfParagraph)
{
	if (!endOfParagraph)
	{
		m_charInfoCountLen++;	// not incremented by tagWrite()
		return tagWrite ("\n");
	}
	else
		return 0;
}

// aka "soft hyphen"
int MSWRITEImport::optionalHyphenWrite (void)
{
	m_charInfoCountLen++;	// not incremented by tagWrite()
	return tagWrite ("\xC2\xAD");
}

int MSWRITEImport::bodyStartWrite (void)
{
	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"0\" name=\"Text Frameset 1\" visible=\"1\">");
	// TODO: runaround?
	tagWrite ("<FRAME runaround=\"1\" autoCreateNewFrame=\"1\" newFrameBehavior=\"0\" copy=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
					m_top, m_bottom, m_left, m_right);

	return 0;
}

int MSWRITEImport::bodyEndWrite (void)
{
	// <PAGEBREAKING hardFrameBreakAfter=\"true\"/>" may have been in the last paragraph
	// and for "hardFrameBreakAfter" to do its work, we need one more final paragraph!
	if (m_needAnotherParagraph)
	{
		debug ("needAnotherParagraph==true in bodyEndWrite()\n");
		tagWrite ("<PARAGRAPH><TEXT></TEXT><LAYOUT></LAYOUT></PARAGRAPH>");
		m_needAnotherParagraph = false;
	}

	tagWrite ("</FRAMESET>");

	// since "Text Frameset 1" has ended, we can output the header & footer, now
	delayOutputFlush ();

	return 0;
}

int MSWRITEImport::headerStartWrite (void)
{
	// headers must go after body in KWord
	delayOutput (true);

	// dummy header frames
	//

	// except, if the header is NOT on the first page, then make an empty "First Page Header"
	// by setting "visible=1"
	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"1\" name=\"First Page Header\" visible=\"%i\">",
						(!isHeaderOnFirstPage ()) ? 1 : 0);
	tagWrite ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehavior=\"2\" autoCreateNewFrame=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
					m_headerFromTop, m_headerFromTop, m_left, m_right);
	tagWrite ("</FRAMESET>");

	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"2\" name=\"Odd Pages Header\" visible=\"0\">");
	tagWrite ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehavior=\"2\" autoCreateNewFrame=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
						m_headerFromTop, m_headerFromTop, m_left, m_right);
	tagWrite ("</FRAMESET>");

	// real header frame
	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"3\" name=\"Even Pages Header\" visible=\"1\">");
	tagWrite ("<FRAME runaround=\"1\" copy=\"1\" newFrameBehavior=\"2\" autoCreateNewFrame=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
						m_headerFromTop, m_headerFromTop, m_left, m_right);
	return 0;
}

int MSWRITEImport::headerEndWrite (void)
{
	tagWrite ("</FRAMESET>");
	delayOutput (false);

	return 0;
}

int MSWRITEImport::footerStartWrite (void)
{
	// footers must go after body in KWord
	delayOutput (true);

	// dummy footer frames
	//

	// except, if the footer is NOT on the first page, then make an empty "First Page Footer"
	// by setting "visible=1"
	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"4\" name=\"First Page Footer\" visible=\"%i\">",
					(!isFooterOnFirstPage ()) ? 1 : 0);
	tagWrite ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehavior=\"2\" autoCreateNewFrame=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
					m_footerFromTop, m_footerFromTop, m_left, m_right);
	tagWrite ("</FRAMESET>");

	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"5\" name=\"Odd Pages Footer\" visible=\"0\">");
	tagWrite ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehavior=\"2\" autoCreateNewFrame=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
					m_footerFromTop, m_footerFromTop, m_left, m_right);
	tagWrite ("</FRAMESET>");

	// real footer frame
	tagWrite ("<FRAMESET frameType=\"1\" frameInfo=\"6\" name=\"Even Pages Footer\" visible=\"1\">");
	tagWrite ("<FRAME runaround=\"1\" copy=\"1\" newFrameBehavior=\"2\" autoCreateNewFrame=\"0\""
					" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
					m_footerFromTop, m_footerFromTop, m_left, m_right);
	return 0;
}

int MSWRITEImport::footerEndWrite (void)
{
	tagWrite ("</FRAMESET>");
	delayOutput (false);

	return 0;
}

int MSWRITEImport::paraInfoStartWrite (const MSWRITE_FPROP_PAP & /*pap*/)
{
	if (tagWrite ("<PARAGRAPH><TEXT>"))
	{
		error ("tagWrite (\"<p\") error\n");
		return 1;
	}

	// reset charInfo counters
	m_charInfoCountStart = 0;
	m_charInfoCountLen = 0;

	return 0;
}

int MSWRITEImport::paraInfoEndWrite (const MSWRITE_FPROP_PAP &pap)
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

		/*debug ("raw indent:  first: %i  left: %i  right: %i\n",
					indentFirst, indentLeft, indentRight);*/

		if (pap.isObject ())
		{
			// MSWrite _always_ ignores "First Line Indent" if it's an object
			if (indentFirst)
				debug ("user specified indentFirst (%i) with an image/object\n", indentFirst);
			indentFirst = 0;

			// MSWrite does not add the horizontal offset of the image from the left margin to the Left Indent
			// -- instead, it selects the bigger one
			// TODO: proper image positioning (see doc IMPERFECT)
			if (m_objectHorizOffset > indentLeft)
			{
				debug ("image is further away from left margin by itself, rather than using indentLeft (%i > %i)\n",
							m_objectHorizOffset, indentLeft);
				indentLeft = m_objectHorizOffset;
			}
		}

		// hopefully these test operations will be cheaper than the XML ones :)
		if (indentFirst || indentLeft || indentRight)
		{
			output += "<INDENTS";
			if (indentFirst) output += " first=\"" + QString::number (indentFirst) + "\"";
			if (indentLeft) output += " left=\"" + QString::number (indentLeft) + "\"";
			if (indentRight) output += " right=\"" + QString::number (indentRight) + "\"";
			output += "/>";
		}

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
			output += "\"/>";
		}

		// we want the document to _look_ like it does in Write
		// but we don't care if this changes other document formatting properties...
		if (!m_wantPureConversion)
		{
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
				amount -= m_lineSpacingFromAbove;
				if (amount <= 0) amount = 0;		// no emulation can be perfect...

				output += QString::number (amount);
				output += "\" />";
			}

			// GUESS (TODO: fix) the amount of trailing linespace
			switch (lineSpacing)
			{
			case 10:
				m_lineSpacingFromAbove = 0;
				break;
			case 15:
				m_lineSpacingFromAbove = 7;
				break;
			case 20:
				m_lineSpacingFromAbove = 14;
				break;
			default:		// unknown
				m_lineSpacingFromAbove = 0;
				break;
			}
		}	// if (!m_wantPureConversion)	{

		if (m_pageBreak)
		{
#if 0
			debug ("\tpagebrk: output: offset: %i lastcharinfo_len: %i\n",
				m_pageBreakOffset, m_charInfoCountLenLast);
#endif

			// page break before all the text
			if (m_pageBreakOffset == 0 && m_charInfoCountLenLast > 0)
			{
				output += "<PAGEBREAKING hardFrameBreak=\"true\"/>";
				m_needAnotherParagraph = false;	// this paragraph is on first page so we don't need another one
			}
			// we assume that the pageBreak was after all the text (TODO: don't assume this)
			else
			{
				output += "<PAGEBREAKING hardFrameBreakAfter=\"true\"/>";
				m_needAnotherParagraph = true;	// need another paragraph for hardFrameBreakAfter to work
			}

			m_pageBreak = false;		// reset flag
		}
		else
			m_needAnotherParagraph = false;
		//debug ("pageBreak: %i\n", m_pageBreak);

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

			//debug ("Tab: isNormal: %i  ptPos: %i\n",
			//			pap.tbd [i].isTabNormal (), pap.tbd [i].getTabNumPoints ());
		}

	output += "</LAYOUT>";

	output += "<FORMATS>";
		output += m_formatOutput; m_formatOutput = "";		// output all the charInfo for this paragraph
	output += "</FORMATS>";

	output += "</PARAGRAPH>";

	if (tagWrite (output)) return 1;

	return 0;
}

int MSWRITEImport::charInfoStartWrite (const MSWRITE_FPROP_CHP & /*chp*/)
{
	return 0;
}

// outputs character formatting tags
int MSWRITEImport::charInfoEndWrite (const MSWRITE_FPROP_CHP &chp)
{
	// output type of format information (page number or normal text)
	m_formatOutput += "<FORMAT id=\"";
	if (chp.isPageNumber ())
		m_formatOutput += "4";
	else
		m_formatOutput += "1";
	m_formatOutput += "\" ";

	m_formatOutput += "pos=\""; m_formatOutput += QString::number (m_charInfoCountStart); m_formatOutput += "\" ";
	m_formatOutput += "len=\""; m_formatOutput += QString::number (m_charInfoCountLen); m_formatOutput += "\">";

	m_charInfoCountStart += m_charInfoCountLen;
	m_charInfoCountLenLast = m_charInfoCountLen;	// used in paraInfoEndWrite()
	m_charInfoCountLen = 0;

	if (chp.isPageNumber ())
	{
		m_formatOutput += "<VARIABLE>";
			m_formatOutput += "<TYPE key=\"NUMBER\" type=\"4\"/>";
			m_formatOutput += "<PGNUM subtype=\"0\" value=\"1\"/>";
		m_formatOutput += "</VARIABLE>";
	}

	int fontCode = chp.getFontCode ();
	if (fontCode >= 0)
	{
		m_formatOutput += "<FONT name=\"";
			m_formatOutput += getFontName (fontCode);
			m_formatOutput += "\"/>";;
			m_formatOutput += "<SIZE value=\"";
			m_formatOutput += QString::number (chp.getFontPointSize ());
			m_formatOutput += "\"/>";
	}
	else
	{
		debug ("no font\n");

		// we can depend on the style so no need for this
		//m_formatOutput += "<FONT name=\"helvetica\"/>";
		//m_formatOutput += "<SIZE value=\"12\"/>";
	}

	if (chp.isBold ())
	  	m_formatOutput += "<WEIGHT value=\"75\"/>";
//else
//  		m_formatOutput += "<WEIGHT value=\"50\" />";

	if (chp.isItalic ())
		m_formatOutput += "<ITALIC value=\"1\"/>";
//	else
//		m_formatOutput += "<ITALIC value=\"0\" />";

	if (chp.isUnderlined ())
		m_formatOutput += "<UNDERLINE value=\"1\"/>";
//	else
//		m_formatOutput += "<UNDERLINE value=\"0\" />";

	/*if (chp.isNormalPosition ())
		m_formatOutput += "<VERTALIGN value=\"0\" />";
	else*/ if (chp.isSubscript ())
		m_formatOutput += "<VERTALIGN value=\"1\"/>";
	else if (chp.isSuperscript ())
		m_formatOutput += "<VERTALIGN value=\"2\"/>";
	/*else
		error ("unknown valign\n");*/

	m_formatOutput += "</FORMAT>";

	return 0;
}

// delayed output functions
// (for Headers and Footers which must come after the Body in KWord)

void MSWRITEImport::delayOutput (const bool yes)
{
	m_delayOutput = yes;
}

int MSWRITEImport::delayOutputFlush (void)
{
	QCString strUtf8 = m_heldOutput.utf8 ();
	return (m_outfile->write (strUtf8, strUtf8.length ()) == false);
}

// text output functions
//

int MSWRITEImport::textWrite_lowLevel (const char *str)
{
#if 0
	return textWrite_lowLevel (QString (str));
#else	// while this is code duplication (of below func), this ensures that no
		// characters are mysteriously converted (this makes optionalHyphenWrite () work)
	if (m_delayOutput)
	{
		// header/footer must be written after main body
		m_heldOutput += str;
		return 0;
	}
	else
	{
		return (m_outfile->write (str, strlen (str)) == false);
	}
#endif
}

int MSWRITEImport::textWrite_lowLevel (const QString &str)
{
	if (m_delayOutput)
	{
		// header/footer must be written after main body
		m_heldOutput += str;
		return 0;
	}
	else
	{
		QCString strUtf8 = str.utf8 ();
		return (m_outfile->write (strUtf8, strUtf8.length ()) == false);
	}
}

int MSWRITEImport::textWrite (const char *inStr)
{
#if 0
	// update character information counter (should be done after charset conversion)
	m_charInfoCountLen += strlen (inStr);
#endif

// old, messy way (and should be done after charset conversion anyway)
#if 0
	char outStr [(MSWRITE_BUFFER_SIZE + 3) * 6];

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
#endif

	// from Win Character Set...
	QString strUnicode;

	// there is a codec, therefore there is a decoder...
	if (m_codec)
	{
		// output Unicode (UTF8)
		strUnicode = m_decoder->toUnicode (inStr, strlen (inStr));
	}
	else
	{
		// output a plain string still in wrong Character Set (hopefully the user won't notice)
		strUnicode = inStr;
	}

	// update character information counter (after charset conversion)
	m_charInfoCountLen += strUnicode.length ();

#if 0
	int k = strUnicode.length ();
	int l = strlen (inStr);
	if (k != l)
	{
		warning ("k(%i) != l(%i) for s(\"%s\")\n", k, l, inStr);
	}
#endif

	// make string XML-friendly
	strUnicode.replace (QRegExp ("&"), "&amp;");
	strUnicode.replace (QRegExp ("<"), "&lt;");
	strUnicode.replace (QRegExp (">"), "&gt;");
	strUnicode.replace (QRegExp ("\""), "&quot;");
	strUnicode.replace (QRegExp ("'"), "&apos;");

	return textWrite_lowLevel (strUnicode);
}

// same as textWrite (+va_arg) but without XML translation
int MSWRITEImport::tagWrite (const char *format, ...)
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

int MSWRITEImport::tagWrite (const int num)
{
	return tagWrite ("%i", num);
}

int MSWRITEImport::tagWrite (const QString &str)
{
	return (textWrite_lowLevel ((const char *) str.utf8 ()));
}

int MSWRITEImport::imageStartWrite (const int imageType, const int outputLength,
													const int widthTwips, const int heightTwips,
													const int widthScaledRel1000, const int heightScaledRel1000,
													const int horizOffsetTwips)
{
	QString imageName;
	QString fileInStore;

	tagWrite ("#");

	m_formatOutput += "<FORMAT id=\"6\" pos=\"0\" len=\"1\">";
	m_formatOutput += "<ANCHOR type=\"frameset\" instance=\"";
	if (imageType == MSWRITE_OBJECT_BMP)
	{
		imageName = "Picture ";
		imageName += QString::number (m_numPixmap);

		m_formatOutput += imageName;

		fileInStore += "pictures/picture" + QString::number (m_numPixmap) + ".bmp";
	}
	else if (imageType == MSWRITE_OBJECT_WMF)
	{
		imageName += "Clipart ";
		imageName += QString::number (m_numClipart);

		m_formatOutput += imageName;

		fileInStore += "cliparts/clipart" + QString::number (m_numClipart) + ".wmf";
	}
	else
	{
		error ("unsupported picture type %i\n", imageType);
		return 1;
	}
	m_formatOutput += "\"/>";
	m_formatOutput += "</FORMAT>";

	if (imageType == MSWRITE_OBJECT_BMP)
	{
		m_objectFrameset += "<FRAMESET frameType=\"2\" frameInfo=\"0\" name=\"";
		m_objectFrameset += imageName;
		m_objectFrameset += "\" visible=\"1\">";

		m_objectFrameset += "<FRAME runaround=\"1\" copy=\"0\" newFrameBehavior=\"1\"";
		m_objectFrameset += " left=\"";
			m_objectFrameset += QString::number (m_left + horizOffsetTwips / 20);
			m_objectFrameset += "\"";
		m_objectFrameset += " right=\"";
			m_objectFrameset += QString::number (m_left + (horizOffsetTwips + widthTwips * widthScaledRel1000 / 1000) / 20);
			m_objectFrameset += "\"";
		m_objectFrameset += " top=\"";
			m_objectFrameset += QString::number (m_top);
			m_objectFrameset += "\"";
		m_objectFrameset += " bottom=\"";
			m_objectFrameset += QString::number (m_top + (heightTwips * heightScaledRel1000 / 1000) / 20);
			m_objectFrameset += "\"/>";

		m_objectFrameset += "<IMAGE keepAspectRatio=\"false\">";
		m_objectFrameset += "<KEY msec=\"0\" hour=\"0\" second=\"0\" minute=\"0\" day=\"1\" month=\"1\" year=\"1970\"";
		m_objectFrameset += " filename=\"";
		m_objectFrameset += fileInStore;
		m_objectFrameset += "\"/>";
		m_objectFrameset += "</IMAGE>";

		m_objectFrameset += "</FRAMESET>";

		m_pixmaps += "<KEY msec=\"0\" hour=\"0\" second=\"0\" minute=\"0\" day=\"1\" month=\"1\" year=\"1970\"";
		m_pixmaps += " name=\"";
		m_pixmaps += fileInStore;
		m_pixmaps += "\"";
		m_pixmaps += " filename=\"";
		m_pixmaps += fileInStore;
		m_pixmaps += "\"/>";

		m_numPixmap++;
	}
	else if (imageType == MSWRITE_OBJECT_WMF)
	{
		m_objectFrameset += "<FRAMESET frameType=\"5\" frameInfo=\"0\" name=\"";
		m_objectFrameset += imageName;
		m_objectFrameset += "\" visible=\"1\">";

		m_objectFrameset += "<FRAME runaround=\"1\" copy=\"0\" newFrameBehavior=\"1\"";
		m_objectFrameset += " left=\"";
			m_objectFrameset += QString::number (m_left + horizOffsetTwips / 20);
			m_objectFrameset += "\"";
		m_objectFrameset += " right=\"";
			m_objectFrameset += QString::number (m_left + (horizOffsetTwips + widthTwips * widthScaledRel1000 / 1000) / 20);
			m_objectFrameset += "\"";
		m_objectFrameset += " top=\"";
			m_objectFrameset += QString::number (m_top);
			m_objectFrameset += "\"";
		m_objectFrameset += " bottom=\"";
			m_objectFrameset += QString::number (m_top + (heightTwips * heightScaledRel1000 / 1000) / 20);
			m_objectFrameset += "\"/>";

		m_objectFrameset += "<CLIPART>";
		m_objectFrameset += "<KEY msec=\"0\" hour=\"0\" second=\"0\" minute=\"0\" day=\"1\" month=\"1\" year=\"1970\"";
		m_objectFrameset += " filename=\"";
		m_objectFrameset += fileInStore;
		m_objectFrameset += "\"/>";
		m_objectFrameset += "</CLIPART>";

		m_objectFrameset += "</FRAMESET>";

		m_cliparts += "<KEY msec=\"0\" hour=\"0\" second=\"0\" minute=\"0\" day=\"1\" month=\"1\" year=\"1970\"";
		m_cliparts += " name=\"";
		m_cliparts += fileInStore;
		m_cliparts += "\"";
		m_cliparts += " filename=\"";
		m_cliparts += fileInStore;
		m_cliparts += "\"/>";

		m_numClipart++;
	}

	m_objectData [m_objectUpto].m_nameInStore = fileInStore;
	m_objectData [m_objectUpto].m_dataLength = outputLength;
	m_objectData [m_objectUpto].m_data = new char [outputLength];
	if (!m_objectData [m_objectUpto].m_data)
	{
		error ("cannot allocate memory for m_objectData [%i].data (len: %i)\n",
					m_objectUpto, outputLength);
		return 1;
	}

	// if anchored images could be positioned properly, this wouldn't be needed
	m_objectHorizOffset = horizOffsetTwips / 20;;

	return 0;
}

int MSWRITEImport::imageWrite (const char *buffer, const unsigned length)
{
	// consistency check: aren't going to write too much data, past end of array?
	if (m_objectData [m_objectUpto].m_dataUpto + (int) length > m_objectData [m_objectUpto].m_dataLength)
	{
		error ("m_objectData [%i].dataUpto (%i) + length (%i) > m_objectData [%i].dataLength (%i)\n",
					m_objectUpto, m_objectData [m_objectUpto].m_dataUpto, length,
					m_objectUpto, m_objectData [m_objectUpto].m_dataLength);
		return 1;
	}

	memcpy (m_objectData  [m_objectUpto].m_data + m_objectData [m_objectUpto].m_dataUpto, buffer, length);
	m_objectData [m_objectUpto].m_dataUpto += length;

	return 0;
}

int MSWRITEImport::imageEndWrite (void)
{
	// consistency check: wrote exactly the right amount of data?
	if (m_objectData [m_objectUpto].m_dataUpto != m_objectData [m_objectUpto].m_dataLength)
		warning ("m_objectData [%i].dataUpto (%i) != m_objectData [%i].dataLength (%i)\n",
					m_objectUpto, m_objectData [m_objectUpto].m_dataUpto,
					m_objectUpto, m_objectData [m_objectUpto].m_dataLength);

	m_objectUpto++;

	return 0;
}

// constructor
MSWRITEImport::MSWRITEImport (KoFilter *, const char *, const QStringList &)
						: KoFilter()
{
	m_wantPureConversion = true;
		
	m_pageBreak = 0;
	m_needAnotherParagraph = false;
	
	m_charInfoCountLenLast = 0;

	m_lineSpacingFromAbove = 0;

	delayOutput (false);

	m_numPixmap = 0;
	m_numClipart = 0;
	m_objectUpto = 0;

	m_infile = (FILE *) NULL;
	m_outfile = (KoStore *) NULL;

	m_codec = NULL;
	m_decoder = NULL;
}

// destructor
MSWRITEImport::~MSWRITEImport ()
{
	delete (m_decoder);

	closeFiles ();
}

// * pureConversion is when the document is imported with the formatting "as is"
// * non-pureConversion is when the filter attempts to compensate for differences in the way MSWrite & KWord
// 	implement formatting (e.g. see linespacing)
void MSWRITEImport::wantPureConversion (bool yesorno)
{
	m_wantPureConversion = yesorno;
}

// front-end filter
KoFilter::ConversionStatus MSWRITEImport::convert (const QCString &from, const QCString &to)
{
	if (to != "application/x-kword" || from != "application/x-mswrite")
		return KoFilter::NotImplemented;

	QString prefixOut = "tar:"; // ###### TODO
	m_objectPrefix = prefixOut;
	debug ("prefixOut: \"%s\"\n", (const char *) prefixOut.utf8 ());

	if (openFiles (m_chain->inputFile().utf8 (), m_chain->outputFile().utf8 ()))
	{
		error ("could not open files\n");
		return KoFilter::FileNotFound;
	}

	// just select CP 1252 until a "Select Encoding" dialog is added
	m_codec = QTextCodec::codecForName ("CP 1252");

	if (m_codec)
		m_decoder = m_codec->makeDecoder();
	else
		warning ("Cannot convert from win charset!\n");

	// output version info of core lib
	// (so when bug reports come in, we know what lib it was using)
	debug (MSWRITE_IMPORT_LIB::getVersion ());

	if (MSWRITE_IMPORT_LIB::filter ())
	{
		error ("could not filter document\n");
		return KoFilter::StupidError;
	}

	return KoFilter::OK;
}

#include <mswriteimport.moc>
