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

#include "kspread_canvas.h"
#include "kspread_layout.h"
#include "kspread_table.h"
#include "kspread_doc.h"

#include <stdlib.h>
#include <stdio.h>

#include <qdom.h>
#include <qapplication.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kcharsets.h>
#include <koGlobal.h>

using namespace std;

/*****************************************************************************
 *
 * KSpreadLayout
 *
 *****************************************************************************/

KSpreadLayout::KSpreadLayout( KSpreadTable *_table )
{
    m_pTable = _table;
    m_mask = 0;

    m_eFloatColor = KSpreadLayout::AllBlack;
    m_eFloatFormat = KSpreadLayout::OnlyNegSigned;
    m_iPrecision = -1;
    m_bgColor = QColor();
    m_eAlign = KSpreadLayout::Undefined;
    m_eAlignY = KSpreadLayout::Middle;
    m_leftBorderPen.setColor( Qt::black );
    m_leftBorderPen.setWidth( 1 );
    m_leftBorderPen.setStyle( Qt::NoPen );
    m_topBorderPen.setColor( Qt::black );
    m_topBorderPen.setWidth( 1 );
    m_topBorderPen.setStyle( Qt::NoPen );
    m_rightBorderPen.setColor( Qt::black );
    m_rightBorderPen.setWidth( 1 );
    m_rightBorderPen.setStyle( Qt::NoPen );
    m_bottomBorderPen.setColor( Qt::black );
    m_bottomBorderPen.setWidth( 1 );
    m_bottomBorderPen.setStyle( Qt::NoPen );
    m_fallDiagonalPen.setColor( Qt::black );
    m_fallDiagonalPen.setWidth( 1 );
    m_fallDiagonalPen.setStyle( Qt::NoPen );
    m_goUpDiagonalPen.setColor( Qt::black );
    m_goUpDiagonalPen.setWidth( 1 );
    m_goUpDiagonalPen.setStyle( Qt::NoPen );
    m_backGroundBrush.setStyle( Qt::NoBrush);
    m_backGroundBrush.setColor( Qt::red );
    m_dFaktor = 1.0;
    m_bMultiRow = FALSE;
    m_bVerticalText = FALSE;
    m_textPen.setColor( QApplication::palette().active().text() );
    m_eFormatNumber=KSpreadLayout::Number;
    m_rotateAngle=0;
    m_strComment="";
    m_indent=0;
    QFont font = KGlobalSettings::generalFont();
    KGlobal::charsets()->setQFont(font, KGlobal::locale()->charset());
    m_textFont = font;
}

KSpreadLayout::~KSpreadLayout()
{
}

void KSpreadLayout::copy( KSpreadLayout &_l )
{
    m_mask = _l.m_mask;

    m_eFloatColor = _l.m_eFloatColor;
    m_eFloatFormat = _l.m_eFloatFormat;
    m_iPrecision = _l.m_iPrecision;
    m_bgColor = _l.m_bgColor;
    m_eAlign = _l.m_eAlign;
    m_eAlignY = _l.m_eAlignY;
    m_leftBorderPen = _l.m_leftBorderPen;
    m_topBorderPen = _l.m_topBorderPen;
    m_rightBorderPen = _l.m_rightBorderPen;
    m_bottomBorderPen = _l.m_bottomBorderPen;
    m_fallDiagonalPen = _l.m_fallDiagonalPen;
    m_goUpDiagonalPen = _l.m_goUpDiagonalPen;
    m_backGroundBrush = _l.m_backGroundBrush;
    m_dFaktor = _l.m_dFaktor;
    m_bMultiRow = _l.m_bMultiRow;
    m_textPen = _l.m_textPen;
    m_textFont = _l.m_textFont;
    m_strPrefix = _l.m_strPrefix;
    m_strPostfix = _l.m_strPostfix;
    m_bVerticalText = _l.m_bVerticalText;
    m_eFormatNumber = _l.m_eFormatNumber;
    m_rotateAngle = _l.m_rotateAngle;
    m_strComment = _l.m_strComment;
    m_indent=_l.m_indent;
}

void KSpreadLayout::clearProperties()
{
    m_mask = 0;

    layoutChanged();
}

void KSpreadLayout::clearProperty( Properties p )
{
    m_mask &= ~(uint)p;

    layoutChanged();
}

bool KSpreadLayout::hasProperty( Properties p ) const
{
    return ( m_mask & (uint)p );
}

void KSpreadLayout::setProperty( Properties p )
{
    m_mask |= (uint)p;
}

/////////////
//
// Loading and saving
//
/////////////

QDomElement KSpreadLayout::createElement( const QString &tagName, const QFont &font, QDomDocument &doc ) const {

    QDomElement e = doc.createElement( tagName );

    e.setAttribute( "family", font.family() );
    e.setAttribute( "size", font.pointSize() );
    e.setAttribute( "weight", font.weight() );
    if ( font.bold() )
	e.setAttribute( "bold", "yes" );
    if ( font.italic() )
	e.setAttribute( "italic", "yes" );
    if ( font.underline() )
    	e.setAttribute( "underline", "yes" );
    if ( font.strikeOut() )
    	e.setAttribute( "strikeout", "yes" );
    //e.setAttribute( "charset", KGlobal::charsets()->name( font ) );

    return e;
}

QDomElement KSpreadLayout::createElement( const QString& tagname, const QPen& pen, QDomDocument &doc ) const
{
    QDomElement e=doc.createElement( tagname );
    e.setAttribute( "color", pen.color().name() );
    e.setAttribute( "style", (int)pen.style() );
    e.setAttribute( "width", (int)pen.width() );
    return e;
}

