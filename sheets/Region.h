/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   Copyright 2005-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef CALLIGRA_SHEETS_REGION
#define CALLIGRA_SHEETS_REGION

#include <QList>
#include <QRect>
#include <QSet>
#include <QSharedDataPointer>
#include <QString>

#include "SheetsDebug.h"
#include "sheets_odf_export.h"

inline uint qHash(const QPoint& point)
{
    return (static_cast<uint>(point.x()) << 16) + static_cast<uint>(point.y());
}

namespace Calligra
{
namespace Sheets
{
class Cell;
class Map;
class Sheet;

/**
 * \class Region
 * \brief The one for all class for points and ranges.
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * \since 1.5
 */
class CALLIGRA_SHEETS_ODF_EXPORT Region
{
public:
    class Element;
    class Point;
    class Range;

    /**
     * Constructor.
     * Creates an empty region.
     */
    Region();

    /**
     * Constructor.
     * Creates a region consisting of a point.
     * @param point the point's location
     * @param sheet the sheet the point belongs to
     */
    explicit Region(const QPoint& point, Sheet* sheet = 0);

    /**
     * Constructor.
     * Creates a region consisting of a range.
     * @param range the range's location
     * @param sheet the sheet the range belongs to
     */
    explicit Region(const QRect& range, Sheet* sheet = 0);

    /**
     * Constructor.
     * Creates a region consisting of the region defined in @p expression .
     * @param expression a string representing the region (e.g. "A1:B3")
     * @param map used to determine the sheet, if it's named in the string
     * @param sheet the fallback sheet, if \p expression does not contain one
     */
    explicit Region(const QString& expression, const Map* map = 0, Sheet* sheet = 0);

    /**
     * Copy Constructor.
     * Creates a copy of the region.
     * @param region the region to copy
     */
    Region(const Region& region);

    /**
     * Constructor.
     * Creates a region consisting of a point.
     * @param col the column of the point
     * @param row the row of the point
     * @param sheet the sheet the point belongs to
     */
    Region(int col, int row, Sheet* sheet = 0);

    /**
     * Constructor.
     * Creates a region consisting of a range at the location
     * @param col the column of the range' starting point
     * @param row the row of the range' starting point
     * @param width the width of the range
     * @param height the height of the range
     * @param sheet the sheet the range belongs to
     */
    Region(int col, int row, int width, int height, Sheet* sheet = 0);

    /**
     * Destructor.
     */
    virtual ~Region();


    /**
     *  @return a QRegion that unifies all contained ranges
     */
    QVector<QRect> rects() const;

    /**
     * @param originSheet The name is created relative to this sheet.
     * @return the name of the region (e.g. "A1:A2")
     */
    QString name(Sheet* originSheet = 0) const;

    /**
     * @param sRegion will be modified, if a valid sheet was found. The sheetname
     * will be removed
     * @return sheet named in the @p sRegion or null
     */
    Sheet* filterSheetName(QString& sRegion);



    /**
     * @return @c true, if this region contains no elements
     */
    bool isEmpty() const;

    /**
     * @return @c true, if this region contains only a single point
     */
    bool isSingular() const;

    /**
     * @return @c true, if this region is contiguous
     */
    bool isContiguous() const;

    /**
     * @return @c true, if this region contains at least one valid point or one valid range
     */
    bool isValid() const;

    /**
     * @param col The column to check
     *
     * @return @c True, if the column @p col is selected. If column @p col
     * is not given, it returns true, if at least one column is selected
     *
     * \note If you want to check more than one column for selection, use
     * columnsSelected(). It returns a set of all selected columns at once.
     */
    bool isColumnSelected(uint col = 0) const;

    /**
     * @param row the row to check
     *
     * @return @c true , if the row @p row is selected. If row @p row
     * is not given, it returns true, if at least one row is selected
     *
     * \note If you want to check more than one row for selection, use
     * rowsSelected(). It returns a set of all selected rows at once.
     */
    bool isRowSelected(uint row = 0) const;

    /**
     * @return @c true , if at least one column or one row is selected
     */
    bool isColumnOrRowSelected() const;

    /**
     * @return @c true , if all cells in the sheet are selected
     */
    bool isAllSelected() const;

    /**
     * @return a set of column numbers, for those columns, that are selected
     */
    QSet<int> columnsSelected() const;

    /**
     * @return a set of row numbers, for those rows, that are selected
     */
    QSet<int> rowsSelected() const;

    /**
     * @return a set of column numbers, for those columns, that have at least
     * one cell selected
     */
    QSet<int> columnsAffected() const;

    /**
     * @return a set of row numbers, for those rows, that have at least
     * one cell selected
     */
    QSet<int> rowsAffected() const;

    /**
     * @param point the point's location
     * @param sheet the sheet the point belongs to
     * @return @c true, if the region contains the point @p point
     */
    bool contains(const QPoint& point, Sheet* sheet = 0) const;



