/* This file is part of the KDE project
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

#ifndef MSWRITEIMPORT_H
#define MSWRITEIMPORT_H

#include <qstring.h>
#include <qfile.h>
#include <qobject.h>
#include <qtextcodec.h>

#include <koFilter.h>
#include <koStore.h>

#include "mswritelib.h"


// KDE debug area for mswritefilter
const int MSWRITE_DEBUG_AREA = 30509;


// KoStore can only have 1 file open at a time
// so we store objects in this structure temporarily
class MSWRITE_OBJECT_DATA
{
public:
	char *m_data;
	int m_dataLength;
	int m_dataUpto;
	QString m_nameInStore;

	MSWRITE_OBJECT_DATA ()
	{
		m_data = NULL;
		m_dataLength = 0;
		m_dataUpto = 0;
	}

	~MSWRITE_OBJECT_DATA ()
	{
		if (m_data) delete [] m_data;
	}
};


class MSWRITEImport : public KoFilter, public MSWRITE_IMPORT_LIB
{
	 Q_OBJECT

private:
	void debug (const char *format, ...);
	void warning (const char *format, ...);
	void error (const char *format, ...);

	int openFiles (const char *infilename);
	void closeFiles (void);

	int infileRead (void *, size_t, size_t);
	int infileSeek (long, int);
	long infileTell (void) const;

	int documentGetStats (void);
	int documentStartWrite (const int firstPageNumber);
	int documentEndWrite (void);

	int bodyStartWrite (void);
	int bodyEndWrite (void);
	int headerStartWrite (void);
	int headerEndWrite (void);
	int footerStartWrite (void);
	int footerEndWrite (void);

	int pageNewWrite (const int pageNumberClaimed = 0);
	int pageBreakWrite (void);
	int pageNumberWrite (void);
	int newLineWrite (const bool endOfParagraph = true);
	int optionalHyphenWrite (void);

	int paraInfoStartWrite (const MSWRITE_FPROP_PAP &);
	int paraInfoEndWrite (const MSWRITE_FPROP_PAP &);
	int charInfoStartWrite (const MSWRITE_FPROP_CHP &);
	int charInfoEndWrite (const MSWRITE_FPROP_CHP &);

	void delayOutput (const bool yes);
	int delayOutputFlush (void);

	int textWrite_lowLevel (const char *str);
	int textWrite_lowLevel (const QString &str);

	int textWrite (const char *inStr);

	int tagWrite (const char *, ...);
	int tagWrite (const int);
	int tagWrite (const QString &str);

	int imageStartWrite (const int imageType, const int outputLength,
								const int widthTwips, const int heightTwips,
								const int widthScaledRel1000, const int heightScaledRel1000,
								const int horizOffsetTwips);
	int imageWrite (const char *buffer, const unsigned length);
	int imageEndWrite (void);

	// temporary string for debugging functions
	char m_debugStr [MSWRITE_BUFFER_SIZE];

	// files
	FILE *m_infile;
	KoStoreDevice *m_outfile;

	// import options (compensate for differences between KWord and MS Write)
	bool m_simulateLinespacing;
	bool m_simulateImageOffset;

	// page/margin dimensions
	int m_pageWidth, m_pageHeight;
	int m_leftMargin, m_rightMargin, m_topMargin, m_bottomMargin;
	int m_left, m_right, m_top, m_bottom;		// describing border of Text Frameset (position, not magnitude)
	int m_headerFromTop, m_footerFromTop;

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

	MSWRITE_OBJECT_DATA *m_objectData;
	int m_objectUpto;

	int m_objectHorizOffset;
	bool m_paraIsImage;

	// XML output that is held back until after "Text Frameset 1" is output
	// (i.e. header & footer)
	bool m_delayOutput;
	QString m_heldOutput;

	// for charset conversion
	QTextCodec *m_codec;
	QTextDecoder *m_decoder;

	// Override MSWRITE_IMPORT_LIB::sigProgress (void) {}
	void sigProgress (const int value)
	{
		KoFilter::sigProgress (value);
	}

public:
	MSWRITEImport (KoFilter *parent, const char *name, const QStringList &);
	MSWRITEImport ();

	virtual ~MSWRITEImport ();

	// front-end filter function
	virtual KoFilter::ConversionStatus convert (const QCString &from, const QCString &to);
};

#endif // MSWRITEIMPORT_H
