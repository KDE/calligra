/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>

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
 * Boston, MA 02110-1301, USA
*/

#ifndef SWINDER_CELL_H
#define SWINDER_CELL_H

#include "format.h"
#include "value.h"

#include <vector>
#include <QtGlobal>
#include <QList>
#include <QString>

namespace Swinder
{

class Workbook;
class Sheet;
class PictureObject;
class ChartObject;
class OfficeArtObject;

struct Hyperlink
{
    Hyperlink() : isValid(false) {}
    Hyperlink(const QString& displayName, const QString& location, const QString& targetFrameName) : isValid(true), displayName(displayName), location(location), targetFrameName(targetFrameName) {}
    bool operator==(const Hyperlink& b) {
        if (!isValid && !b.isValid) return true;
        if (!isValid || !b.isValid) return false;
        return displayName == b.displayName && location == b.location && targetFrameName == b.targetFrameName;
    }
    bool operator!=(const Hyperlink& b) { return !operator==(b); }

    bool isValid;
    QString displayName;
    QString location;
    QString targetFrameName;
};
}
Q_DECLARE_TYPEINFO(Swinder::Hyperlink, Q_MOVABLE_TYPE);
namespace Swinder
{
class Cell
{
public:

    Cell(Sheet* sheet, unsigned column, unsigned row);
    virtual ~Cell();

    // Returns the sheet this cell is located in.
    Sheet* sheet();

    // Returns the column this cell is in.
    unsigned column() const;
    // Returns the row this cell is in.
    unsigned row() const;

    // Returns the name this cell is in. This could be for example
    // the name D3 for a cell that is in column 4 and row 3.
    QString name() const;
    static QString name(unsigned column, unsigned row);

    // Retuns the column label. As example the column 4 has the label D.
    QString columnLabel() const;
    static QString columnLabel(unsigned column);

    // Retuns the value this cell has.
    Value value() const;
    void setValue(const Value& value);

    // Returns the formula of this cell. May an empty string if this cell has no formula.
    QString formula() const;
    void setFormula(const QString& formula);

    // Returns the format of this cell.
    const Format& format() const;
    void setFormat(const Format* format);

    // Returns the optional column span.
    unsigned columnSpan() const;
    void setColumnSpan(unsigned span);

    // Returns the optional row span.
    unsigned rowSpan() const;
    void setRowSpan(unsigned span);

    // Returns if this cell does cover the optional spans.
    bool isCovered() const;
    void setCovered(bool covered);

    // Returns the cell-repeat number. The cell-repeat defines how often this cell will be
    // repeated. Per default this will return 1 what means it will only repeat its own cell.
    // If the is bigger then the defined number of following cells will be ignored/covered.
    int columnRepeat() const;
    void setColumnRepeat(int repeat);
    
    // Defines if this cell has a hyperlink.
    bool hasHyperlink() const;
    Hyperlink hyperlink() const;
    void setHyperlink(const Hyperlink& link);

    // Returns the optional note/comment/annotation of this cell.
    QString note() const;
    void setNote(const QString &n);
    
    // Defines a list of pictures anchored to this cell.
    QList<PictureObject*> pictures() const;
    void setPictures(const QList<PictureObject*>&);
    void addPicture(PictureObject*);

    // Defines a list of charts anchored to this cell.
    QList<ChartObject*> charts() const;
    void addChart(ChartObject* chart);

    // Defines a list of OfficeArt-objects anchored to this cell.
    QList<OfficeArtObject*> officeArts() const;
    void addOfficeArt(OfficeArtObject* officeart);
    
    bool operator==(const Cell &other) const;
    bool operator!=(const Cell &other) const;

private:
    // no copy or assign
    Cell(const Cell&);
    Cell& operator=(const Cell&);

    class Private;
    Private* d;
};

} // namespace Swinder


#endif // SWINDER_CELL_H

