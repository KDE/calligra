/* This file is part of the KDE project
   Copyright (C) 1998, 1999  Torben Weis <weis@kde.org>
   Copyright (C) 2000 - 2005 The KSpread Team <koffice-devel@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <float.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <kdebug.h>
#include <klocale.h>

#include <KoXmlNS.h>
#include <KoGenStyles.h>
#include <KoGlobal.h>
#include <KoStyleStack.h>
#include <KoOasisLoadingContext.h>
#include <KoOasisStyles.h>

#include "Canvas.h"
#include "Doc.h"
#include "Global.h"
#include "Region.h"
#include "Sheet.h"
#include "SheetPrint.h"
#include "Style.h"
#include "StyleManager.h"

#include "Format.h"

using namespace std;
using namespace KSpread;

// static variable construction
// NOTE Stefan: These values are always overridden by the Doc c'tor.
double ColumnFormat::s_columnWidth = 100.0;
double RowFormat::s_rowHeight = 20.0;

/*****************************************************************************
 *
 * RowFormat
 *
 *****************************************************************************/

class RowFormat::Private
{
public:
    Sheet*      sheet;
    int         row;
    double      height;
    bool        hide;
    RowFormat*  next;
    RowFormat*  prev;
};

RowFormat::RowFormat( Sheet* sheet, int row )
    : d( new Private )
{
    d->sheet    = sheet;
    d->row      = row;
    d->height   = s_rowHeight;
    d->next     = 0;
    d->prev     = 0;
    d->hide     = false;
}

RowFormat::~RowFormat()
{
    if ( d->next )
        d->next->setPrevious( d->prev );
    if ( d->prev )
        d->prev->setNext( d->next );
    delete d;
}

void RowFormat::setMMHeight( double height )
{
    setDblHeight( MM_TO_POINT ( height ) );
}

void RowFormat::setHeight( int height )
{
    setDblHeight( (double) height );
}

void RowFormat::setDblHeight( double height )
{
    // avoid unnecessary updates
    if ( qAbs( height - dblHeight() ) < DBL_EPSILON )
        return;

    // Lower maximum size by old height
    d->sheet->adjustSizeMaxY ( - dblHeight() );

    d->height = height;

    // Rise maximum size by new height
    d->sheet->adjustSizeMaxY ( dblHeight() );
    d->sheet->print()->updatePrintRepeatRowsHeight();
    d->sheet->print()->updateNewPageListY ( row() );

    d->sheet->emit_updateRow( this, d->row );
}

int RowFormat::height() const
{
    return (int) dblHeight();
}

double RowFormat::dblHeight() const
{
    if (d->row == 0)
        return s_rowHeight;
    if( d->hide )
        return 0.0;
    return d->height;
}

double RowFormat::mmHeight() const
{
    return POINT_TO_MM ( dblHeight() );
}

QDomElement RowFormat::save( QDomDocument& doc, int yshift, bool copy ) const
{
    QDomElement row = doc.createElement( "row" );
    row.setAttribute( "height", d->height );
    row.setAttribute( "row", d->row - yshift );
    if( d->hide )
        row.setAttribute( "hide", (int) d->hide );

    const Style style = d->sheet->style( QRect( 1, d->row, KS_colMax, 1 ) );
    if ( !style.isEmpty() )
    {
        kDebug(36003) << "saving cell style of row " << d->row << endl;
        QDomElement format;
        style.saveXML( doc, format, false, copy );
        row.appendChild( format );
    }

    return row;
}

