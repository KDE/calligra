/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#ifndef koGlobal_h
#define koGlobal_h

// paper formats ( mm )
#define PG_A3_WIDTH		297.0
#define PG_A3_HEIGHT		420.0
#define PG_A4_WIDTH		210.0
#define PG_A4_HEIGHT		297.0
#define PG_A5_WIDTH		148.0
#define PG_A5_HEIGHT		210.0
#define PG_B5_WIDTH		182.0
#define PG_B5_HEIGHT		257.0
#define PG_US_LETTER_WIDTH	216.0
#define PG_US_LETTER_HEIGHT	279.0
#define PG_US_LEGAL_WIDTH	216.0
#define PG_US_LEGAL_HEIGHT	356.0
#define PG_US_EXECUTIVE_WIDTH	191.0
#define PG_US_EXECUTIVE_HEIGHT	254.0
#define PG_SCREEN_WIDTH		240.0
#define PG_SCREEN_HEIGHT	180.0

// paper formats ( inch ) (obsolete?)
#define PG_A3_WIDTH_I		  11.69
#define PG_A3_HEIGHT_I		  16.54
#define PG_A4_WIDTH_I		  8.26
#define PG_A4_HEIGHT_I		  11.7
#define PG_A5_WIDTH_I		  5.83
#define PG_A5_HEIGHT_I		  8.27
#define PG_B5_WIDTH_I		  7.17
#define PG_B5_HEIGHT_I		  10.13
#define PG_US_LETTER_WIDTH_I	  8.5
#define PG_US_LETTER_HEIGHT_I	  11.0
#define PG_US_LEGAL_WIDTH_I	  8.5
#define PG_US_LEGAL_HEIGHT_I	  14.0
#define PG_US_EXECUTIVE_WIDTH_I	  7.5
#define PG_US_EXECUTIVE_HEIGHT_I  10.0
#define PG_SCREEN_WIDTH_I	  9.45
#define PG_SCREEN_HEIGHT_I	  7.09

/**
 *  Represents the paper format a document shall be printed on.
 */
enum KoFormat {
    PG_DIN_A3 = 0,
    PG_DIN_A4 = 1,
    PG_DIN_A5 = 2,
    PG_US_LETTER = 3,
    PG_US_LEGAL = 4,
    PG_SCREEN = 5,
    PG_CUSTOM = 6,
    PG_DIN_B5 = 7,
    PG_US_EXECUTIVE = 8
};

/**
 *  Represents the orientation of a printed document.
 */
enum KoOrientation {
    PG_PORTRAIT = 0,
    PG_LANDSCAPE = 1
};

/**
 *  Represents the length unit that is used by an app.
 *  Possible values are for millimeter, points and inches.
 */
enum KoUnit {
    PG_MM = 0,
    PG_PT = 1,
    PG_INCH = 2
};

/**
 * Header/Footer type.
 * 0 ... Header/Footer is the same on all pages
 * 2 ... Header/Footer for the first page differs
 * 3 ... Header/Footer for even - odd pages are different
 */
enum KoHFType {
    HF_SAME = 0,
    HF_FIRST_DIFF = 2,
    HF_EO_DIFF = 3
};

// structure for page layout
struct KoPageLayout
{
    KoFormat format;
    KoOrientation orientation;

    KoUnit unit;

    double ptWidth;
    double ptHeight;
    double ptLeft;
    double ptRight;
    double ptTop;
    double ptBottom;
    double mmWidth;
    double mmHeight;
    double mmLeft;
    double mmTop;
    double mmRight;
    double mmBottom;
    double inchWidth;
    double inchHeight;
    double inchLeft;
    double inchTop;
    double inchRight;
    double inchBottom;

    bool operator==( const KoPageLayout& l ) const {
	if ( unit != l.unit )
	    return false;
	switch( unit ) {
	case PG_PT: {
	    return ( ptWidth == l.ptWidth &&
		     ptHeight == l.ptHeight &&
		     ptLeft == l.ptLeft &&
		     ptRight == l.ptHeight &&
		     ptTop == l.ptTop &&
		     ptBottom == l.ptBottom );
	}
	case PG_MM: {
	    return ( mmWidth == l.mmWidth &&
		     mmHeight == l.mmHeight &&
		     mmLeft == l.mmLeft &&
		     mmRight == l.mmHeight &&
		     mmTop == l.mmTop &&
		     mmBottom == l.mmBottom );
	}
	case PG_INCH: {
	    return ( inchWidth == l.inchWidth &&
		     inchHeight == l.inchHeight &&
		     inchLeft == l.inchLeft &&
		     inchRight == l.inchHeight &&
		     inchTop == l.inchTop &&
		     inchBottom == l.inchBottom );
	}
	}
	return false;
    }
};

// structure for header-footer
struct KoHeadFoot
{
    QString headLeft;
    QString headMid;
    QString headRight;
    QString footLeft;
    QString footMid;
    QString footRight;
};

// structure for columns
struct KoColumns
{
    int columns;
    double ptColumnSpacing;
    double mmColumnSpacing;
    double inchColumnSpacing;
};

// structure for KWord header-Footer
struct KoKWHeaderFooter
{
    KoHFType header;
    KoHFType footer;
    double ptHeaderBodySpacing;
    double ptFooterBodySpacing;
    double mmHeaderBodySpacing;
    double mmFooterBodySpacing;
    double inchHeaderBodySpacing;
    double inchFooterBodySpacing;
};

// 1 inch ^= 72 pt
// 1 inch ^= 25.399956 mm (-pedantic ;p)
// Note: I don't use division but multiplication with the inverse value
// because it's faster ;p (Werner)
#define POINT_TO_MM(px) ((px)*0.352777167)
#define MM_TO_POINT(mm) ((mm)*2.83465058)
#define POINT_TO_INCH(px) ((px)*0.01388888888889)
#define INCH_TO_POINT(inch) ((inch)*72.0)
#define MM_TO_INCH(mm) ((mm)*0.039370147)
#define INCH_TO_MM(inch) ((inch)*25.399956)


#endif // koGlobal
