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

#ifndef MSWRITEIMPORT_H
#define MSWRITEIMPORT_H

#include <qstring.h>
#include <qfile.h>
#include <qobject.h>

#include <koFilter.h>
#include <koStore.h>

#include "mswritelib.h"
#define MSWRITE_PROJECT			MSWRITEImport
#define MSWRITE_DEBUG_AREA		30509


class MSWRITE_OBJECT_DATA
{
public:
	char *data;
	int dataLength;
	int dataUpto;
	QString nameInStore;

	MSWRITE_OBJECT_DATA ()
	{
		data = NULL;
		dataLength = 0;
		dataUpto = 0;
	}

	~MSWRITE_OBJECT_DATA ()
	{
		delete [] data;
	}
};


class MSWRITE_PROJECT : public KoFilter, public MSWRITE_IMPORT_LIB
{
	 Q_OBJECT

private:
	void debug (const char *format, ...);
	void warning (const char *format, ...);
	void error (const char *format, ...);

	int openFiles (const char *_infilename, const char *_outfilename);
	void closeFiles (void);

	int infileRead (void *, size_t, size_t);
	int infileSeek (long, int);
	long infileTell (void) const;

	int documentGetStats (void);
	int documentStartWrite (const int _firstPageNumber);
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
	int newLineWrite (void);

	int paraInfoStartWrite (const MSWRITE_FPROP_PAP &);
	int paraInfoEndWrite (const MSWRITE_FPROP_PAP &);
	int charInfoStartWrite (const MSWRITE_FPROP_CHP &);
	int charInfoEndWrite (const MSWRITE_FPROP_CHP &);

	void delayOutput (const bool yes);
	int delayOutputFlush (void);

	int textWrite_lowLevel (const char *str);

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
	char debugStr [MSWRITE_BUFFER_SIZE];

	// files
	FILE *infile;
	KoStore *outfile;
	char infilename [FILENAME_MAX + 1], outfilename [FILENAME_MAX + 1];

	// page/margin dimensions
	int pageWidth, pageHeight;
	int leftMargin, rightMargin, topMargin, bottomMargin;
	int left, right, top, bottom;		// describing border of Text Frameset (position, not magnitude)
	int headerFromTop, footerFromTop;

	// formatting
	QString formatOutput;
	int charInfoCountStart, charInfoCountLen;
	bool pageBreak, needAnotherParagraph;
	int lineSpacingFromAbove;

	// picture counters
	QString objectPrefix;
	int numPixmap, numClipart;
	QString objectFrameset;
	QString pixmaps;
	QString cliparts;

	MSWRITE_OBJECT_DATA *objectData;
	int objectUpto;

	// XML output that is held back until after "Text Frameset 1" is output
	// (i.e. header & footer)
	bool delayOutputVar;
	QString heldOutput;

	// Override MSWRITE_IMPORT_LIB::sigProgress (void) {}
	void sigProgress (const int value)
	{
		KoFilter::sigProgress (value);
	}

public:
	MSWRITE_PROJECT (KoFilter *parent, const char *name, const QStringList&);
	MSWRITE_PROJECT ();

	virtual ~MSWRITE_PROJECT ();

        virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );
};

#endif // MSWRITEIMPORT_H
