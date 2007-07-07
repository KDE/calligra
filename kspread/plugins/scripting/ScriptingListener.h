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
#include <QObject>

namespace KSpread {

    class Sheet;
    class Cell;

    /**
    * The ScriptingCellListener class implements a listener to changes
    * within cells of a sheet.
    *
    * Python sample script that creates a listener for the range of cells
    * A1:F50 within the sheet Sheet1.
    * \code
    * import Kross, KSpread
    * listener = KSpread.createListener("Sheet1", "A1:F50")
    * if not self.listener:
    *     raise "Failed to create listener."
    * def changed(column, row):
    *     print "column=%i row=%i" % (column,row)
    * listener.connect("changed(int,int)", changed)
    * \endcode
    */
    class ScriptingCellListener : public QObject
    {
            Q_OBJECT
        public:
            explicit ScriptingCellListener(KSpread::Sheet *sheet, const QRect& area);
            virtual ~ScriptingCellListener();

        Q_SIGNALS:
            void changed(int column, int row);

        private Q_SLOTS:
            void slotChanged(const Cell&);

        private:
            Q_DISABLE_COPY( ScriptingCellListener )

            /// \internal d-pointer class.
            class Private;
            /// \internal d-pointer instance.
            Private* const d;
    };

}

#endif
