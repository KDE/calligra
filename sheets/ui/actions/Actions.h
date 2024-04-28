/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTIONS
#define CALLIGRA_SHEETS_ACTIONS

#include "sheets_ui_export.h"

#include <QMap>
#include <QObject>
#include <QString>

class QAction;

namespace Calligra
{
namespace Sheets
{
class Cell;
class Sheet;
class CellToolBase;
class CellAction;
class Selection;

/**
 * This class holds a list of all the existing actions, and can create them when needed.
 */

class CALLIGRA_SHEETS_UI_EXPORT Actions
{
public:
    Actions(CellToolBase *tool);
    ~Actions();

    /** For actions that need access to the cell tool, which isn't yet constructed fully in the constructor. */
    void init();
    CellToolBase *tool()
    {
        return m_tool;
    }
    void updateOnChange(bool readWrite, Selection *selection, const Cell &activeCell);
    void onEditorDeleted();
    CellAction *cellAction(const QString &name);
    QAction *action(const QString &name);

protected:
    void createActions();
    void addAction(CellAction *);

    CellToolBase *m_tool;
    QMap<QString, CellAction *> cellActions;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTIONS
