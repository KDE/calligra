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

#include <qstringlist.h>

// paper formats ( mm ) - public for compat reasons, but DO NOT USE in new programs !
// See KoPageFormat's methods instead.
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

/**
 *  Represents the paper format a document shall be printed on.
 *  For compatibility reasons, and because of screen and custom,
 *  this enum doesn't map to QPrinter::PageSize but KoPageFormat::printerPageSize does the conversion.
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
    PG_US_EXECUTIVE = 8,
    PG_DIN_A0 = 9,
    PG_DIN_A1 = 10,
    PG_DIN_A2 = 11,
    PG_DIN_A6 = 12,
    PG_DIN_A7 = 13,
    PG_DIN_A8 = 14,
    PG_DIN_A9 = 15,
    PG_DIN_B0 = 16,
    PG_DIN_B1 = 17,
    PG_DIN_B10 = 18,
    PG_DIN_B2 = 19,
    PG_DIN_B3 = 20,
    PG_DIN_B4 = 21,
    PG_DIN_B6 = 22
    // etc.
};

/**
 *  Represents the orientation of a printed document.
 */
enum KoOrientation {
    PG_PORTRAIT = 0,
    PG_LANDSCAPE = 1
};

namespace KoPageFormat
{
    /**
     * Convert a KoFormat into a KPrinter::PageSize.
     * If format is 'screen' it will use A4 landscape.
     * If format is 'custom' it will use A4 portrait.
     * (you may want to take care of those cases separately).
     * Usually passed to KPrinter::setPageSize().
     */
    int /*KPrinter::PageSize*/ printerPageSize( KoFormat format );
    // We return int instead of the enum to avoid including kprinter.h

    /**
     * Returns the width (in mm) for a given page format and orientation
     * 'Custom' isn't supported by this function, obviously.
     */
    double width( KoFormat format, KoOrientation orientation );

    /**
     * Returns the height (in mm) for a given page format and orientation
     * 'Custom' isn't supported by this function, obviously.
     */
    double height( KoFormat format, KoOrientation orientation );

    /**
     * Returns the internal name of the given page format.
     * Use for saving.
     */
    QString formatString( KoFormat format );

    /**
     * Convert a format string (internal name) to a page format value.
     * Use for loading.
     */
    KoFormat formatFromString( const QString & string );

    /**
     * Returns the translated name of the given page format.
     * Use for showing the user.
     */
    QString name( KoFormat format );

    /**
     * Lists the translated names of all the available formats
     */
    QStringList allFormats();

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

    // Deprecated, don't use. pt* should be enough.
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
};

// structure for KWord header-Footer
struct KoKWHeaderFooter
{
    KoHFType header;
    KoHFType footer;
    double ptHeaderBodySpacing;
    double ptFooterBodySpacing;

    // Deprecated, don't use. pt* should be enough.
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
