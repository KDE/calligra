/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CELL_ACTION
#define CALLIGRA_SHEETS_CELL_ACTION

#include "sheets_ui_export.h"

#include <KoIcon.h>

#include <QObject>
#include <QString>

class QAction;

namespace Calligra
{
namespace Sheets
{
class Sheet;
class CellToolBase;
class Actions;
class Selection;

/**
 * A cell (or cell group) action. This class encapsulates one action (QAction) and handles triggering and selection management.
 */

class CALLIGRA_SHEETS_UI_EXPORT CellAction : public QObject
{
    Q_OBJECT

public:
    CellAction(Actions *actions, const QString &actionName, const QString &caption, const QIcon &icon, const QString &tooltip);
    virtual ~CellAction();

    QAction *action() { return m_action; }
    QString name() { return m_name; }
protected Q_SLOTS:
    void triggered();
protected:
    virtual void execute(Selection *selection, Sheet *sheet) = 0;

    Actions *m_actions;
    CellToolBase *m_tool;
    QString m_name;
    QAction *m_action;
};

} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_CELL_ACTION
