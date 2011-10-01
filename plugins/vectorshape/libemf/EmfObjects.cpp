/*
  Copyright 2008 Brad Hards <bradh@frogmouth.net>
  Copyright 2009,2011 Inge Wallin <inge@lysator.liu.se>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either 
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public 
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

// Own
#include "EmfObjects.h"

// Qt
#include <QFont>

// KDE
#include <KDebug>

// LibEmf
#include "Bitmap.h"
#include "EmfRecords.h"


namespace Libemf
{

static int convertFontWeight(quint32 emfWeight);


// ================================================================
//                         class EmrTextObject

// See MS-EMF section 2.2.5.

EmrTextObject::EmrTextObject( QDataStream &stream, quint32 size, TextType textType )
{
    stream >> m_referencePoint;
    size -= 8;
    //kDebug(33100) << "Text ref. point:" << m_referencePoint;;

    stream >> m_charCount;
    size -= 4;
    //kDebug(33100) << "Number of characters in string:" << m_charCount;;

    stream >> m_offString;
    size -= 4;

    // 36 bytes for the body of the parent structure (EMR_EXTTEXTOUTA or EMR_EXTTEXTOUTW)
    // then parts of the EmrText structure
    quint32 offString = m_offString - 36 - 8 - 4 - 4; 

    stream >> m_options;
    size -= 4;
    offString -= 4;

    stream >> m_rectangle;
    size -= 16;
    offString -= 16;

    stream >> m_offDx;
    size -= 4;
    offString -= 4;
    // as for offString. 36 bytes for parent, then the earlier parts of EmrText
    quint32 offDx = m_offDx - 36 - 8 - 4 - 4 - 4 - 16 - 4;

    soakBytes( stream, offString ); // skips over UndefinedSpace1.
    size -= offString;
    offDx -= offString;

    if ( textType ==  SixteenBitChars ) {
        m_textString = recordWChars( stream, m_charCount );
        size -= 2 * m_charCount;
        offDx -= 2 * m_charCount;

        // If the number of characters is uneven, then we need to soak 2
        // bytes to make it a full word.
        if (m_charCount & 0x01) {
            soakBytes( stream, 2 );
            size -= 2;
            offDx -= 2;
        }
    } else {
        m_textString = recordChars( stream, m_charCount );
        size -= m_charCount;
        offDx -= m_charCount;

        // If the number of characters is not a multiple of 4, then we need to soak some
        // bytes to make it a full word.
        int rest = m_charCount % 4;
        if (rest != 0) {
            soakBytes( stream, 4 - rest );
            size -= 4 - rest;
            offDx -= 4 - rest;
        }
    }

    // TODO: parse the spacing array
    soakBytes( stream, size );
}

EmrTextObject::~EmrTextObject()
{
}

QPoint EmrTextObject::referencePoint() const
{
    return m_referencePoint;
}

QString EmrTextObject::textString() const
{
    return m_textString;
}

quint32 EmrTextObject::options() const
{
    return m_options;
}

QRect EmrTextObject::rectangle() const
{
    return m_rectangle;
}

QString EmrTextObject::recordWChars( QDataStream &stream, int numChars )
{
    QString text;
    QChar myChar;
    for ( int i = 0; i < numChars; ++i ) {
        stream >> myChar;
        text.append( myChar );
    }
    return text;
}

QString EmrTextObject::recordChars( QDataStream &stream, int numChars )
{
    QString text;
    quint8 myChar;
    for ( int i = 0; i < numChars; ++i ) {
        stream >> myChar;
        text.append( QChar( myChar ) );
    }
    return text;
}

void EmrTextObject::soakBytes( QDataStream &stream, int numBytes )
{
    quint8 scratch;
    for ( int i = 0; i < numBytes; ++i ) {
        stream >> scratch;
    }
}


// ================================================================
// Objects that will be stored in the object table


QPen extCreatePen(quint32 penStyle, quint32 width,
                  quint8 red, quint8 green, quint8 blue, quint8 reserved)
{
    Q_UNUSED( reserved );

#if 0
    kDebug(31000) << hex << penStyle << dec << width
                  << red << green << blue << reserved;
#endif

    QPen pen;
    pen.setColor( QColor( red, green, blue ) );

    if ( penStyle & PS_GEOMETRIC ) {
	pen.setCosmetic( false );
    } else {
	pen.setCosmetic( true );
    }

    switch ( penStyle & 0xF ) {
    case PS_SOLID:
        pen.setStyle( Qt::SolidLine );
        break;
    case PS_DASH:
        pen.setStyle( Qt::DashLine );
        break;
    case PS_DOT:
        pen.setStyle( Qt::DotLine );
        break;
    case PS_DASHDOT:
        pen.setStyle( Qt::DashDotLine );
        break;
    case PS_DASHDOTDOT:
        pen.setStyle( Qt::DashDotDotLine );
        break;
    case PS_NULL:
        pen.setStyle( Qt::NoPen );
        break;
    case PS_INSIDEFRAME:
        // FIXME: We don't properly support this
        pen.setStyle( Qt::SolidLine );
        break;
    case PS_USERSTYLE:
        kDebug(33100) << "UserStyle pen not yet supported, using SolidLine";
        pen.setStyle( Qt::SolidLine );
        break;
    case PS_ALTERNATE:
        kDebug(33100) << "Alternate pen not yet supported, using DashLine";
        pen.setStyle( Qt::DashLine );
        break;
    default:
        kDebug(33100) << "unexpected pen type, using SolidLine" << (penStyle & 0xF);
        pen.setStyle( Qt::SolidLine );
    }

    switch ( penStyle & PS_ENDCAP_FLAT ) {
    case PS_ENDCAP_ROUND:
        pen.setCapStyle( Qt::RoundCap );
        break;
    case PS_ENDCAP_SQUARE:
        pen.setCapStyle( Qt::SquareCap );
        break;
    case PS_ENDCAP_FLAT:
        pen.setCapStyle( Qt::FlatCap );
        break;
    default:
        kDebug(33100) << "unexpected cap style, using SquareCap" << (penStyle & PS_ENDCAP_FLAT);
        pen.setCapStyle( Qt::SquareCap );
    }
    pen.setWidth(width);

    return pen;
}

QBrush createBrushIndirect(quint32 brushStyle,
                           quint8 red, quint8 green, quint8 blue, quint8 reserved,
                           quint32 brushHatch)
{
    Q_UNUSED( reserved );
    Q_UNUSED( brushHatch );

#if 0
    kDebug(31000) << hex << brushStyle << dec
                  << red << green << blue << reserved << brushHatch;
#endif

    QBrush brush;

    switch ( brushStyle ) {
    case BS_SOLID:
	brush.setStyle( Qt::SolidPattern );
	break;
    case BS_NULL:
	brush.setStyle( Qt::NoBrush );
	break;
    case BS_HATCHED:
	brush.setStyle( Qt::CrossPattern );
	break;
    case BS_PATTERN:
	Q_ASSERT( 0 );
	break;
    case BS_INDEXED:
	Q_ASSERT( 0 );
	break;
    case BS_DIBPATTERN:
	Q_ASSERT( 0 );
	break;
    case BS_DIBPATTERNPT:
	Q_ASSERT( 0 );
	break;
    case BS_PATTERN8X8:
	Q_ASSERT( 0 );
	break;
    case BS_DIBPATTERN8X8:
	Q_ASSERT( 0 );
	break;
    case BS_MONOPATTERN:
	Q_ASSERT( 0 );
	break;
    default:
	Q_ASSERT( 0 );
    }

    brush.setColor( QColor( red, green, blue ) );

    // TODO: Handle the BrushHatch enum.

    return brush;
}

QBrush createMonoBrush(Bitmap *bitmap)
{
    QImage  pattern(bitmap->image());
    QBrush  brush(pattern);

    return brush;
}

QFont extCreateFontIndirectW(const ExtCreateFontIndirectWRecord &extCreateFontIndirectW)
{
    QFont font( extCreateFontIndirectW.fontFace() );

    font.setWeight( convertFontWeight( extCreateFontIndirectW.weight() ) );

    if ( extCreateFontIndirectW.height() < 0 ) {
	font.setPixelSize( -1 * extCreateFontIndirectW.height() );
    } else if ( extCreateFontIndirectW.height() > 0 ) {
        font.setPixelSize( extCreateFontIndirectW.height() );
    } // zero is "use a default size" which is effectively no-op here.

    // .snp files don't always provide 0x01 for italics
    if ( extCreateFontIndirectW.italic() != 0x00 ) {
	font.setItalic( true );
    }

    if ( extCreateFontIndirectW.underline() != 0x00 ) {
	font.setUnderline( true );
    }

    return font;
}


// ----------------------------------------------------------------
// static functions

static int convertFontWeight( quint32 emfWeight )
{
    // FIXME: See how it's done in the wmf library and check if this is suitable here.

    if ( emfWeight == 0 ) {
        return QFont::Normal;
    } else if ( emfWeight <= 200 ) {
        return QFont::Light;
    } else if ( emfWeight <= 450 ) {
        return QFont::Normal;
    } else if ( emfWeight <= 650 ) {
        return QFont::DemiBold;
    } else if ( emfWeight <= 850 ) {
        return QFont::Bold;
    } else {
        return QFont::Black;
    }
}


}