QFont KSpreadLayout::toFont(QDomElement &element) const
{
    QFont f;
    f.setFamily( element.attribute( "family" ) );

    bool ok;
    f.setPointSize( element.attribute("size").toInt( &ok ) );
    if ( !ok ) return QFont();

    f.setWeight( element.attribute("weight").toInt( &ok ) );
    if ( !ok ) return QFont();

    if ( element.hasAttribute( "italic" ) && element.attribute("italic") == "yes" )
	f.setItalic( TRUE );

    if ( element.hasAttribute( "bold" ) && element.attribute("bold") == "yes" )
	f.setBold( TRUE );

    if ( element.hasAttribute( "underline" ) && element.attribute("underline") == "yes" )
	f.setUnderline( TRUE );

    if ( element.hasAttribute( "strikeout" ) && element.attribute("strikeout") == "yes" )
	f.setStrikeOut( TRUE );

/* Uncomment when charset is added to kspread_dlg_layout
   + save a document-global charset
    if ( element.hasAttribute( "charset" ) )
	KGlobal::charsets()->setQFont( f, element.attribute("charset") );
    else */
	KGlobal::charsets()->setQFont( f, KGlobal::locale()->charset() );

    return f;
}

QPen KSpreadLayout::toPen(QDomElement &element) const
{
    bool ok;
    QPen p;
    p.setStyle( (Qt::PenStyle)element.attribute("style").toInt( &ok ) );
    if ( !ok ) return QPen();

    p.setWidth( element.attribute("width").toInt( &ok ) );
    if ( !ok ) return QPen();

    p.setColor( QColor( element.attribute("color") ) );

    return p;
}

QDomElement KSpreadLayout::save( QDomDocument& doc ) const
{
    QDomElement format = doc.createElement( "format" );

    if ( hasProperty( PAlign ) )
	format.setAttribute( "align", (int)m_eAlign );
    if ( hasProperty( PAlignY ) )
	format.setAttribute( "alignY", (int)m_eAlignY );
    if ( hasProperty( PBackgroundColor ) && m_bgColor.isValid() )
	format.setAttribute( "bgcolor", m_bgColor.name() );
    if ( hasProperty( PMultiRow ) &&  m_bMultiRow )
	format.setAttribute( "multirow", "yes" );
    if ( hasProperty( PVerticalText ) && m_bVerticalText )
	format.setAttribute( "verticaltext", "yes" );
    if ( hasProperty( PPrecision ) )
	format.setAttribute( "precision", m_iPrecision );
    if ( hasProperty( PPrefix ) && !m_strPrefix.isEmpty() )
	format.setAttribute( "prefix", m_strPrefix );
    if ( hasProperty( PPostfix ) && !m_strPostfix.isEmpty() )
	format.setAttribute( "postfix", m_strPostfix );
    if ( hasProperty( PFloatFormat ) )
	format.setAttribute( "float", (int)m_eFloatFormat );
    if ( hasProperty( PFloatColor ) )
	format.setAttribute( "floatcolor", (int)m_eFloatColor );
    if ( hasProperty( PFaktor ) )
	format.setAttribute( "faktor", m_dFaktor );
    if ( hasProperty( PFormatNumber ) )
	format.setAttribute( "format",(int) m_eFormatNumber);
    if ( hasProperty( PAngle ) )
	format.setAttribute( "angle", m_rotateAngle );
    if ( hasProperty( PIndent ) )
	format.setAttribute( "indent", m_indent );
    if ( hasProperty( PFont ) )
	format.appendChild( createElement( "font", m_textFont, doc ) );
    if ( hasProperty( PTextPen ) && m_textPen.color().isValid())
	format.appendChild( createElement( "pen", m_textPen, doc ) );
    if ( hasProperty( PBackgroundBrush ) )
    {
	format.setAttribute( "brushcolor", m_backGroundBrush.color().name() );
	format.setAttribute( "brushstyle",(int)m_backGroundBrush.style() );
    }
    if ( hasProperty( PLeftBorder ) )
    {
	QDomElement left = doc.createElement( "left-border" );
	left.appendChild( createElement( "pen", m_leftBorderPen, doc ) );
	format.appendChild( left );
    }
    if ( hasProperty( PTopBorder ) )
    {
	QDomElement top = doc.createElement( "top-border" );
	top.appendChild( createElement( "pen", m_topBorderPen, doc ) );
	format.appendChild( top );
    }
    if ( hasProperty( PRightBorder ) )
    {
	QDomElement right = doc.createElement( "right-border" );
	right.appendChild( createElement( "pen", m_rightBorderPen, doc ) );
	format.appendChild( right );
    }
    if ( hasProperty( PBottomBorder ) )
    {
	QDomElement bottom = doc.createElement( "bottom-border" );
	bottom.appendChild( createElement( "pen", m_bottomBorderPen, doc ) );
	format.appendChild( bottom );
    }
    if ( hasProperty( PFallDiagonal ) )
    {
	QDomElement fallDiagonal  = doc.createElement( "fall-diagonal" );
	fallDiagonal.appendChild( createElement( "pen", m_fallDiagonalPen, doc ) );
	format.appendChild( fallDiagonal );
    }
    if ( hasProperty( PGoUpDiagonal ) )
    {
	QDomElement goUpDiagonal = doc.createElement( "up-diagonal" );
	goUpDiagonal.appendChild( createElement( "pen", m_goUpDiagonalPen, doc ) );
	format.appendChild( goUpDiagonal );
    }

    return format;
}

