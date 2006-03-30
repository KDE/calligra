/* This file is part of the KDE project
   Copyright (C) 2001-2003 Clarence Dang <dang@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License Version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License Version 2 for more details.

   You should have received a copy of the GNU Library General Public License
   Version 2 along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef NDEBUG
	//#define DEBUG_XML_OUTPUT
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <qfile.h>
#include <qobject.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtextstream.h>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <kgenericfactory.h>

#include <KoFilterChain.h>
#include <KoStore.h>

#include "list.h"
#include "libmswrite.h"

#include "ImportDialog.h"
#include "mswriteimport.h"


class MSWriteImportFactory : KGenericFactory <MSWriteImport, KoFilter>
{
public:
	MSWriteImportFactory () : KGenericFactory <MSWriteImport, KoFilter> ("kwordmswriteimport")
	{
	}

protected:
	virtual void setupTranslations (void)
	{
		KGlobal::locale()->insertCatalog ("kofficefilters");
	}
};

K_EXPORT_COMPONENT_FACTORY (libmswriteimport, MSWriteImportFactory ())


//
// Device that reads from .WRI file
//

class WRIDevice : public MSWrite::Device
{
private:
	FILE *m_infp;

public:
	WRIDevice () : m_infp (NULL)
	{
	}

	virtual ~WRIDevice ()
	{
		closeFile ();
	}

	bool openFile (const char *fileName)
	{
		m_infp = fopen (fileName, "rb");
		if (!m_infp)
		{
			error (MSWrite::Error::FileError, "could not open file for reading\n");
			return false;
		}

		return true;
	}

	bool closeFile (void)
	{
		if (m_infp)
		{
			if (fclose (m_infp))
			{
				error (MSWrite::Error::FileError, "could not close input file\n");
				return false;
			}

			m_infp = NULL;
		}

		return true;
	}

	bool read (MSWrite::Byte *buf, const MSWrite::DWord numBytes)
	{
		if (fread (buf, 1, (size_t) numBytes, m_infp) != (size_t) numBytes)
		{
			error (MSWrite::Error::FileError, "could not read from input file\n");
			return false;
		}

		return true;
	}

	bool write (const MSWrite::Byte *, const MSWrite::DWord)
	{
		error (MSWrite::Error::InternalError, "writing to an input file?\n");
		return false;
	}

	bool seek (const long offset, const int whence)
	{
		if (fseek (m_infp, offset, whence))
		{
			error (MSWrite::Error::InternalError, "could not seek input file\n");
			return false;
		}

		return true;
	}

	long tell (void)
	{
		return ftell (m_infp);
	}

	void debug (const char *s)
	{
		kDebug (30509) << s;
	}

	void debug (const int i)
	{
		kDebug (30509) << i;
	}

	void error (const int errorCode, const char *message,
					const char * /*file*/ = "", const int /*lineno*/ = 0,
					MSWrite::DWord /*tokenValue*/ = NoToken)
	{
		if (errorCode == MSWrite::Error::Warn)
			kWarning (30509) << message;
		else
		{
			m_error = errorCode;
			kError (30509) << message;
		}
	}
};


//
// Generator that creates the KWord file
//

class KWordGenerator : public MSWrite::Generator, public MSWrite::NeedsDevice
{
private:
	// KoStore can only have 1 file open at a time
	// so we store objects in this structure temporarily
	class WRIObject
	{
	private:
		WRIObject (const WRIObject &rhs);

	public:
		MSWrite::Byte *m_data;
		MSWrite::DWord m_dataLength;
		MSWrite::DWord m_dataUpto;
		QString m_nameInStore;

		WRIObject () : m_data (NULL), m_dataLength (0), m_dataUpto (0)
		{
		}

		~WRIObject ()
		{
			delete [] m_data;
		}

		WRIObject operator= (const WRIObject &rhs)
		{
			delete [] m_data;

			m_dataLength = rhs.m_dataLength;
			m_dataUpto = rhs.m_dataUpto;
			m_nameInStore = rhs.m_nameInStore;

			if (rhs.m_data)
			{
				m_data = new MSWrite::Byte [m_dataLength];
				if (m_data)
					memcpy (m_data, rhs.m_data, m_dataLength);
				// remember to check m_data before use
			}
			else
				m_data = NULL;

			return *this;
		}
	};

	// page/margin dimensions
	int m_pageWidth, m_pageHeight;
	int m_left, m_right, m_top, m_bottom;		// describing border of Text Frameset (position, not magnitude)
	int m_leftMargin, m_rightMargin, m_topMargin, m_bottomMargin;
	int m_headerFromTop, m_footerFromTop;
	bool m_hasHeader, m_isHeaderOnFirstPage;
	bool m_hasFooter, m_isFooterOnFirstPage;
	bool m_writeHeaderFirstTime, m_writeFooterFirstTime;

	int inWhat;

	enum inWhatPossiblities
	{
		Nothing,
		Header,
		Footer,
		Body
	};

	int m_startingPageNumber;

	KoFilterChain *m_chain;
	KoStoreDevice *m_outfile;

	// for charset conversion
	QTextCodec *m_codec;
	QTextDecoder *m_decoder;

	// import options (compensate for differences between KWord and MS Write)
	bool m_simulateLineSpacing;
	bool m_simulateImageOffset;

	// formatting
	QString m_formatOutput;
	int m_charInfoCountStart, m_charInfoCountLen;
	bool m_pageBreak, m_needAnotherParagraph;
	int m_pageBreakOffset;
	int m_lineSpacingFromAbove;

	// picture counters
	int m_numPictures;
	QString m_pictures;

	QString m_objectFrameset;

	MSWrite::List <WRIObject> m_objectList;

