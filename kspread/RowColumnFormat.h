/* This file is part of the KDE project
   Copyright (C) 1998, 1999  Torben Weis <weis@kde.org>
   Copyright (C) 2000 - 2003 The KSpread Team <koffice-devel@kde.org>

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

#ifndef KSPREAD_ROW_COLUMN_FORMAT
#define KSPREAD_ROW_COLUMN_FORMAT

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QPen>

#include <koffice_export.h>
#include <KoXmlReader.h>

#include "Global.h"
#include "Style.h"

class QDomElement;
class QDomDocument;
class DCOPObject;
class KLocale;
class KoGenStyle;
class KoGenStyles;
class KoOasisLoadingContext;
class KoOasisStyles;
class KoStyleStack;

namespace KSpread
{
class Sheet;

/**
 * A row style.
 */
class KSPREAD_EXPORT RowFormat
{
public:
    RowFormat( Sheet* sheet, int row );
    ~RowFormat();

    QDomElement save( QDomDocument&, int yshift = 0, bool copy = false ) const;
    bool load( const KoXmlElement& row, int yshift = 0, Paste::Mode sp = Paste::Normal, bool paste = false );
    bool loadOasis( const KoXmlElement& row, KoXmlElement * rowStyle );

    /**
     * @return the height in zoomed pixels as integer value.
     * \deprecated
     */
    KDE_DEPRECATED int height() const;

    /**
     * @return the height in zoomed pixels as double value.
     * Use this function, if you want to work with height without having rounding problems.
     */
    double dblHeight() const;

    /**
     * @return the height in millimeters.
     * \deprecated
     */
    KDE_DEPRECATED double mmHeight() const;

    /**
     * Sets the height to _h zoomed pixels.
     *
     * @param _h is calculated in display pixels as integer value. The function cares for zooming.
     * \deprecated
     */
    KDE_DEPRECATED void setHeight( int _h );

    /**
     * Sets the height to _h zoomed pixels.
     *
     * @param _h is calculated in display pixels as double value. The function cares for zooming.
     * Use this function when setting the height, to not get rounding problems.
     */
    void setDblHeight( double _h );

    /**
     * Sets the height.
     *
     * @param '_h' is assumed to be a unzoomed millimeter value.
     * \deprecated
     */
    KDE_DEPRECATED void setMMHeight( double _h );

    /**
     * @reimp
     */
    bool isDefault() const;

    /**
     * @return the row for this RowFormat. May be 0 if this is the default format.
     */
    int row() const;
    void setRow( int row );

    RowFormat* next() const;
    RowFormat* previous() const;
    void setNext( RowFormat* c );
    void setPrevious( RowFormat* c );

    /**
     * Sets the hide flag
     */
    void setHidden( bool _hide, bool repaint = true );
    bool hidden() const;

    static void setGlobalRowHeight( double height ) { s_rowHeight = height; }
    static double globalRowHeight() { return s_rowHeight; }

    bool operator==( const RowFormat& other ) const;
    inline bool operator!=( const RowFormat& other ) const { return !operator==( other ); }

private:
    // default column width and row height
    static double s_rowHeight;

    class Private;
    Private * const d;
};

/**
 * A column style.
 */
class KSPREAD_EXPORT ColumnFormat
{
public:
    ColumnFormat( Sheet* sheet, int column );
    ~ColumnFormat();

    QDomElement save( QDomDocument&, int xshift = 0, bool copy = false ) const;
    bool load( const KoXmlElement& row, int xshift = 0,Paste::Mode sp = Paste::Normal, bool paste = false );

    /**
     * @return the width in zoomed pixels as integer.
     * \deprecated
     */
    KDE_DEPRECATED int width() const;

    /**
     * @return the width in zoomed pixels as double.
     * Use this function, if you want to use the width and later restore it back,
     * so you don't get rounding problems
     */
    double dblWidth() const;

    /**
     * @return the width in millimeters.
     * \deprecated
     */
    KDE_DEPRECATED double mmWidth() const;

    /**
     * Sets the width to _w zoomed pixels.
     *
     * @param _w is calculated in display pixels. The function cares for
     *           zooming.
     * \deprecated
     */
    KDE_DEPRECATED void setWidth( int _w );

    /**
     * Sets the width to _w zoomed pixels as double value.
     * Use this function to set the width without getting rounding problems.
     *
     * @param _w is calculated in display pixels. The function cares for
     *           zooming.
     */
    void setDblWidth( double _w );

    /**
     * Sets the width.
     *
     * @param _w is assumed to be a unzoomed millimeter value.
     * \deprecated
     */
    KDE_DEPRECATED void setMMWidth( double _w );

    /**
     * @reimp
     */
    bool isDefault() const;

    /**
     * @return the column of this ColumnFormat. May be 0 if this is the default format.
     */
    int column() const;
    void setColumn( int column );

    ColumnFormat* next() const;
    ColumnFormat* previous() const;
    void setNext( ColumnFormat* c );
    void setPrevious( ColumnFormat* c );

    void setHidden( bool _hide );
    bool hidden() const;

    static void setGlobalColWidth( double width ) { s_columnWidth = width; }
    static double globalColWidth() { return s_columnWidth; }

    bool operator==( const ColumnFormat& other ) const;
    inline bool operator!=( const ColumnFormat& other ) const { return !operator==( other ); }

private:
    // default column width
    static double s_columnWidth;

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_ROW_COLUMN_FORMAT