bool KSpreadLayout::load( const QDomElement& f,PasteMode pm )
{
    bool ok;

    if ( f.hasAttribute( "align" ) )
    {
	Align a = (Align)f.attribute("align").toInt( &ok );
	if ( !ok )
	    return false;
	// Validation
	if ( (unsigned int)a < 1 || (unsigned int)a > 4 )
        {
	    kdDebug(36001) << "Value out of range Cell::align=" << (unsigned int)a << endl;
	    return false;
	}
	// Assignment
	setAlign( a );
    }
    if ( f.hasAttribute( "alignY" ) )
    {
	AlignY a = (AlignY)f.attribute("alignY").toInt( &ok );
	if ( !ok )
	    return false;
	// Validation
	if ( (unsigned int)a < 1 || (unsigned int)a > 4 )
        {
	    kdDebug(36001) << "Value out of range Cell::alignY=" << (unsigned int)a << endl;
	    return false;
	}
	// Assignment
	setAlignY( a );
    }

    if ( f.hasAttribute( "bgcolor" ) )
	setBgColor( QColor( f.attribute( "bgcolor" ) ) );

    if ( f.hasAttribute( "multirow" ) )
	setMultiRow( true );

    if ( f.hasAttribute( "verticaltext" ) )
	setVerticalText( true );

    if ( f.hasAttribute( "precision" ) )
    {
	int i = f.attribute("precision").toInt( &ok );
	if ( i < -1 )
        {
	    kdDebug(36001) << "Value out of range Cell::precision=" << i << endl;
	    return false;
	}
        // Assignment
        setPrecision(i);
    }

    if ( f.hasAttribute( "float" ) )
    {
	FloatFormat a = (FloatFormat)f.attribute("float").toInt( &ok );
	if ( !ok ) return false;
	if ( (unsigned int)a < 1 || (unsigned int)a > 3 )
        {
	    kdDebug(36001) << "Value out of range Cell::float=" << (unsigned int)a << endl;
	    return false;
	}
	// Assignment
	setFloatFormat( a );
    }

    if ( f.hasAttribute( "floatcolor" ) )
    {
	FloatColor a = (FloatColor)f.attribute("floatcolor").toInt( &ok );
	if ( !ok ) return false;
	if ( (unsigned int)a < 1 || (unsigned int)a > 2 )
        {
	    kdDebug(36001) << "Value out of range Cell::floatcolor=" << (unsigned int)a << endl;
	    return false;
	}
	// Assignment
	setFloatColor( a );
    }

    if ( f.hasAttribute( "faktor" ) )
    {
	setFaktor( f.attribute("faktor").toDouble( &ok ) );
	if ( !ok ) return false;
    }
    if ( f.hasAttribute( "format" ) )
    {
	setFormatNumber((formatNumber)f.attribute("format").toInt( &ok ));
	if ( !ok ) return false;
    }
    if ( f.hasAttribute( "angle" ) )
    {
            setAngle(f.attribute( "angle").toInt( &ok ));
	    if ( !ok )
		return false;
    }
    if ( f.hasAttribute( "indent" ) )
    {
            setIndent(f.attribute( "indent").toInt( &ok ));
	    if ( !ok )
		return false;
    }
    if ( f.hasAttribute( "brushcolor" ) )
	setBackGroundBrushColor( QColor( f.attribute( "brushcolor" ) ) );

    if ( f.hasAttribute( "brushstyle" ) )
    {
	setBackGroundBrushStyle((Qt::BrushStyle) f.attribute( "brushstyle" ).toInt(&ok)  );
	if(!ok) return false;
    }

    QDomElement pen = f.namedItem( "pen" ).toElement();
    if ( !pen.isNull() )
	setTextPen( toPen(pen) );

    QDomElement font = f.namedItem( "font" ).toElement();
    if ( !font.isNull() )
	setTextFont( toFont(font) );

    if(pm!=NoBorder)
    {
    QDomElement left = f.namedItem( "left-border" ).toElement();
    if ( !left.isNull() )
    {
	QDomElement pen = left.namedItem( "pen" ).toElement();
	if ( !pen.isNull() )
	    setLeftBorderPen( toPen(pen) );
    }

    QDomElement top = f.namedItem( "top-border" ).toElement();
    if ( !top.isNull() )
    {
	QDomElement pen = top.namedItem( "pen" ).toElement();
	if ( !pen.isNull() )
	    setTopBorderPen( toPen(pen) );
    }

    QDomElement right = f.namedItem( "right-border" ).toElement();
    if ( !right.isNull() )
    {
	QDomElement pen = right.namedItem( "pen" ).toElement();
	if ( !pen.isNull() )
	    setRightBorderPen( toPen(pen) );
    }

    QDomElement bottom = f.namedItem( "bottom-border" ).toElement();
    if ( !bottom.isNull() )
    {
	QDomElement pen = bottom.namedItem( "pen" ).toElement();
	if ( !pen.isNull() )
	    setBottomBorderPen( toPen(pen) );
    }

    QDomElement fallDiagonal = f.namedItem( "fall-diagonal" ).toElement();
    if ( !fallDiagonal.isNull() )
    {
	QDomElement pen = fallDiagonal.namedItem( "pen" ).toElement();
	if ( !pen.isNull() )
	    setFallDiagonalPen( toPen(pen) );
    }

    QDomElement goUpDiagonal = f.namedItem( "up-diagonal" ).toElement();
    if ( !goUpDiagonal.isNull() )
    {
	QDomElement pen = goUpDiagonal.namedItem( "pen" ).toElement();
	if ( !pen.isNull() )
	    setGoUpDiagonalPen( toPen(pen) );
    }

    }

    if ( f.hasAttribute( "prefix" ) )
	setPrefix( f.attribute( "prefix" ) );
    if ( f.hasAttribute( "postfix" ) )
	setPostfix( f.attribute( "postfix" ) );

    return true;
}


/////////////
//
// Set methods
//
/////////////

void KSpreadLayout::setAlign( Align _align )
{
    if(_align==KSpreadLayout::Undefined)
        clearProperty( PAlign );
    else
        setProperty( PAlign );

    m_eAlign = _align;
    layoutChanged();
}