    /* TODO Stefan #2: Optimize! Adjacent Points/Ranges */
    /**
     * Adds the point @p point to this region.
     * @param point the point's location
     * @param sheet the sheet the point belongs to
     */
    Element* add(const QPoint& point, Sheet* sheet = 0);

    /**
     * Adds the range @p range to this region.
     * @param range the range's location
     * @param sheet the sheet the range belongs to
     */
    Element* add(const QRect& range, Sheet* sheet = 0);

    /**
     * Adds the region @p region to this region.
     * @param region the region to be added
     * @param sheet the fallback sheet used, if an element has no sheet set
     */
    Element* add(const Region& region, Sheet* sheet = 0);

    /* TODO Stefan #3: Improve! */
    /**
     * Subtracts the point @p point from this region.
     * @param point the point's location
     * @param sheet the sheet the point belongs to
     */
    void sub(const QPoint& point, Sheet* sheet);

    /**
     * Subtracts the range @p range from this region.
     * @param range the range's location
     * @param sheet the sheet the range belongs to
     */
    void sub(const QRect& range, Sheet* sheet);

    /**
     * Subtracts the region @p region from this region.
     * @param region the region to subtract
     */
    void sub(const Region& region);

    /**
     * Intersects the region @p region and this region and
     * returns the result of the intersection as a new Region.
     */
    Region intersected(const Region& region) const;

    /**
     * Intersects this region with the row @p row and returns
     * the result of the intersection as a new Region.
     */
    Region intersectedWithRow(int row) const;

    /**
     * @param point the point's location
     * @param sheet the sheet the point belongs to
     */
    virtual Element* eor(const QPoint& point, Sheet* sheet = 0);

    /**
     * Deletes all elements of the region. The result is an empty region.
     */
    virtual void clear();


    QRect firstRange() const;
    QRect lastRange() const;
    Sheet* firstSheet() const;
    Sheet* lastSheet() const;

    QRect boundingRect() const;


    static QRect normalized(const QRect& rect);


    /**
     * @param region the region to compare
     * @return @c true, if this region equals region @p region
     */
    bool operator==(const Region& region) const;
    inline bool operator!=(const Region& region) const {
        return !operator==(region);
    }

    /**
     * @param region the region to copy
     */
    void operator=(const Region& region);



    /**
     * @return the map to which this region belongs.
     */
    const Map* map() const;

    /**
     * Sets the map to which this region belongs.
     */
    void setMap(const Map*);


    typedef QList<Element*>::Iterator      Iterator;
    typedef QList<Element*>::ConstIterator ConstIterator;

    ConstIterator constBegin() const;
    ConstIterator constEnd() const;

    static bool isValid(const QPoint& point);
    static bool isValid(const QRect& rect);

protected:
    /**
     * @return the list of elements
     */
    QList<Element*>& cells() const;

    /**
     * @param index the index of the element in whose front the new point
     * is inserted
     * @param point the location of the point to be inserted
     * @param sheet the sheet the point belongs to
     * @param multi @c true to allow multiple occurrences of a point
     * @return the added point, a null pointer, if @p point is not
     * valid or the element containing @p point
     */
    Element* insert(int index, const QPoint& point, Sheet* sheet, bool multi = true);

    /**
     * @param index the index of the element in whose front the new range
     * is inserted
     * @param range the location of the range to be inserted
     * @param sheet the sheet the range belongs to
     * @param multi @c true to allow multiple occurrences of a range
     * @return the added range, a null pointer, if @p range is not
     * valid or the element containing @p range
     */
    Element* insert(int index, const QRect& range, Sheet* sheet, bool multi = true);

    /**
     * @internal used to create derived Points
     */
    virtual Point* createPoint(const QPoint&) const;

    /**
     * @internal used to create derived Points
     */
    virtual Point* createPoint(const QString&) const;

    /**
     * @internal used to create derived Points
     */
    virtual Point* createPoint(const Point&) const;

    /**
     * @internal used to create derived Ranges
     */
    virtual Range* createRange(const QRect&) const;

    /**
     * @internal used to create derived Ranges
     */
    virtual Range* createRange(const Point&, const Point&) const;

    /**
     * @internal used to create derived Ranges
     */
    virtual Range* createRange(const QString&) const;

    /**
     * @internal used to create derived Ranges
     */
    virtual Range* createRange(const Range&) const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};


/***************************************************************************
  class Region::Element
****************************************************************************/
/**
 * Base class for region elements, which can be points or ranges.
 * This class is used by Calligra::Sheets::Region and could not be used outside of it.
 *
 * Size:
 * m_sheet: 4 bytes
 * vtable: 4 bytes
 * sum: 8 bytes
 */
class CALLIGRA_SHEETS_ODF_EXPORT Region::Element
{
public:
    enum Type { Undefined, Point, Range };

    Element();
    virtual ~Element();

    virtual Type type() const {
        return Undefined;
    }
    virtual bool isValid() const {
        return false;
    }
    virtual bool isColumn() const {
        return false;
    }
    virtual bool isRow() const {
        return false;
    }
    virtual bool isAll() const {
        return false;
    }

