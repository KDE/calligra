/* This file is part of the KDE project

   Copyright 1999-2006 The KSpread Team <calligra-devel@kde.org>

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


#ifndef CALLIGRA_SHEETS_FUNCTION_COMPLETION
#define CALLIGRA_SHEETS_FUNCTION_COMPLETION

#include <QObject>

#include "sheets_common_export.h"

class QListWidgetItem;

namespace Calligra
{
namespace Sheets
{
class CellEditor;

/**
* Provides autocompletion facilities in formula editors.
* When the user types in the first few characters of a
* function name in a CellEditor which has a FunctionCompletion
* object installed on it, the FunctionCompletion object
* creates and displays a list of possible names which the user
* can select from. If the user selects a function name from the list,
* the @ref FunctionCompletion::selectedCompletion() signal is emitted
*/
class FunctionCompletion : public QObject
{
    Q_OBJECT

public:

    explicit FunctionCompletion(CellEditor *editor);
    ~FunctionCompletion() override;

    /**
    * Handles various keyboard and mouse actions which may occur on the autocompletion popup list
    */
    bool eventFilter(QObject *o, QEvent *e) override;

    /**
    * Populates the autocompletion list box with the specified choices and shows it so that the user can view and select a function name.
    * @param choices A list of possible function names which match the characters that the user has already entered.
    */
    void showCompletion(const QStringList &choices);

public Q_SLOTS:
    /**
    * Hides the autocompletion list box if it is visible and emits the @ref selectedCompletion signal.
    */
    void doneCompletion();

private Q_SLOTS:
    void itemSelected(QListWidgetItem* item = 0);

Q_SIGNALS:
    /**
    * Emitted, if the user selects a function name from the list.
    */
    void selectedCompletion(const QString& item);

private:
    class Private;
    Private * const d;
    FunctionCompletion(const FunctionCompletion&);
    FunctionCompletion& operator=(const FunctionCompletion&);
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FUNCTION_COMPLETION