void KSpreadLayout::setAlignY( AlignY _alignY)
{
    if(_alignY==KSpreadLayout::Middle)
        clearProperty( PAlignY );
    else
        setProperty( PAlignY );

    m_eAlignY = _alignY;
    layoutChanged();
}

void KSpreadLayout::setFaktor( double _d )
{
    if(_d==1.0)
        clearProperty( PFaktor );
    else
        setProperty( PFaktor );

    m_dFaktor = _d;
    layoutChanged();
}

void KSpreadLayout::setPrefix( const QString& _prefix )
{
    setProperty( PPrefix );

    m_strPrefix = _prefix;
    layoutChanged();
}

void KSpreadLayout::setPostfix( const QString& _postfix )
{
    setProperty( PPostfix );

    m_strPostfix = _postfix;
    layoutChanged();
}

void KSpreadLayout::setPrecision( int _p )
{
    if(_p!=-1)
        setProperty( PPrecision );
    else
        clearProperty( PPrecision );

    m_iPrecision = _p;
    layoutChanged();
}

void KSpreadLayout::setLeftBorderPen( const QPen& _p )
{
    if ( _p.style() == Qt::NoPen )
	clearProperty( PLeftBorder );
    else
	setProperty( PLeftBorder );

    m_leftBorderPen = _p;
    layoutChanged();
}

void KSpreadLayout::setLeftBorderStyle( Qt::PenStyle s )
{
    QPen p = leftBorderPen();
    p.setStyle( s );
    setLeftBorderPen( p );
}

void KSpreadLayout::setLeftBorderColor( const QColor & c )
{
    QPen p = leftBorderPen();
    p.setColor( c );
    setLeftBorderPen( p );
}

void KSpreadLayout::setLeftBorderWidth( int _w )
{
    QPen p = leftBorderPen();
    p.setWidth( _w );
    setLeftBorderPen( p );
}

void KSpreadLayout::setTopBorderPen( const QPen& _p )
{
    if ( _p.style() == Qt::NoPen )
	clearProperty( PTopBorder );
    else
	setProperty( PTopBorder );

    m_topBorderPen = _p;
    layoutChanged();
}

void KSpreadLayout::setTopBorderStyle( Qt::PenStyle s )
{
    QPen p = topBorderPen();
    p.setStyle( s );
    setTopBorderPen( p );
}

void KSpreadLayout::setTopBorderColor( const QColor& c )
{
    QPen p = topBorderPen();
    p.setColor( c );
    setTopBorderPen( p );
}

void KSpreadLayout::setTopBorderWidth( int _w )
{
    QPen p = topBorderPen();
    p.setWidth( _w );
    setTopBorderPen( p );
}

void KSpreadLayout::setRightBorderPen( const QPen& p )
{
    if ( p.style() == Qt::NoPen )
	clearProperty( PRightBorder );
    else
	setProperty( PRightBorder );

    m_rightBorderPen = p;
    layoutChanged();
}

void KSpreadLayout::setRightBorderStyle( Qt::PenStyle _s )
{
    QPen p = rightBorderPen();
    p.setStyle( _s );
    setRightBorderPen( p );
}

void KSpreadLayout::setRightBorderColor( const QColor & _c )
{
    QPen p = rightBorderPen();
    p.setColor( _c );
    setRightBorderPen( p );
}

void KSpreadLayout::setRightBorderWidth( int _w )
{
    QPen p = rightBorderPen();
    p.setWidth( _w );
    setRightBorderPen( p );
}

void KSpreadLayout::setBottomBorderPen( const QPen& p )
{
    if ( p.style() == Qt::NoPen )
	clearProperty( PBottomBorder );
    else
	setProperty( PBottomBorder );

    m_bottomBorderPen = p;
    layoutChanged();
}

void KSpreadLayout::setBottomBorderStyle( Qt::PenStyle _s )
{
    QPen p = bottomBorderPen();
    p.setStyle( _s );
    setBottomBorderPen( p );
}

void KSpreadLayout::setBottomBorderColor( const QColor & _c )
{
    QPen p = bottomBorderPen();
    p.setColor( _c );
    setBottomBorderPen( p );
}

void KSpreadLayout::setBottomBorderWidth( int _w )
{
    QPen p = bottomBorderPen();
    p.setWidth( _w );
    setBottomBorderPen( p );
}

void KSpreadLayout::setFallDiagonalPen( const QPen& _p )
{
    if ( _p.style() == Qt::NoPen )
	clearProperty( PFallDiagonal );
    else
	setProperty( PFallDiagonal );

    m_fallDiagonalPen = _p;
    layoutChanged();
}

void KSpreadLayout::setFallDiagonalStyle( Qt::PenStyle s )
{
    QPen p = fallDiagonalPen();
    p.setStyle( s );
    setFallDiagonalPen( p );
}

void KSpreadLayout::setFallDiagonalColor( const QColor& c )
{
    QPen p = fallDiagonalPen();
    p.setColor( c );
    setFallDiagonalPen( p );
}

void KSpreadLayout::setFallDiagonalWidth( int _w )
{
    QPen p = fallDiagonalPen();
    p.setWidth( _w );
    setFallDiagonalPen( p );
}

void KSpreadLayout::setGoUpDiagonalPen( const QPen& _p )
{
    if ( _p.style() == Qt::NoPen )
	clearProperty( PGoUpDiagonal );
    else
	setProperty( PGoUpDiagonal );

    m_goUpDiagonalPen = _p;
    layoutChanged();
}

void KSpreadLayout::setGoUpDiagonalStyle( Qt::PenStyle s )
{
    QPen p = goUpDiagonalPen();
    p.setStyle( s );
    setGoUpDiagonalPen( p );
}

void KSpreadLayout::setGoUpDiagonalColor( const QColor& c )
{
    QPen p = goUpDiagonalPen();
    p.setColor( c );
    setGoUpDiagonalPen( p );
}

