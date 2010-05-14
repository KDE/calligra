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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_ROW_COLUMN_FORMAT
#define KSPREAD_ROW_COLUMN_FORMAT

#include <QBrush>

#include "kspread_export.h"
#include <KoXmlReader.h>

#include "Global.h"
#include "Style.h"

class QDomElement;
class QDomDocument;
class KoGenStyle;

namespace KSpread
{
class Sheet;

/**
 * A row style.
 */
class KSPREAD_EXPORT RowFormat
{
public:
    RowFormat();
    RowFormat(const RowFormat& other);
    ~RowFormat();

    void setSheet(Sheet* sheet);

    QDomElement save(QDomDocument&, int yshift = 0) const;
    bool load(const KoXmlElement& row, int yshift = 0, Paste::Mode mode = Paste::Normal);
    bool loadOdf(const KoXmlElement& row, KoXmlElement * rowStyle);

    /**
     * \return the row's height
     */
    double height() const;

    /**
     * The visible row height, respecting hiding and filtering attributes.
     * \return the visible row height
     */
    double visibleHeight() const;

    /**
     * Sets the height to _h zoomed pixels.
     *
     * @param _h is calculated in display pixels as double value. The function cares for zooming.
     * Use this function when setting the height, to not get rounding problems.
     */
    void setHeight(double _h);

    /**
     * @reimp
     */
    bool isDefault() const;

    /**
     * @return the row for this RowFormat. May be 0 if this is the default format.
     */
    int row() const;
    void setRow(int row);

    RowFormat* next() const;
    RowFormat* previous() const;
    void setNext(RowFormat* c);
    void setPrevious(RowFormat* c);

    /**
     * Sets the hide flag
     */
    void setHidden(bool _hide, bool repaint = true);
    bool isHidden() const;

    void setFiltered(bool filtered);
    bool isFiltered() const;

    void setPageBreak(bool pageBreak);
    bool isPageBreakSet();

    bool isHiddenOrFiltered() const;

    bool operator==(const RowFormat& other) const;
    inline bool operator!=(const RowFormat& other) const {
        return !operator==(other);
    }

private:
    // do not allow assignment
    RowFormat& operator=(const RowFormat&);

    class Private;
    Private * const d;
};

/**
 * A column style.
 */
class KSPREAD_EXPORT ColumnFormat
{
public:
    ColumnFormat();
    ColumnFormat(const ColumnFormat& other);
    ~ColumnFormat();

    void setSheet(Sheet* sheet);

    QDomElement save(QDomDocument&, int xshift = 0) const;
    bool load(const KoXmlElement& row, int xshift = 0, Paste::Mode mode = Paste::Normal);

    /**
     * \return the column's width
     */
    double width() const;

    /**
     * The visible column height, respecting hiding and filtering attributes.
     * \return the visible column width
     */
    double visibleWidth() const;

    /**
     * Sets the width to _w zoomed pixels as double value.
     * Use this function to set the width without getting rounding problems.
     *
     * @param _w is calculated in display pixels. The function cares for
     *           zooming.
     */
    void setWidth(double _w);

    /**
     * @reimp
     */
    bool isDefault() const;

    /**
     * @return the column of this ColumnFormat. May be 0 if this is the default format.
     */
    int column() const;
    void setColumn(int column);

    ColumnFormat* next() const;
    ColumnFormat* previous() const;
    void setNext(ColumnFormat* c);
    void setPrevious(ColumnFormat* c);

    void setHidden(bool _hide);
    bool isHidden() const;

    void setFiltered(bool filtered);
    bool isFiltered() const;

    bool isHiddenOrFiltered() const;

    void setPageBreak(bool pageBreak);
    bool isPageBreakSet();

    bool operator==(const ColumnFormat& other) const;
    inline bool operator!=(const ColumnFormat& other) const {
        return !operator==(other);
    }

private:
    // do not allow assignment
    ColumnFormat& operator=(const ColumnFormat&);

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_ROW_COLUMN_FORMAT