	double m_objectHorizOffset;
	bool m_paraIsImage;

	MSWriteImport *m_koLink;

	// XML output that is held back until after "Text Frameset 1" is output
	// (i.e. header & footer must come after the Body in KWord)
	bool m_delayOutput;
	QString m_heldOutput;

	void delayOutput (const bool yes)
	{
		m_delayOutput = yes;
	}

	bool delayOutputFlush (void)
	{
		Q3CString strUtf8 = m_heldOutput.utf8 ();
		int strLength = strUtf8.length ();

		if (m_outfile->write (strUtf8, strLength) != strLength)
			ErrorAndQuit (MSWrite::Error::FileError, "could not write delayed output\n");

		m_heldOutput = "";
		return true;
	}

public:
	KWordGenerator () : m_hasHeader (false), m_isHeaderOnFirstPage (false),
								m_hasFooter (false), m_isFooterOnFirstPage (false),
								m_writeHeaderFirstTime (true), m_writeFooterFirstTime (true),
								inWhat (Nothing),
								m_decoder (NULL),
								m_simulateLineSpacing (false),
								m_simulateImageOffset (true),
								m_pageBreak (false), m_needAnotherParagraph (false),
								m_lineSpacingFromAbove (0),
								m_numPictures (0)
	{
		delayOutput (false);

		// just select windows-1252 until the "Select Encoding" dialog works
		m_codec = QTextCodec::codecForName ("CP 1252");

		if (m_codec)
			m_decoder = m_codec->makeDecoder();
		else
			kWarning (30509) << "Cannot convert from Win Charset!" << endl;
	}

	virtual ~KWordGenerator ()
	{
		delete m_decoder;
	}

	void setFilterChain (KoFilterChain *chain)
	{
		m_chain = chain;
	}

	bool writeDocumentBegin (const MSWrite::Word,
						 				const MSWrite::PageLayout *pageLayout)
	{
		kDebug (30509) << "writeDocumentBegin()" << endl;

		// open maindoc.xml
		m_outfile = m_chain->storageFile ("root", KoStore::Write);
		if (!m_outfile)
			ErrorAndQuit (MSWrite::Error::FileError, "could not open root in store\n");


		//
		// store page dimensions for now
		//

		// page width & height
		m_pageWidth = Twip2Point (pageLayout->getPageWidth ());
		m_pageHeight = Twip2Point (pageLayout->getPageHeight ());

		// offset of margins
		m_left = Twip2Point (pageLayout->getLeftMargin ());
		m_right = m_left + Twip2Point (pageLayout->getTextWidth ()) - 1;
		m_top = Twip2Point (pageLayout->getTopMargin ());
		m_bottom = m_top + Twip2Point (pageLayout->getTextHeight ()) - 1;

		// size of margins
		m_leftMargin = m_left;
		m_rightMargin = Twip2Point (pageLayout->getRightMargin ());
		m_topMargin = m_top;
		m_bottomMargin = Twip2Point (pageLayout->getBottomMargin ());

		kDebug (30509) << "leftMargin: " << m_leftMargin << endl;
		kDebug (30509) << "rightMargin: " << m_rightMargin << endl;
		kDebug (30509) << "topMargin: " << m_topMargin << endl;
		kDebug (30509) << "bottomMargin: " << m_bottomMargin << endl;

		// offset of header & footer
		m_headerFromTop = Twip2Point (pageLayout->getHeaderFromTop ());
		m_footerFromTop = Twip2Point (pageLayout->getFooterFromTop ());

		kDebug (30509) << "headerFromTop: " << m_headerFromTop
				  				<< " footerFromTop: " << m_footerFromTop << endl;

		m_startingPageNumber = pageLayout->getPageNumberStart ();

		return true;
	}

	bool writeDocumentBeginForReal (void)
	{
		kDebug (30509) << "writeDocumentBeginForReal()" << endl;

		// adjust margins/PAPERBORDERS to ensure that the header & footer are
		// within them
		// TODO: stop header & footer from changing body's location
		// TODO: recompute offset of margins after recomputing margins
		if (m_hasHeader)
			if (m_headerFromTop < m_topMargin)
				m_topMargin = m_headerFromTop;

		if (m_hasFooter)
			if (m_pageHeight - m_footerFromTop < m_bottomMargin)
				m_bottomMargin = m_pageHeight - m_footerFromTop;

		kDebug (30509) << "adjusted::: leftMargin: " << m_leftMargin
								<< "  rightMargin: " << m_rightMargin
								<< "  topMargin: " << m_topMargin
								<< "  bottomMargin: " << m_bottomMargin
								<< endl;

		// start document
		// TODO: error checking
		writeTextInternal ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
		writeTextInternal ("<!DOCTYPE DOC PUBLIC \"-//KDE//DTD kword 1.3//EN\" "
									"\"http://www.koffice.org/DTD/kword-1.3.dtd\">");
		writeTextInternal ("<DOC xmlns=\"http://www.koffice.org/DTD/kword\" "
									"mime=\"application/x-kword\" "
									"syntaxVersion=\"3\" editor=\"KWord\">");

		writeTextInternal ("<PAPER format=\"1\" "
									"width=\"%i\" height=\"%i\" "
									"orientation=\"0\" columns=\"1\" "
									"hType=\"%i\" fType=\"%i\">",
									m_pageWidth, m_pageHeight,
									m_isHeaderOnFirstPage ? 0 : 2,
									m_isFooterOnFirstPage ? 0 : 2);

		writeTextInternal ("<PAPERBORDERS left=\"%i\" right=\"%i\" "
									"top=\"%i\" bottom=\"%i\"/>",
									m_leftMargin, m_rightMargin,
									m_topMargin, m_bottomMargin);

		writeTextInternal ("</PAPER>");

		writeTextInternal ("<ATTRIBUTES processing=\"0\" "
									"tabStopValue=\"%lf\" "
									"hasHeader=\"%i\" hasFooter=\"%i\"/>",
									Inch2Point (0.5),
									m_hasHeader ? 1 : 0, m_hasFooter ? 1 : 0);

		// handle page numbering not starting from 1
		if (m_startingPageNumber != 1)
			writeTextInternal ("<VARIABLESETTINGS startingPageNumber=\"%i\"/>",
								 		m_startingPageNumber);

		writeTextInternal ("<FRAMESETS>");

		return true;
	}