bool RowFormat::load( const KoXmlElement & row, int yshift, Paste::Mode sp, bool paste )
{
    bool ok;

    d->row = row.attribute( "row" ).toInt( &ok ) + yshift;
    if ( !ok )
        return false;

    if ( row.hasAttribute( "height" ) )
    {
        if ( d->sheet->doc()->syntaxVersion() < 1 ) //compatibility with old format - was in millimeter
            d->height = qRound( MM_TO_POINT( row.attribute( "height" ).toDouble( &ok ) ) );
        else
            d->height = row.attribute( "height" ).toDouble( &ok );

        if ( !ok ) return false;
    }

    // Validation
    if ( d->height < 0 )
    {
        kDebug(36001) << "Value height=" << d->height << " out of range" << endl;
        return false;
    }
    if ( d->row < 1 || d->row > KS_rowMax )
    {
        kDebug(36001) << "Value row=" << d->row << " out of range" << endl;
        return false;
    }

    if ( row.hasAttribute( "hide" ) )
    {
        setHidden( (int) row.attribute( "hide" ).toInt( &ok ) );
        if ( !ok )
            return false;
    }

    KoXmlElement f( row.namedItem( "format" ).toElement() );

    if ( !f.isNull() && ( sp == Paste::Normal || sp == Paste::Format || sp == Paste::NoBorder ) )
    {
        Style style;
        if ( !style.loadXML( f, sp, paste ) )
            return false;
        d->sheet->setStyle( Region(QRect( 1, d->row, KS_colMax, 1 )), style );
        return true;
    }

    return true;
}

int RowFormat::row() const
{
    return d->row;
}

void RowFormat::setRow( int row )
{
    d->row = row;
}

RowFormat* RowFormat::next() const
{
    return d->next;
}

RowFormat* RowFormat::previous() const
{
    return d->prev;
}

void RowFormat::setNext( RowFormat* next )
{
    d->next = next;
}

void RowFormat::setPrevious( RowFormat* prev )
{
    d->prev = prev;
}

void RowFormat::setHidden( bool _hide, bool repaint )
{
    if ( _hide != d->hide ) // only if we change the status
    {
        if ( _hide )
        {
	    // Lower maximum size by height of row
            d->sheet->adjustSizeMaxY ( - dblHeight() );
            d->hide = _hide; //hide must be set after we requested the height
            d->sheet->emit_updateRow( this, d->row, repaint );
        }
        else
        {
	    // Rise maximum size by height of row
            d->hide = _hide; //unhide must be set before we request the height
            d->sheet->adjustSizeMaxY ( dblHeight() );
            d->sheet->emit_updateRow( this, d->row, repaint );
        }
    }
}

bool RowFormat::hidden() const
{
    return d->hide;
}

bool RowFormat::isDefault() const
{
    if ( d->height != s_rowHeight )
        return false;
    if ( d->hide == true )
        return false;
    return true;
}

bool RowFormat::operator==( const RowFormat& other ) const
{
    // NOTE Stefan: Don't compare sheet and cell.
    if ( d->height != other.d->height )
        return false;
    if ( d->hide != other.d->hide )
        return false;
    return true;
}

/*****************************************************************************
 *
 * ColumnFormat
 *
 *****************************************************************************/

class ColumnFormat::Private
{
public:
    Sheet*          sheet;
    int             column;
    double          width;
    bool            hide;
    ColumnFormat*   next;
    ColumnFormat*   prev;
};

ColumnFormat::ColumnFormat( Sheet* sheet, int column )
    : d( new Private )
{
    d->sheet    = sheet;
    d->column   = column;
    d->width    = s_columnWidth;
    d->hide     = false;
    d->prev     = 0;
    d->next     = 0;
}

ColumnFormat::~ColumnFormat()
{
    if ( d->next )
        d->next->setPrevious( d->prev );
    if ( d->prev )
        d->prev->setNext( d->next );
    delete d;
}

void ColumnFormat::setMMWidth( double width )
{
    setDblWidth( MM_TO_POINT ( width ) );
}

void ColumnFormat::setWidth( int width )
{
    setDblWidth( (double)width );
}

void ColumnFormat::setDblWidth( double width )
{
    // avoid unnecessary updates
    if ( qAbs( width - dblWidth() ) < DBL_EPSILON )
        return;

    // Lower maximum size by old width
    d->sheet->adjustSizeMaxX ( - dblWidth() );

    d->width = width;

    // Rise maximum size by new width
    d->sheet->adjustSizeMaxX ( dblWidth() );
    d->sheet->print()->updatePrintRepeatColumnsWidth();
    d->sheet->print()->updateNewPageListX ( column() );

    d->sheet->emit_updateColumn( this, d->column );
}

int ColumnFormat::width() const
{
    return (int) dblWidth();
}

