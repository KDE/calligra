/*
 * This file is part of KSpread
 *
 * Copyright (c) 2007 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTINGLISTENER_H
#define SCRIPTINGLISTENER_H

#include <QRect>
#include <QVariant>
#include <QObject>

namespace KSpread
{

class Sheet;
class Region;

/**
* The ScriptingCellListener class implements a listener to changes
* within cells of a sheet.
*
* Python sample script that creates a listener for the range of cells
* A1:F50 within the sheet Sheet1.
* \code
* import Kross, KSpread
* listener = KSpread.createListener("Sheet1", "A1:F50")
* if not listener:
*     raise "Failed to create listener."
* def regionChanged(regions):
*     print "regions=%s" % regions
* def cellChanged(column, row):
*     print "column=%i row=%i" % (column,row)
* listener.connect("regionChanged(QVariantList)", regionChanged)
* listener.connect("cellChanged(int,int)", cellChanged)
* \endcode
*/
class ScriptingCellListener : public QObject
{
    Q_OBJECT
public:
    explicit ScriptingCellListener(KSpread::Sheet *sheet, const QRect& area);
    virtual ~ScriptingCellListener();

Q_SIGNALS:

    /**
    * This signal got emitted if the content of a region of cells changed.
    */
    void regionChanged(const QVariantList& ranges);

    /**
    * This signal got emitted if the content of a cell changed.
    */
    void cellChanged(int column, int row);

private Q_SLOTS:
    void slotChanged(const Region&);

private:
    Q_DISABLE_COPY(ScriptingCellListener)

    /// \internal d-pointer class.
    class Private;
    /// \internal d-pointer instance.
    Private* const d;
};

}

#endif