void KSpreadLayout::setGoUpDiagonalWidth( int _w )
{
    QPen p = goUpDiagonalPen();
    p.setWidth( _w );
    setGoUpDiagonalPen( p );
}

void KSpreadLayout::setBackGroundBrush( const QBrush& _p)
{
    if ( _p.style() == Qt::NoBrush )
        clearProperty( PBackgroundBrush );
    else
        setProperty( PBackgroundBrush );

    m_backGroundBrush = _p;
    layoutChanged();
}

void KSpreadLayout::setBackGroundBrushStyle( Qt::BrushStyle s )
{
    QBrush b = backGroundBrush();
    b.setStyle( s );
    setBackGroundBrush( b );
}

void KSpreadLayout::setBackGroundBrushColor( const QColor& c )
{
    QBrush b = backGroundBrush();
    b.setColor( c );
    setBackGroundBrush( b );
}

void KSpreadLayout::setTextFont( const QFont& _f )
{
    setProperty( PFont );

    m_textFont = _f;
    layoutChanged();
}

void KSpreadLayout::setTextFontSize( int _s )
{
    QFont f = textFont();
    f.setPointSize( _s );
    setTextFont( f );
}

void KSpreadLayout::setTextFontFamily( const QString& _f )
{
    QFont f = textFont();
    f.setFamily( _f );
    setTextFont( f );
}

void KSpreadLayout::setTextFontBold( bool _b )
{
    QFont f = textFont();
    f.setBold( _b );
    setTextFont( f );
}

void KSpreadLayout::setTextFontItalic( bool _i )
{
    QFont f = textFont();
    f.setItalic( _i );
    setTextFont( f );
}

void KSpreadLayout::setTextFontUnderline( bool _i )
{
    QFont f = textFont();
    f.setUnderline( _i );
    setTextFont( f );
}

void KSpreadLayout::setTextFontStrike( bool _i )
{
    QFont f = textFont();
    f.setStrikeOut( _i );
    setTextFont( f );
}

void KSpreadLayout::setTextPen( const QPen& _p )
{
    setProperty( PTextPen );

    layoutChanged();
    m_textPen = _p;
}

void KSpreadLayout::setTextColor( const QColor & _c )
{
    QPen p = textPen();
    p.setColor( _c );
    setTextPen( p );
}

void KSpreadLayout::setBgColor( const QColor & _c )
{
    setProperty( PBackgroundColor );

    m_bgColor = _c;
    layoutChanged();
}

void KSpreadLayout::setFloatFormat( FloatFormat _f )
{
    setProperty( PFloatFormat );

    m_eFloatFormat = _f;
    layoutChanged();
}

void KSpreadLayout::setFloatColor( FloatColor _c )
{
    setProperty( PFloatColor );

    m_eFloatColor = _c;
    layoutChanged();
}

void KSpreadLayout::setMultiRow( bool _b )
{
    setProperty( PMultiRow );

    m_bMultiRow = _b;
    layoutChanged();
}

void KSpreadLayout::setVerticalText( bool _b )
{
    setProperty( PVerticalText );

    m_bVerticalText = _b;
    layoutChanged();
}

void KSpreadLayout::setFormatNumber(formatNumber _format)
{
    if ( _format == KSpreadLayout::Number )
        clearProperty( PFormatNumber );
    else
        setProperty( PFormatNumber );

    m_eFormatNumber=_format;
    layoutChanged();

}

void KSpreadLayout::setAngle(int _angle)
{
    if ( _angle == 0 )
        clearProperty( PAngle );
    else
        setProperty( PAngle );

    m_rotateAngle=_angle;
    layoutChanged();
}

void KSpreadLayout::setIndent( int _indent )
{
    if ( _indent==0 )
        clearProperty( PIndent );
    else
        setProperty( PIndent );

    m_indent=_indent;
    layoutChanged();
}

void KSpreadLayout::setComment( const QString& _comment )
{
    if ( _comment.isEmpty() )
        clearProperty( PComment );
    else
        setProperty( PComment );

    m_strComment=_comment;
    layoutChanged();
}


/////////////
//
// Get methods
//
/////////////

