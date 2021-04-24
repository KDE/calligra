// This file is part of KSpread
// SPDX-FileCopyrightText: 2007 Sebastian Sauer <mail@dipe.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef SCRIPTINGLISTENER_H
#define SCRIPTINGLISTENER_H

#include <QRect>
#include <QVariant>
#include <QObject>

namespace Calligra
{
namespace Sheets
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
    explicit ScriptingCellListener(Calligra::Sheets::Sheet *sheet, const QRect& area);
    ~ScriptingCellListener() override;

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

} // namespace Sheets
} // namespace Calligra

#endif