    virtual bool contains(const QPoint&) const {
        return false;
    }
    virtual bool contains(const QRect&) const {
        return false;
    }

    virtual QString name(Sheet* = 0) const {
        return QString("");
    }
    virtual QRect rect() const {
        return QRect();
    }

    virtual bool isColumnFixed() const {
        return false;
    }
    virtual bool isRowFixed() const {
        return false;
    }
    virtual bool isTopFixed() const {
        return false;
    }
    virtual bool isLeftFixed() const {
        return false;
    }
    virtual bool isBottomFixed() const {
        return false;
    }
    virtual bool isRightFixed() const {
        return false;
    }

    Sheet* sheet() const {
        return m_sheet;
    }
    void setSheet(Sheet* sheet) {
        m_sheet = sheet;
    }

protected:
    /* TODO Stefan #6:
        Elaborate, if this pointer could be avoided by QDict or whatever in
        Region.
    */
    Sheet* m_sheet;
};


/***************************************************************************
  class Region::Point
****************************************************************************/

/**
 * A point in a region.
 * This class is used by Calligra::Sheets::Region and could not be used outside of it.
 *
 * Size:
 * m_sheet: 4 bytes
 * vtable: 4 bytes
 * m_point: 8 bytes
 * sum: 16 bytes
 */
class CALLIGRA_SHEETS_ODF_EXPORT Region::Point : public Region::Element
{
public:
    Point() : Element(), m_point() {}
    Point(int col, int row) : Element(), m_point(col, row) {}
    Point(const QPoint&);
    Point(const QString&);
    ~Point() override;

    Type type() const override {
        return Element::Point;
    }
    bool isValid() const override {
        return (!m_point.isNull() && Region::isValid(m_point));
    }
    bool isColumn() const override {
        return false;
    }
    bool isRow() const override {
        return false;
    }
    bool isAll() const override {
        return false;
    }

    bool contains(const QPoint&) const override;
    bool contains(const QRect&) const override;

    QString name(Sheet* originSheet = 0) const override;

    QRect rect() const override {
        return QRect(m_point, m_point);
    }

    bool isColumnFixed() const override {
        return m_fixedColumn;
    }
    bool isRowFixed() const override {
        return m_fixedRow;
    }
    bool isTopFixed() const override {
        return m_fixedRow;
    }
    bool isLeftFixed() const override {
        return m_fixedColumn;
    }
    bool isBottomFixed() const override {
        return m_fixedRow;
    }
    bool isRightFixed() const override {
        return m_fixedColumn;
    }

    QPoint pos() const {
        return m_point;
    }
    Cell cell() const;

    bool operator==(const Point& other) const {
        return ((m_point == other.m_point) && (m_sheet == other.m_sheet));
    }

private:
    QPoint m_point;
    bool m_fixedColumn;
    bool m_fixedRow;
};


/***************************************************************************
  class Region:.Range
****************************************************************************/

/**
 * A range in a region.
 * This class is used by Calligra::Sheets::Region and could not be used outside of it.
 *
 * Size:
 * m_sheet: 4 bytes
 * vtable: 4 bytes
 * m_range: 16 bytes
 * sum: 24 bytes
 */
class CALLIGRA_SHEETS_ODF_EXPORT Region::Range : public Region::Element
{
public:
    Range(const QRect&);
    Range(const Region::Point&, const Region::Point&);
    Range(const QString&);
    ~Range() override;

    Type type() const override {
        return Element::Range;
    }
    bool isValid() const override {
        return !m_range.isNull() && Region::isValid(m_range);
    }
    bool isColumn() const override;
    bool isRow() const override;
    bool isAll() const override;

    bool contains(const QPoint&) const override;
    bool contains(const QRect&) const override;

    QString name(Sheet* originSheet = 0) const override;

    QRect rect() const override {
        return m_range;
    }

    bool isColumnFixed() const override {
        return m_fixedLeft && m_fixedRight;
    }
    bool isRowFixed() const override {
        return m_fixedTop && m_fixedBottom;
    }
    bool isTopFixed() const override {
        return m_fixedTop;
    }
    bool isLeftFixed() const override {
        return m_fixedLeft;
    }
    bool isBottomFixed() const override {
        return m_fixedBottom;
    }
    bool isRightFixed() const override {
        return m_fixedRight;
    }

    int width() const;
    int height() const;

private:
    QRect m_range;
    bool m_fixedTop;
    bool m_fixedLeft;
    bool m_fixedBottom;
    bool m_fixedRight;
};

} // namespace Sheets
} // namespace Calligra

Q_DECLARE_TYPEINFO(Calligra::Sheets::Region, Q_MOVABLE_TYPE);


/***************************************************************************
  QDebug support
****************************************************************************/

inline QDebug operator<<(QDebug str, const Calligra::Sheets::Region& r)
{
    return str << qPrintable(r.name());
}

#endif // CALLIGRA_SHEETS_REGION