	bool writeDocumentEnd (const MSWrite::Word, const MSWrite::PageLayout *)
	{
		kDebug (30509) << "writeDocumentEnd()" << endl;

		// write framesets for the objects
		writeTextInternal (m_objectFrameset);

		writeTextInternal ("</FRAMESETS>");
		writeTextInternal ("<STYLES>");
		writeTextInternal ("<STYLE>");
			writeTextInternal ("<NAME value=\"Standard\"/>");
			writeTextInternal ("<FLOW align=\"left\"/>");
			writeTextInternal ("<INDENTS first=\"0\" left=\"0\" right=\"0\"/>");
			writeTextInternal ("<OFFSETS before=\"0\" after=\"0\"/>");

			writeTextInternal ("<FORMAT id=\"1\">");
				writeTextInternal ("<COLOR blue=\"0\" red=\"0\" green=\"0\"/>");
				writeTextInternal ("<FONT name=\"helvetica\"/>");
				writeTextInternal ("<SIZE value=\"12\"/>");
				writeTextInternal ("<WEIGHT value=\"50\"/>");
				writeTextInternal ("<ITALIC value=\"0\"/>");
				writeTextInternal ("<UNDERLINE value=\"0\"/>");
				writeTextInternal ("<STRIKEOUT value=\"0\"/>");
				writeTextInternal ("<VERTALIGN value=\"0\"/>");
			writeTextInternal ("</FORMAT>");

			writeTextInternal ("<FOLLOWING name=\"Standard\"/>");
		writeTextInternal ("</STYLE>");
		writeTextInternal ("</STYLES>");

		// write out image keys
		writeTextInternal ("<PICTURES>");
			writeTextInternal (m_pictures);
		writeTextInternal ("</PICTURES>");

		// end document
		writeTextInternal ("</DOC>");

		// close maindoc.xml
		m_outfile->close ();
		m_outfile = (KoStoreDevice *) NULL;

		//
		// output object data

		/*if (m_objectUpto != getNumObjects ())
			warning ("m_objectUpto (%i) != getNumObjects() (%i) -- this is probably because OLE is unimplemented\n",
						m_objectUpto, getNumObjects ());*/

		MSWrite::List <WRIObject>::Iterator it;
		MSWrite::List <WRIObject>::Iterator end(m_objectList.end ());
		for (it = m_objectList.begin (); it != end; ++it)
		{
			kDebug (30509) << "outputting object \'" << (*it).m_nameInStore
					  				<< "\'   (length: " << (*it).m_dataLength << ")"
									<< endl;

			if (!(*it).m_data)
				ErrorAndQuit (MSWrite::Error::InternalError, "image data not initialised\n");

			// open file for object in store
			m_outfile = m_chain->storageFile ((*it).m_nameInStore, KoStore::Write);
			if (!m_outfile)
				ErrorAndQuit (MSWrite::Error::FileError, "could not open image in store\n");

			if (m_outfile->write ((const char *) (*it).m_data, (*it).m_dataLength)
				!= (Q_LONG) (*it).m_dataLength)
				ErrorAndQuit (MSWrite::Error::FileError, "could not write image to store\n");

			// close object in store
			m_outfile->close ();
			m_outfile = NULL;
		}

		return true;
	}


	bool writeFooterBegin (void)
	{
		kDebug (30509) << "writeFooterBegin()" << endl;

		inWhat = Footer;
		m_hasFooter = true;

		// footers must go after body in KWord
		delayOutput (true);

		// footer frameset will be written in writeParaInfoBegin()

		return true;
	}

	bool writeFooterEnd (void)
	{
		kDebug (30509) << "writeFooterEnd()" << endl;

		inWhat = Nothing;

		if (!m_writeFooterFirstTime)
			writeTextInternal ("</FRAMESET>");
		delayOutput (false);

		return true;
	}

	bool writeHeaderBegin (void)
	{
		kDebug (30509) << "writeHeaderBegin()" << endl;

		inWhat = Header;
		m_hasHeader = true;

		// headers must go after body in KWord
		delayOutput (true);

		// header frameset will be written in writeParaInfoBegin()

		return true;
	}

	bool writeHeaderEnd (void)
	{
		kDebug (30509) << "writeHeaderEnd()" << endl;

		inWhat = Nothing;

		if (!m_writeHeaderFirstTime)
			writeTextInternal ("</FRAMESET>");
		delayOutput (false);

		return true;
	}

	bool writeBodyBegin (void)
	{
		kDebug (30509) << "writeBodyBegin()" << endl;

		inWhat = Body;

		// writeFooterBegin() and writeHeaderBegin() have been called by now
		// so we have enough information to actually write about them
		writeDocumentBeginForReal ();

		writeTextInternal ("<FRAMESET frameType=\"1\" frameInfo=\"0\" name=\"Text Frameset 1\" visible=\"1\">");
		// TODO: runaround?
		writeTextInternal ("<FRAME runaround=\"1\" autoCreateNewFrame=\"1\" newFrameBehavior=\"0\" copy=\"0\""
									" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
									m_top, m_bottom, m_left, m_right);

		return true;
	}