double ColumnFormat::dblWidth() const
{
    if (d->column == 0)
        return s_columnWidth;
    if ( d->hide )
        return 0.0;
    return d->width;
}

double ColumnFormat::mmWidth() const
{
    return POINT_TO_MM( dblWidth() );
}


QDomElement ColumnFormat::save( QDomDocument& doc, int xshift, bool copy ) const
{
    QDomElement col( doc.createElement( "column" ) );
    col.setAttribute( "width", d->width );
    col.setAttribute( "column", d->column - xshift );

    if ( d->hide )
        col.setAttribute( "hide", (int) d->hide );

    const Style style = d->sheet->style( QRect( d->column, 1, 1, KS_rowMax ) );
    if ( !style.isEmpty() )
    {
        kDebug(36003) << "saving cell style of column " << d->column << endl;
        QDomElement format;
        style.saveXML( doc, format, false, copy );
        col.appendChild( format );
    }

    return col;
}

bool ColumnFormat::load( const KoXmlElement & col, int xshift, Paste::Mode sp, bool paste )
{
    bool ok;
    if ( col.hasAttribute( "width" ) )
    {
        if ( d->sheet->doc()->syntaxVersion() < 1 ) //combatibility to old format - was in millimeter
            d->width = qRound( MM_TO_POINT ( col.attribute( "width" ).toDouble( &ok ) ) );
        else
            d->width = col.attribute( "width" ).toDouble( &ok );

        if ( !ok )
            return false;
    }

    d->column = col.attribute( "column" ).toInt( &ok ) + xshift;

    if ( !ok )
        return false;

    // Validation
    if ( d->width < 0 )
    {
        kDebug(36001) << "Value width=" << d->width << " out of range" << endl;
        return false;
    }
    if ( d->column < 1 || d->column > KS_colMax )
    {
        kDebug(36001) << "Value col=" << d->column << " out of range" << endl;
        return false;
    }
    if ( col.hasAttribute( "hide" ) )
    {
        setHidden( (int) col.attribute( "hide" ).toInt( &ok ) );
        if ( !ok )
            return false;
    }

    KoXmlElement f( col.namedItem( "format" ).toElement() );

    if ( !f.isNull() && ( sp == Paste::Normal || sp == Paste::Format || sp == Paste::NoBorder ))
    {
        Style style;
        if ( !style.loadXML( f, sp, paste ) )
            return false;
        d->sheet->setStyle( Region(QRect( d->column, 1, 1, KS_rowMax )), style );
        return true;
    }

    return true;
}

int ColumnFormat::column() const
{
    return d->column;
}

void ColumnFormat::setColumn( int column )
{
    d->column = column;
}

ColumnFormat* ColumnFormat::next() const
{
    return d->next;
}

ColumnFormat* ColumnFormat::previous() const
{
    return d->prev;
}

void ColumnFormat::setNext( ColumnFormat* next )
{
    d->next = next;
}

void ColumnFormat::setPrevious( ColumnFormat* prev )
{
    d->prev = prev;
}

void ColumnFormat::setHidden( bool _hide )
{
    if ( _hide != d->hide ) // only if we change the status
    {
        if ( _hide )
        {
	    // Lower maximum size by width of column
            d->sheet->adjustSizeMaxX ( - dblWidth() );
            d->hide = _hide; //hide must be set after we requested the width
          //  d->sheet->emit_updateColumn( this, d->column );
        }
        else
        {
	    // Rise maximum size by width of column
            d->hide = _hide; //unhide must be set before we request the width
            d->sheet->adjustSizeMaxX ( dblWidth() );
         //   d->sheet->emit_updateColumn( this, d->column );
        }
    }
}

bool ColumnFormat::hidden() const
{
    return d->hide;
}

bool ColumnFormat::isDefault() const
{
    if ( d->width != s_columnWidth )
        return false;
    if ( d->hide == true )
        return false;
    return true;
}

bool ColumnFormat::operator==( const ColumnFormat& other ) const
{
    // NOTE Stefan: Don't compare sheet and cell.
    if ( d->width != other.d->width )
        return false;
    if ( d->hide != other.d->hide )
        return false;
    return true;
}