QString KSpreadLayout::prefix( int col, int row ) const
{
    if ( !hasProperty( PPrefix ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->prefix( col, row );
    }
    return m_strPrefix;
}

QString KSpreadLayout::postfix( int col, int row ) const
{
    if ( !hasProperty( PPostfix ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->postfix( col, row );
    }
    return m_strPostfix;
}

const QPen& KSpreadLayout::fallDiagonalPen( int col, int row ) const
{
    if ( !hasProperty( PFallDiagonal ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->fallDiagonalPen( col, row );
    }
    return m_fallDiagonalPen;
}

int KSpreadLayout::fallDiagonalWidth( int col, int row ) const
{
    return fallDiagonalPen( col, row ).width();
}

Qt::PenStyle KSpreadLayout::fallDiagonalStyle( int col, int row ) const
{
    return fallDiagonalPen( col, row ).style();
}

const QColor& KSpreadLayout::fallDiagonalColor( int col, int row ) const
{
    return fallDiagonalPen( col, row ).color();
}

const QPen& KSpreadLayout::goUpDiagonalPen( int col, int row ) const
{
    if ( !hasProperty( PGoUpDiagonal ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->goUpDiagonalPen( col, row );
    }
    return m_goUpDiagonalPen;
}

int KSpreadLayout::goUpDiagonalWidth( int col, int row ) const
{
    return goUpDiagonalPen( col, row ).width();
}

Qt::PenStyle KSpreadLayout::goUpDiagonalStyle( int col, int row ) const
{
    return goUpDiagonalPen( col, row ).style();
}

const QColor& KSpreadLayout::goUpDiagonalColor( int col, int row ) const
{
    return goUpDiagonalPen( col, row ).color();
}

const QPen& KSpreadLayout::leftBorderPen( int col, int row ) const
{
    if ( !hasProperty( PLeftBorder ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->leftBorderPen( col, row );
	return table()->emptyPen();
    }

    return m_leftBorderPen;
}

Qt::PenStyle KSpreadLayout::leftBorderStyle( int col, int row ) const
{
    return leftBorderPen( col, row ).style();
}

const QColor& KSpreadLayout::leftBorderColor( int col, int row ) const
{
    return leftBorderPen( col, row ).color();
}

int KSpreadLayout::leftBorderWidth( int col, int row ) const
{
    return leftBorderPen( col, row ).width();
}

const QPen& KSpreadLayout::topBorderPen( int col, int row ) const
{
    if ( !hasProperty( PTopBorder ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->topBorderPen( col, row );
	return table()->emptyPen();
    }

    return m_topBorderPen;
}

const QColor& KSpreadLayout::topBorderColor( int col, int row ) const
{
    return topBorderPen( col, row ).color();
}

Qt::PenStyle KSpreadLayout::topBorderStyle( int col, int row ) const
{
    return topBorderPen( col, row ).style();
}

int KSpreadLayout::topBorderWidth( int col, int row ) const
{
    return topBorderPen( col, row ).width();
}

const QPen& KSpreadLayout::rightBorderPen( int col, int row ) const
{
    if ( !hasProperty( PRightBorder ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->rightBorderPen( col, row );
	return table()->emptyPen();
    }

    return m_rightBorderPen;
}

int KSpreadLayout::rightBorderWidth( int col, int row ) const
{
    return rightBorderPen( col, row ).width();
}

Qt::PenStyle KSpreadLayout::rightBorderStyle( int col, int row ) const
{
    return rightBorderPen( col, row ).style();
}

const QColor& KSpreadLayout::rightBorderColor( int col, int row ) const
{
    return rightBorderPen( col, row ).color();
}

const QPen& KSpreadLayout::bottomBorderPen( int col, int row ) const
{
    if ( !hasProperty( PBottomBorder ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->bottomBorderPen( col, row );
	return table()->emptyPen();
    }

    return m_bottomBorderPen;
}

int KSpreadLayout::bottomBorderWidth( int col, int row ) const
{
    return bottomBorderPen( col, row ).width();
}

Qt::PenStyle KSpreadLayout::bottomBorderStyle( int col, int row ) const
{
    return bottomBorderPen( col, row ).style();
}

const QColor& KSpreadLayout::bottomBorderColor( int col, int row ) const
{
    return bottomBorderPen( col, row ).color();
}

const QBrush& KSpreadLayout::backGroundBrush( int col, int row ) const
{
    if ( !hasProperty( PBackgroundBrush ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->backGroundBrush( col, row );
    }
    return m_backGroundBrush;
}

Qt::BrushStyle KSpreadLayout::backGroundBrushStyle( int col, int row ) const
{
    return backGroundBrush( col, row ).style();
}

const QColor& KSpreadLayout::backGroundBrushColor( int col, int row ) const
{
    return backGroundBrush( col, row ).color();
}

int KSpreadLayout::precision( int col, int row ) const
{
    if ( !hasProperty( PPrecision ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->precision( col, row );
    }
    return m_iPrecision;
}

KSpreadLayout::FloatFormat KSpreadLayout::floatFormat( int col, int row ) const
{
    if ( !hasProperty( PFloatFormat ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->floatFormat( col, row );
    }
    return m_eFloatFormat;
}

KSpreadLayout::FloatColor KSpreadLayout::floatColor( int col, int row ) const
{
    if ( !hasProperty( PFloatColor ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->floatColor( col, row );
    }
    return m_eFloatColor;
}

const QColor& KSpreadLayout::bgColor( int col, int row ) const
{
    if ( !hasProperty( PBackgroundColor ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->bgColor( col, row );
    }

    return m_bgColor.isValid() ? m_bgColor : QApplication::palette().active().base();
}

const QPen& KSpreadLayout::textPen( int col, int row ) const
{
    if ( !hasProperty( PTextPen ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->textPen( col, row );
    }
    return m_textPen;
}

const QColor& KSpreadLayout::textColor( int col, int row ) const
{
    return textPen( col, row ).color();
}

const QFont& KSpreadLayout::textFont( int col, int row ) const
{
    if ( !hasProperty( PFont ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->textFont( col, row );
    }

    return m_textFont;
}

int KSpreadLayout::textFontSize( int col, int row ) const
{
    return textFont( col, row ).pointSize();
}

QString KSpreadLayout::textFontFamily( int col, int row ) const
{
    return textFont( col, row ).family();
}

bool KSpreadLayout::textFontBold( int col, int row ) const
{
    return textFont( col, row ).bold();
}

bool KSpreadLayout::textFontItalic( int col, int row ) const
{
    return textFont( col, row ).italic();
}

bool KSpreadLayout::textFontUnderline( int col, int row ) const
{
    return textFont( col, row ).underline();
}

bool KSpreadLayout::textFontStrike( int col, int row ) const
{
    return textFont( col, row ).strikeOut();
}

KSpreadLayout::Align KSpreadLayout::align( int col, int row ) const
{
    if ( !hasProperty( PAlign ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->align( col, row );
    }

    return m_eAlign;
}

KSpreadLayout::AlignY KSpreadLayout::alignY( int col, int row ) const
{
    if ( !hasProperty( PAlignY ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->alignY( col, row );
    }

    return m_eAlignY;
}

double KSpreadLayout::faktor( int col, int row ) const
{
    if ( !hasProperty( PFaktor ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->faktor( col, row );
    }

    return m_dFaktor;
}

bool KSpreadLayout::multiRow( int col, int row ) const
{
    if ( !hasProperty( PMultiRow ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->multiRow( col, row );
    }

    return m_bMultiRow;
}

bool KSpreadLayout::verticalText( int col, int row ) const
{
    if ( !hasProperty( PVerticalText ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->verticalText( col, row );
    }

    return m_bVerticalText;
}

KSpreadLayout::formatNumber KSpreadLayout::getFormatNumber( int col, int row ) const
{
    if ( !hasProperty( PFormatNumber ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->getFormatNumber( col, row );
    }

    return m_eFormatNumber;
}

int KSpreadLayout::getAngle( int col, int row ) const
{
    if ( !hasProperty( PAngle ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->getAngle( col, row );
    }

    return m_rotateAngle;
}

QString KSpreadLayout::comment( int col, int row ) const
{
    if ( !hasProperty( PComment ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->comment( col, row );
    }

    return m_strComment;
}

int KSpreadLayout::getIndent( int col, int row ) const
{
    if ( !hasProperty( PIndent ) )
    {
	const KSpreadLayout* l = fallbackLayout( col, row );
	if ( l )
	    return l->getIndent( col, row );
    }

    return m_indent;
}


/////////////
//
// Get methods
//
/////////////

const QPen& KSpreadLayout::leftBorderPen() const
{
    return m_leftBorderPen;
}

const QPen& KSpreadLayout::topBorderPen() const
{
    return m_topBorderPen;
}

const QPen& KSpreadLayout::rightBorderPen() const
{
    return m_rightBorderPen;
}

const QPen& KSpreadLayout::bottomBorderPen() const
{
    return m_bottomBorderPen;
}

const QPen& KSpreadLayout::fallDiagonalPen() const
{
    return m_fallDiagonalPen;
}

const QPen& KSpreadLayout::goUpDiagonalPen() const
{
    return m_goUpDiagonalPen;
}

const QBrush& KSpreadLayout::backGroundBrush() const
{
    return m_backGroundBrush;
}

const QFont& KSpreadLayout::textFont() const
{
    return m_textFont;
}

const QPen& KSpreadLayout::textPen() const
{
    return m_textPen;
}

/////////////
//
// Misc
//
/////////////

void KSpreadLayout::layoutChanged()
{
}

KSpreadLayout* KSpreadLayout::fallbackLayout( int, int )
{
    return 0;
}

const KSpreadLayout* KSpreadLayout::fallbackLayout( int, int ) const
{
    return 0;
}

bool KSpreadLayout::isDefault() const
{
    return TRUE;
}

KLocale* KSpreadLayout::locale()
{
    return m_pTable->doc()->locale();
}

/*****************************************************************************
 *
 * KRowLayout
 *
 *****************************************************************************/

#define UPDATE_BEGIN bool b_update_begin = m_bDisplayDirtyFlag; m_bDisplayDirtyFlag = true;
#define UPDATE_END if ( !b_update_begin && m_bDisplayDirtyFlag ) m_pTable->emit_updateRow( this, m_iRow );

RowLayout::RowLayout( KSpreadTable *_table, int _row ) : KSpreadLayout( _table )
{
    m_next = 0;
    m_prev = 0;

    m_bDisplayDirtyFlag = false;
    m_fHeight = POINT_TO_MM(20.0);
    m_iRow = _row;
    m_bDefault = false;
}

RowLayout::~RowLayout()
{
    if ( m_next )
	m_next->setPrevious( m_prev );
    if ( m_prev )
	m_prev->setNext( m_next );
}

void RowLayout::setMMHeight( float _h )
{
  UPDATE_BEGIN;

  m_fHeight = _h;

  UPDATE_END;
}
void RowLayout::setHeight( int _h, KSpreadCanvas *_canvas )
{
  UPDATE_BEGIN;

  if ( _canvas )
    m_fHeight = POINT_TO_MM( _h / _canvas->zoom() );
  else
    m_fHeight = POINT_TO_MM( _h  );

  UPDATE_END;
}

int RowLayout::height( KSpreadCanvas *_canvas )
{
  if ( _canvas )
    return (int)( MM_TO_POINT(_canvas->zoom() * m_fHeight));
  else
    return (int)(MM_TO_POINT(m_fHeight));
}

QDomElement RowLayout::save( QDomDocument& doc, int yshift )
{
    QDomElement row = doc.createElement( "row" );
    row.setAttribute( "height", m_fHeight );
    row.setAttribute( "row", m_iRow - yshift );

    return row;
}

bool RowLayout::load( const QDomElement& row, int yshift )
{
    bool ok;
    if ( row.hasAttribute( "height" ) )
    {
	m_fHeight = row.attribute( "height" ).toFloat( &ok );
	if ( !ok ) return false;
    }

    m_iRow = row.attribute( "row" ).toInt( &ok ) + yshift;
    if ( !ok ) return false;

    // Validation
    if ( m_fHeight < 1 )
    {
	kdDebug(36001) << "Value height=" << m_fHeight << " out of range" << endl;
	return false;
    }
    if ( m_iRow < 1 || m_iRow >= 10000 )
    {
	kdDebug(36001) << "Value row=" << m_iRow << " out of range" << endl;
	return false;
    }

    return true;
}

const QPen& RowLayout::topBorderPen( int _col, int _row ) const
{
    // First look at the row above us
    if ( !hasProperty( PTopBorder ) )
    {
	const RowLayout* rl = table()->rowLayout( _row - 1 );
	if ( rl->hasProperty( PBottomBorder ) )
	    return rl->bottomBorderPen( _col, _row - 1 );
    }

    return KSpreadLayout::topBorderPen( _col, _row );
}

void RowLayout::setTopBorderPen( const QPen& p )
{
    RowLayout* cl = table()->nonDefaultRowLayout( row() - 1, FALSE );
    if ( cl )
	cl->clearProperty( PBottomBorder );

    KSpreadLayout::setTopBorderPen( p );
}

const QPen& RowLayout::bottomBorderPen( int _col, int _row ) const
{
    // First look at the row below of us
    if ( !hasProperty( PBottomBorder ) )
    {
	const RowLayout* rl = table()->rowLayout( _row + 1 );
	if ( rl->hasProperty( PTopBorder ) )
	    return rl->topBorderPen( _col, _row + 1 );
    }

    return KSpreadLayout::bottomBorderPen( _col, _row );
}

void RowLayout::setBottomBorderPen( const QPen& p )
{
    RowLayout* cl = table()->nonDefaultRowLayout( row() + 1, FALSE );
    if ( cl )
	cl->clearProperty( PTopBorder );

    KSpreadLayout::setBottomBorderPen( p );
}

KSpreadLayout* RowLayout::fallbackLayout( int col, int )
{
    return table()->columnLayout( col );
}

const KSpreadLayout* RowLayout::fallbackLayout( int col, int ) const
{
    return table()->columnLayout( col );
}

bool RowLayout::isDefault() const
{
    return m_bDefault;
}

/*****************************************************************************
 *
 * ColumnLayout
 *
 *****************************************************************************/

#undef UPDATE_BEGIN
#undef UPDATE_END

#define UPDATE_BEGIN bool b_update_begin = m_bDisplayDirtyFlag; m_bDisplayDirtyFlag = true;
#define UPDATE_END if ( !b_update_begin && m_bDisplayDirtyFlag ) m_pTable->emit_updateColumn( this, m_iColumn );

ColumnLayout::ColumnLayout( KSpreadTable *_table, int _column ) : KSpreadLayout( _table )
{
  m_bDisplayDirtyFlag = false;
  m_fWidth = POINT_TO_MM(60.0);
  m_iColumn = _column;
  m_bDefault=false;
  m_prev = 0;
  m_next = 0;
}

ColumnLayout::~ColumnLayout()
{
    if ( m_next )
	m_next->setPrevious( m_prev );
    if ( m_prev )
	m_prev->setNext( m_next );
}

void ColumnLayout::setMMWidth( float _w )
{
  UPDATE_BEGIN;

  m_fWidth = _w;

  UPDATE_END;
}

void ColumnLayout::setWidth( int _w, KSpreadCanvas *_canvas )
{
  UPDATE_BEGIN;

  if ( _canvas )
      m_fWidth = POINT_TO_MM(_w / _canvas->zoom());
  else
      m_fWidth = POINT_TO_MM(_w);

  UPDATE_END;
}

int ColumnLayout::width( KSpreadCanvas *_canvas )
{
  if ( _canvas )
    return (int)(MM_TO_POINT( _canvas->zoom() * m_fWidth));
  else
    return (int)(MM_TO_POINT( m_fWidth ));
}

QDomElement ColumnLayout::save( QDomDocument& doc, int xshift )
{
  QDomElement col = doc.createElement( "column" );
  col.setAttribute( "width", m_fWidth );
  col.setAttribute( "column", m_iColumn - xshift );

  return col;
}

bool ColumnLayout::load( const QDomElement& col, int xshift )
{
    bool ok;
    if ( col.hasAttribute( "width" ) )
    {
	m_fWidth = col.attribute( "width" ).toFloat( &ok );
	if ( !ok ) return false;
    }

    m_iColumn = col.attribute( "column" ).toInt( &ok ) + xshift;

    if ( !ok ) return false;

    // Validation
    if ( m_fWidth < 1 )
    {
	kdDebug(36001) << "Value width=" << m_fWidth << " out of range" << endl;
	return false;
    }
    if ( m_iColumn < 1 || m_iColumn >= 10000 )
    {
	kdDebug(36001) << "Value col=" << m_iColumn << " out of range" << endl;
	return false;
    }

    return true;
}

const QPen& ColumnLayout::leftBorderPen( int _col, int _row ) const
{
    // First look ar the right column at the right
    if ( !hasProperty( PLeftBorder ) )
    {
	const ColumnLayout* cl = table()->columnLayout( _col - 1 );
	if ( cl->hasProperty( PRightBorder ) )
	    return cl->rightBorderPen( _col - 1, _row );
    }

    return KSpreadLayout::leftBorderPen( _col, _row );
}

void ColumnLayout::setLeftBorderPen( const QPen& p )
{
    ColumnLayout* cl = table()->nonDefaultColumnLayout( column() - 1, FALSE );
    if ( cl )
	cl->clearProperty( PRightBorder );

    KSpreadLayout::setLeftBorderPen( p );
}

const QPen& ColumnLayout::rightBorderPen( int _col, int _row ) const
{
    // First look ar the right column at the right
    if ( !hasProperty( PRightBorder ) )
    {
	const ColumnLayout* cl = table()->columnLayout( _col + 1 );
	if ( cl->hasProperty( PLeftBorder ) )
	    return cl->leftBorderPen( _col + 1, _row );
    }

    return KSpreadLayout::rightBorderPen( _col, _row );
}

void ColumnLayout::setRightBorderPen( const QPen& p )
{
    ColumnLayout* cl = table()->nonDefaultColumnLayout( column() + 1, FALSE );
    if ( cl )
	cl->clearProperty( PLeftBorder );

    KSpreadLayout::setRightBorderPen( p );
}

KSpreadLayout* ColumnLayout::fallbackLayout( int, int )
{
    return table()->defaultLayout();
}

const KSpreadLayout* ColumnLayout::fallbackLayout( int, int ) const
{
    return table()->defaultLayout();
}

bool ColumnLayout::isDefault() const
{
    return m_bDefault;
}

#undef UPDATE_BEGIN
#undef UPDATE_END