	bool writeBodyEnd (void)
	{
		kDebug (30509) << "writeBodyEnd()" << endl;

		inWhat = Nothing;

		// <PAGEBREAKING hardFrameBreakAfter=\"true\"/>" may have been in the last paragraph
		// and for "hardFrameBreakAfter" to do its work, we need one more final paragraph!
		if (m_needAnotherParagraph)
		{
			kDebug (30509) << "needAnotherParagraph in bodyEndWrite()" << endl;
			writeTextInternal ("<PARAGRAPH><TEXT></TEXT><LAYOUT></LAYOUT></PARAGRAPH>");
			m_needAnotherParagraph = false;
		}

		writeTextInternal ("</FRAMESET>");

		// since "Text Frameset 1" has ended, we can output the header & footer, now
		delayOutputFlush ();

		return true;
	}

	bool writeParaInfoBegin (const MSWrite::FormatParaProperty *paraProperty,
										const MSWrite::OLE *ole,
										const MSWrite::Image *image)
	{
		//kDebug (30509) << "writeParaInfoBegin()" << endl;

		// reset charInfo counters
		m_charInfoCountStart = 0;
		m_charInfoCountLen = 0;

		if (inWhat == Header)
		{
			m_isHeaderOnFirstPage = paraProperty->getIsOnFirstPage ();

			if (m_writeHeaderFirstTime)
			{
				// dummy header frames
				//

				// except, if the header is NOT on the first page, then make an empty "First Page Header"
				// by setting "visible=1"
				writeTextInternal ("<FRAMESET frameType=\"1\" frameInfo=\"1\" name=\"First Page Header\" visible=\"%i\">",
											m_isHeaderOnFirstPage ? 1 : 0);
				writeTextInternal ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehavior=\"2\" autoCreateNewFrame=\"0\""
											" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
											m_headerFromTop, m_headerFromTop, m_left, m_right);
				writeTextInternal ("</FRAMESET>");

				writeTextInternal ("<FRAMESET frameType=\"1\" frameInfo=\"2\" name=\"Even Pages Header\" visible=\"0\">");
				writeTextInternal ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehavior=\"2\" autoCreateNewFrame=\"0\""
											" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
											m_headerFromTop, m_headerFromTop, m_left, m_right);
				writeTextInternal ("</FRAMESET>");

				// real header frame
				writeTextInternal ("<FRAMESET frameType=\"1\" frameInfo=\"3\" name=\"Odd Pages Header\" visible=\"1\">");
				writeTextInternal ("<FRAME runaround=\"1\" copy=\"1\" newFrameBehavior=\"2\" autoCreateNewFrame=\"0\""
											" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
											m_headerFromTop, m_headerFromTop, m_left, m_right);

				m_writeHeaderFirstTime = false;
			}
		}
		else if (inWhat == Footer)
		{
			m_isFooterOnFirstPage = paraProperty->getIsOnFirstPage ();

			if (m_writeFooterFirstTime)
			{
				// dummy footer frames
				//

				// except, if the footer is NOT on the first page, then make an empty "First Page Footer"
				// by setting "visible=1"
				writeTextInternal ("<FRAMESET frameType=\"1\" frameInfo=\"4\" name=\"First Page Footer\" visible=\"%i\">",
											m_isFooterOnFirstPage ? 1 : 0);
				writeTextInternal ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehavior=\"2\" autoCreateNewFrame=\"0\""
											" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
											m_footerFromTop, m_footerFromTop, m_left, m_right);
				writeTextInternal ("</FRAMESET>");

				writeTextInternal ("<FRAMESET frameType=\"1\" frameInfo=\"5\" name=\"Even Pages Footer\" visible=\"0\">");
				writeTextInternal ("<FRAME runaround=\"1\" copy=\"0\" newFrameBehavior=\"2\" autoCreateNewFrame=\"0\""
											" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
											m_footerFromTop, m_footerFromTop, m_left, m_right);
				writeTextInternal ("</FRAMESET>");

				// real footer frame
				writeTextInternal ("<FRAMESET frameType=\"1\" frameInfo=\"6\" name=\"Odd Pages Footer\" visible=\"1\">");
				writeTextInternal ("<FRAME runaround=\"1\" copy=\"1\" newFrameBehavior=\"2\" autoCreateNewFrame=\"0\""
											" top=\"%i\" bottom=\"%i\" left=\"%i\" right=\"%i\"/>",
											m_footerFromTop, m_footerFromTop, m_left, m_right);

				m_writeFooterFirstTime = false;
			}
		}

		if (!writeTextInternal ("<PARAGRAPH><TEXT>")) return false;

		if (image)
		{
			kDebug (30509) << "Paragraph is an image!" << endl;

			QString imageName;
			QString fileInStore;


			// give picture a name
			//

			imageName = "Picture ";
			imageName += QString::number (m_numPictures + 1);	// image numbers start at 1...


			// give picture a filename
			//

			fileInStore = "pictures/picture" + QString::number (m_numPictures + 1);

			kDebug (30509) << "\tGetting type..." << endl;

			// append extension
			if (image->getIsBMP ())
				fileInStore += ".bmp";
			else if (image->getIsWMF () )
				fileInStore += ".wmf";
			else
				ErrorAndQuit (MSWrite::Error::InternalError, "unsupported picture type\n");


			// indicate anchored image in formatting
			//
			kDebug (30509) << "\tIndicating anchored image in formatting" << endl;
			if (!writeTextInternal ("#")) return false;

			m_formatOutput += "<FORMAT id=\"6\" pos=\"0\" len=\"1\">";
				m_formatOutput += "<ANCHOR type=\"frameset\" instance=\"";
					m_formatOutput += imageName;
				m_formatOutput += "\"/>";
			m_formatOutput += "</FORMAT>";


			// write framesets (truly written in documentEndWrite())
			//
			kDebug (30509) << "\tWriting framesets!" << endl;

			m_objectFrameset += "<FRAMESET frameType=\"2\" frameInfo=\"0\" name=\"";
			m_objectFrameset += imageName;
			m_objectFrameset += "\" visible=\"1\">";

				m_objectFrameset += "<FRAME runaround=\"1\" copy=\"0\" newFrameBehavior=\"1\"";

				const double imageLeft = double (m_left) + Twip2Point (double (image->getIndent ()));
				m_objectFrameset += " left=\"";
					m_objectFrameset += QString::number (imageLeft);
					m_objectFrameset += "\"";

				const double imageWidth = Twip2Point (double (image->getDisplayedWidth ()));
				m_objectFrameset += " right=\"";
					m_objectFrameset += QString::number (imageLeft + imageWidth - 1);
					m_objectFrameset += "\"";

				m_objectFrameset += " top=\"";
					m_objectFrameset += QString::number (m_top);
					m_objectFrameset += "\"";

				const double imageHeight = Twip2Point (double (image->getDisplayedHeight ()));
				m_objectFrameset += " bottom=\"";
					m_objectFrameset += QString::number (double (m_top) + imageHeight - 1);
					m_objectFrameset += "\"/>";

				m_objectFrameset += "<PICTURE keepAspectRatio=\"false\">";
				m_objectFrameset += "<KEY msec=\"0\" hour=\"0\" second=\"0\" minute=\"0\" day=\"1\" month=\"1\" year=\"1970\"";
				m_objectFrameset += " filename=\"";
				m_objectFrameset += fileInStore;
				m_objectFrameset += "\"/>";
				m_objectFrameset += "</PICTURE>";

			m_objectFrameset += "</FRAMESET>";
		#ifdef DEBUG_XML_OUTPUT
			m_objectFrameset += "\n";
		#endif

			m_pictures += "<KEY msec=\"0\" hour=\"0\" second=\"0\" minute=\"0\" day=\"1\" month=\"1\" year=\"1970\"";
			m_pictures += " name=\"";
			m_pictures += fileInStore;
			m_pictures += "\"";
			m_pictures += " filename=\"";
			m_pictures += fileInStore;
			m_pictures += "\"/>";

			m_numPictures++;


			// store object properties
			//
			kDebug (30509) << "\tStoring object" << endl;

			if (!m_objectList.addToBack ())
				ErrorAndQuit (MSWrite::Error::OutOfMemory, "could not allocate memory for object\n");

			WRIObject &obj = *m_objectList.begin (false);
			obj.m_nameInStore = fileInStore;
			obj.m_dataLength = image->getExternalImageSize ();
			obj.m_data = new MSWrite::Byte [obj.m_dataLength];
			if (!obj.m_data)
				ErrorAndQuit (MSWrite::Error::OutOfMemory, "could not allocate memory for object data\n");

			// if anchored images could be positioned properly, this wouldn't be needed
			m_objectHorizOffset = double (Twip2Point (image->getIndent ()));
			m_paraIsImage = true;
		}
		else
		{
			if (ole)
			{
				if (!writeTextInternal ("[OLE unsupported]")) return false;
			}

			m_paraIsImage = false;
		}


		return true;
	}

	bool writeParaInfoEnd (const MSWrite::FormatParaProperty *paraProperty,
									const MSWrite::OLE * /*ole*/,
									const MSWrite::Image *image)

	{
		//kDebug (30509) << "writeParaInfoEnd()" << endl;

		if (image)
		{
			WRIObject &obj = *m_objectList.begin (false);

			// consistency check: wrote exactly the right amount of data?
			if (obj.m_dataUpto != obj.m_dataLength)
				kWarning (30509) << "obj.dataUpto (" << obj.m_dataUpto
						  				<< ") != obj.dataLength (" << obj.m_dataLength
										<< ")" << endl;
		}

		QString output;
		output += "</TEXT>";

		output += "<LAYOUT>";
			output += "<NAME value=\"Standard\"/>";

			int align = paraProperty->getAlignment ();

			if (align != MSWrite::Alignment::Left)
			{
				output += "<FLOW align=\"";
				switch (align)
				{
					/*case MSWrite::Alignment::Left:
						output += "left";
						break;*/
					case MSWrite::Alignment::Centre:
						output += "center";
						break;
					case MSWrite::Alignment::Right:
						output += "right";
						break;
					case MSWrite::Alignment::Justify:
						output += "justify";
						break;
				}
				output += "\"/>";
			}

			double indentFirst = Twip2Point (double (paraProperty->getLeftIndentFirstLine ()));
			double indentLeft = Twip2Point (double (paraProperty->getLeftIndent ()));
			double indentRight = Twip2Point (double (paraProperty->getRightIndent ()));

	#if 0
			debug ("raw indent:  first: %i  left: %i  right: %i\n",
						indentFirst, indentLeft, indentRight);
	#endif

			if (m_paraIsImage /*paraProperty->isObject ()*/ && m_objectHorizOffset)
			{
				if (align == MSWrite::Align::Center)
				{
					// TODO: I don't know what m_objectHorizOffset is relative to!
					kDebug (30509) <<  "ignoring image offset with centred image" << endl;
					m_objectHorizOffset = 0;
				}
				else
				{
					// MSWrite does not add the horizontal offset of the image from the left margin to the Left Indent
					// -- instead, it selects the bigger one
					// TODO: proper image positioning (see doc IMPERFECT)
					if (m_simulateImageOffset && m_objectHorizOffset > indentLeft)
					{
						kDebug (30509) << "image is further away from left margin by itself, rather than using indentLeft ("
												<< m_objectHorizOffset << " > " << indentLeft << ")" << endl;
						indentLeft = m_objectHorizOffset;
					}
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

			MSWrite::Word lineSpacing = paraProperty->getLineSpacing ();

			if (lineSpacing != MSWrite::LineSpacing::Single)
			{
			#if 1
				output += "<LINESPACING type=\"atleast\" spacingvalue=\"" + QString::number (Twip2Point (lineSpacing)) + "\"/>";
			#else	// old way
				output += "<LINESPACING type=\"";
				switch (lineSpacing)
				{
					//case MSWrite:;LineSpacing::Single:
					//	break;
					case MSWrite::LineSpacing::OneAndAHalf:
						output += "oneandhalf";
						break;
					case MSWrite::LineSpacing::Double:
						output += "double";
						break;
					default:
						kWarning (30509) << "non-\"standard\" linespacing value: " <<  lineSpacing << endl;
						output += "atleast\" ";
						output += "spacingvalue=\"";
							output += QString::number (Twip2Point (lineSpacing));
						break;
				}
				output += "\"/>";
			#endif
			}

			// Do we want the linespacing to _look_ like it does in Write?
			// (this adds extra space before each paragraph)
			if (m_simulateLineSpacing)
			{
				// emulate Write's linespacing (aligned to bottom)
				// by using varying amounts of space before the paragraph
				// TODO: test if it works nicely enough (what if you have several different sized fonts in paragraph?)
				if (lineSpacing != MSWrite::LineSpacing::Single)		// if not normal linespacing...
				{
					output += "<OFFSETS before=\"";

					int amount = 0;
					switch (lineSpacing)
					{
					/*case MSWrite::LineSpacing::Single:
						break;*/
					case MSWrite::LineSpacing::OneAndAHalf:
						amount = 7;
						break;
					case MSWrite::LineSpacing::Double:
						amount = 14;
						break;
					default:
						kWarning (30509) << "unknown linespacing value: " << lineSpacing << endl;
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
				case MSWrite::LineSpacing::Single:
					m_lineSpacingFromAbove = 0;
					break;
				case MSWrite::LineSpacing::OneAndAHalf:
					m_lineSpacingFromAbove = 7;
					break;
				case MSWrite::LineSpacing::Double:
					m_lineSpacingFromAbove = 14;
					break;
				default:		// unknown
					m_lineSpacingFromAbove = 0;
					break;
				}
			}	// if (m_simulateLineSpacing)	{

			if (m_pageBreak)
			{
	#if 0
				debug ("\tpagebrk: output: offset: %i chars in paragraph: %i\n",
					m_pageBreakOffset, m_charInfoCountStart + m_charInfoCountLen);
	#endif

				// page break before all the text
				if (m_pageBreakOffset == 0 && m_charInfoCountStart + m_charInfoCountLen > 0)
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

			// Tabulators
			for (int i = 0; i < paraProperty->getNumTabulator (); i++)
			{
				const MSWrite::FormatParaPropertyTabulator *tab = paraProperty->getTabulator (i);
				if (tab->getIsDummy ()) break;

				output += "<TABULATOR";

				if (tab->getIsDecimal ())
					output += " type=\"3\" alignchar=\".\"";
				else
					output += " type=\"0\"";

				output += " ptpos=\"" + QString::number (Twip2Point (double (tab->getIndent ()))) + "\"/>";

				//debug ("Tab: isNormal: %i  ptPos: %i\n",
				//			paraProperty->tbd [i].isTabNormal (), paraProperty->tbd [i].getTabNumPoints ());
			}

		output += "</LAYOUT>";
	#ifdef DEBUG_XML_OUTPUT
		output += "\n";
	#endif

		output += "<FORMATS>";
	#ifdef DEBUG_XML_OUTPUT
		output += "\n";
	#endif
			// output all the charInfo for this paragraph
			output += m_formatOutput; m_formatOutput = "";
		output += "</FORMATS>";
	#ifdef DEBUG_XML_OUTPUT
		output += "\n";
	#endif

		output += "</PARAGRAPH>";
	#ifdef DEBUG_XML_OUTPUT
		output += "\n";
	#endif

		if (!writeTextInternal (output)) return false;

		return true;
	}

	bool writeCharInfoBegin (const MSWrite::FormatCharProperty * /*charProperty*/)
	{
		//kDebug (30509) << "writeCharInfoBegin()" << endl;

		return true;
	}

	// outputs character formatting tags
	bool writeCharInfoEnd (const MSWrite::FormatCharProperty *charProperty,
						 			const bool = false)
	{
		//kDebug (30509) << "writeCharInfoEnd()" << endl;

		// output type of format information (page number or normal text)
		m_formatOutput += "<FORMAT id=\"";
		if (charProperty->getIsPageNumber ())
			m_formatOutput += "4";
		else
			m_formatOutput += "1";
		m_formatOutput += "\" ";

		m_formatOutput += "pos=\""; m_formatOutput += QString::number (m_charInfoCountStart); m_formatOutput += "\" ";
		m_formatOutput += "len=\""; m_formatOutput += QString::number (m_charInfoCountLen); m_formatOutput += "\">";

		m_charInfoCountStart += m_charInfoCountLen;
		m_charInfoCountLen = 0;

		if (charProperty->getIsPageNumber ())
		{
			m_formatOutput += "<VARIABLE>";
				m_formatOutput += "<TYPE key=\"NUMBER\" type=\"4\"/>";
				m_formatOutput += "<PGNUM subtype=\"0\" value=\"1\"/>";
			m_formatOutput += "</VARIABLE>";
		}

		m_formatOutput += "<FONT name=\"";
			m_formatOutput += (const char *) (charProperty->getFont ()->getName ());
			m_formatOutput += "\"/>";
			m_formatOutput += "<SIZE value=\"";
			m_formatOutput += QString::number (charProperty->getFontSize ());
			m_formatOutput += "\"/>";

		if (charProperty->getIsBold ())
			m_formatOutput += "<WEIGHT value=\"75\"/>";
	//else
	//  		m_formatOutput += "<WEIGHT value=\"50\" />";

		if (charProperty->getIsItalic ())
			m_formatOutput += "<ITALIC value=\"1\"/>";
	//	else
	//		m_formatOutput += "<ITALIC value=\"0\" />";

		if (charProperty->getIsUnderlined ())
			m_formatOutput += "<UNDERLINE value=\"1\"/>";
	//	else
	//		m_formatOutput += "<UNDERLINE value=\"0\" />";

		/*if (charProperty->isNormalPosition ())
			m_formatOutput += "<VERTALIGN value=\"0\" />";
		else*/ if (charProperty->getIsSubscript ())
			m_formatOutput += "<VERTALIGN value=\"1\"/>";
		else if (charProperty->getIsSuperscript ())
			m_formatOutput += "<VERTALIGN value=\"2\"/>";
		/*else
			error ("unknown valign\n");*/

		m_formatOutput += "</FORMAT>";

		return true;
	}

	bool writeBinary (const MSWrite::Byte *buffer, const MSWrite::DWord length)
	{
		kDebug (30509) << "writeBinary()" << endl;

		// must be OLE, TODO: implement OLE properly
		if (!m_paraIsImage)
			return true;

		WRIObject &obj = *m_objectList.begin (false);

		if (!obj.m_data)
			ErrorAndQuit (MSWrite::Error::InternalError, "object data not initialised\n");

		// consistency check: aren't going to write past end of array?
		if (obj.m_dataUpto + length > obj.m_dataLength)
		{
			kDebug (30509) << "object image overrun: "
									<< obj.m_dataUpto << " + " << length
									<< " > " << obj.m_dataLength  << endl;
			ErrorAndQuit (MSWrite::Error::InternalError, "object image overrun\n");
		}

		memcpy (obj.m_data + obj.m_dataUpto, buffer, length);
		obj.m_dataUpto += length;

		return true;
	}



	//
	// text output functions
	//

	bool writeText (const MSWrite::Byte *string)
	{
		// from Win Character Set...
		QString strUnicode;

		// there is a codec, therefore there is a decoder...
		if (m_codec)
		{
			// output Unicode (UTF8)
			strUnicode = m_decoder->toUnicode ((const char *) string, strlen ((const char *) string));
		}
		else
		{
			// output a plain string still in wrong Character Set
			// (hopefully the user won't notice)
			strUnicode = (const char *) string;
		}

		// update character information counter (after charset conversion)
		m_charInfoCountLen += strUnicode.length ();

		// make string XML-friendly (TODO: speed up)
		strUnicode.replace ('&', "&amp;");
		strUnicode.replace ('<', "&lt;");
		strUnicode.replace ('>', "&gt;");
		strUnicode.replace ('\"', "&quot;");
		strUnicode.replace ('\'', "&apos;");

		return writeTextInternal (strUnicode);
	}

	bool writeTextInternal (const MSWrite::Byte *str)
	{
	#if 0
		return textWrite_lowLevel (QString (str));
	#else	// while this is code duplication (of below func), this ensures that no
			// characters are mysteriously converted (this makes writeOptionalHyphen () work)
		if (m_delayOutput)
		{
			// header/footer must be written after main body
			m_heldOutput += (const char *) str;
			return true;
		}
		else
		{
			int strLength = strlen ((const char *) str);

			if (m_outfile->write ((const char *) str, strLength) != strLength)
			{
				ErrorAndQuit (MSWrite::Error::FileError, "could not write to maindoc.xml\n");
			}
			else
				return true;
		}
	#endif
	}

	bool writeTextInternal (const QString &str)
	{
		if (m_delayOutput)
		{
			// header/footer must be written after main body
			m_heldOutput += str;
			return true;
		}
		else
		{
			Q3CString strUtf8 = str.utf8 ();
			int strLength = strUtf8.length ();

			if (m_outfile->write (strUtf8, strLength) != strLength)
			{
				ErrorAndQuit (MSWrite::Error::FileError, "could not write to maindoc.xml (2)\n");
			}
			else
				return true;
		}
	}

	bool writeTextInternal (const int num)
	{
		return writeTextInternal (QString::number (num));
	}

	bool writeTextInternal (const char *format, ...)
	{
		va_list list;
		va_start (list, format);

		bool ret;
		// This function is mainly for outputting tags (where XML characters are
		// already escaped and the text is in the right character set...ASCII
		// = UTF-8 for alphanumeric chars I hope).  So _don't_ pass user text
		// to this function (that's what writeText() is for); otherwise you might
		// exceed this 1024 limit.
		char string [1024];

		vsnprintf (string, sizeof (string) - 1, format, list);
		string [sizeof (string) - 1] = '\0';

		ret = writeTextInternal ((const MSWrite::Byte *) string);

		va_end (list);
		return ret;
	}


	// writePageNew() is called for the pageTable
	// -- however, pageTable can be very inaccurate, so we ignore it
	bool writePageNew (const int)
	{
		return true;
	}

	// handles explicit page breaks
	bool writePageBreak (void)
	{
		// later used in paraEndWrite
		m_pageBreak = true;
		m_pageBreakOffset = m_charInfoCountStart + m_charInfoCountLen;

		return true;
	}

	// handle "(page)" number
	bool writePageNumber (void)
	{
		m_charInfoCountLen++;	// not incremented by writeTextInternal()
		return writeTextInternal ("#");
	}

	bool writeCarriageReturn (void)
	{
		return true;	// ignore CR
	}

	// write newline unless end-of-paragraph
	// (this is the support for paragraphs with multiple newlines)
	bool writeNewLine (const bool endOfParagraph)
	{
		if (!endOfParagraph)
		{
			m_charInfoCountLen++;	// not incremented by writeTextInternal()
			return writeTextInternal ("\n");
		}
		else
			return true;
	}

	// aka "soft hyphen"
	bool writeOptionalHyphen (void)
	{
		m_charInfoCountLen++;	// not incremented by writeTextInternal()
		return writeTextInternal ("\xC2\xAD");
	}

	void setKOfficeLink (MSWriteImport *kofficeLink)
	{
		m_koLink = kofficeLink;
	}

	void sigProgress (const int value)
	{
		m_koLink->sigProgress (value);
	}
};



//
// KoFilter
//

MSWriteImport::MSWriteImport (KoFilter *, const char *, const QStringList &)
					: m_device (NULL), m_parser (NULL), m_generator (NULL)
{
}

MSWriteImport::~MSWriteImport ()
{
	delete m_generator;
	delete m_parser;
	delete m_device;
}

KoFilter::ConversionStatus MSWriteImport::convert (const QByteArray &from, const QByteArray &to)
{
	kDebug (30509) << "MSWriteImport $Date$ using LibMSWrite "
			  				<< MSWrite::Version << endl;

	if (to != "application/x-kword" || from != "application/x-mswrite")
	{
		kError (30509) << "Internal error!  Filter not implemented?" << endl;
		return KoFilter::NotImplemented;
	}

#if 0
	//MSWriteImportDialog *dialog = new MSWriteImportDialog ();
	MSWriteImportDialog dialog;

	/*debug ("DIALOG check alloc\n");
	if (!dialog)
	{
		error ("Could not allocate memory for dialog\n");
		return KoFilter::StupidError;
	}*/

	debug ("DIALOG EXEC!!!\n");
	if (!dialog.exec ())
	{
		error ("Dialog was aborted! Aborting filter!\n");
		return KoFilter::UserCancelled;
	}

	debug ("DIALOG GET!!!\n");

	// read settings from dialog
	m_codec = dialog.getCodec ();
	m_simulateLinespacing = dialog.getSimulateLinespacing ();
	m_simulateImageOffset = dialog.getSimulateImageOffset ();
	debug ("Import options: simulateLinespacing: %i\tsimulateImageOffset: %i\n",
				m_simulateLinespacing, m_simulateImageOffset);

	debug ("DIALOG DELETE\n");
	//delete dialog;
#endif

	// create the Device that will read from the .WRI file
	m_device = new WRIDevice;
	if (!m_device)
	{
		kError (30509) << "Could not allocate memory for device" << endl;
		return KoFilter::OutOfMemory;
	}

	// open the .WRI file
	if (!m_device->openFile (QFile::encodeName (m_chain->inputFile ())))
	{
		kError (30509) << "Could not open \'" << m_chain->inputFile () << "\'" << endl;
		return KoFilter::FileNotFound;
	}


	// create Parser that will interpret the .WRI file and call the Generator
	m_parser = new MSWrite::InternalParser;
	if (!m_parser)
	{
		kError (30509) << "Could not allocate memory for parser" << endl;
		return KoFilter::OutOfMemory;
	}

	// tell the Parser to use the Device to read from the .WRI file
	m_parser->setDevice (m_device);


	// create Generator that will produce the .KWD file
	m_generator = new KWordGenerator;
	if (!m_generator)
	{
		kError (30509) << "Could not allocate memory for generator" << endl;
		return KoFilter::OutOfMemory;
	}

	// give the Generator the Device for error-handling purposes
	m_generator->setDevice (m_device);

	// give the Generator the chain
	m_generator->setFilterChain (m_chain);

	// hand over sigProgess to give some feedback to the user
	m_generator->setKOfficeLink (this);


	// hook up Generator to Parser
	m_parser->setGenerator (m_generator);


	// filter!
	if (!m_parser->parse ())
	{
		// try to return somewhat more meaningful errors than StupidError
		// for the day that KOffice actually reports them to the user properly
		int errorCode = m_device->bad ();
		switch (errorCode)
		{
		case MSWrite::Error::Ok:
			kDebug (30509) << "Error::Ok but aborted???" << endl;
			return KoFilter::InternalError;

		case MSWrite::Error::Warn:
			kDebug (30509) << "Error::Warn" << endl;
			return KoFilter::InternalError;	// warnings should _never_ set m_error

		case MSWrite::Error::InvalidFormat:
			kDebug (30509) << "Error::InvalidFormat" << endl;
			return KoFilter::WrongFormat;

		case MSWrite::Error::OutOfMemory:
			kDebug (30509) << "Error::OutOfMemory" << endl;
			return KoFilter::OutOfMemory;

		case MSWrite::Error::InternalError:
			kDebug (30509) << "Error::InternalError" << endl;
			return KoFilter::InternalError;

		case MSWrite::Error::Unsupported:
			kDebug (30509) << "Error::Unsupported" << endl;
			return KoFilter::InternalError;

		case MSWrite::Error::FileError:
			kDebug (30509) << "Error::FileError" << endl;
			return KoFilter::StupidError;	// got a better return value?
		}

		kWarning (30509) << "Unknown error: " << errorCode << endl;
		return KoFilter::StupidError;
	}

	return KoFilter::OK;
}

#include <mswriteimport.moc>
