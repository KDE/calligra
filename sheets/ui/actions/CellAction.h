/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CELL_ACTION
#define CALLIGRA_SHEETS_CELL_ACTION

#include "sheets_ui_export.h"

// we don't need these here, but all the children will be using them, so easier to define them here
#include "core/Cell.h"
#include "ui/Selection.h"

#include <KoIcon.h>

#include <QObject>
#include <QString>

class QAction;
class QWidget;

namespace Calligra
{
namespace Sheets
{
class Sheet;
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

    QAction *action();
    QString name() { return m_name; }
    bool shouldBeEnabled(bool readWrite, Selection *selection, const Cell &activeCell);
    virtual bool shouldBeChecked(Selection *, const Cell &) { return false; }
    void trigger();
protected Q_SLOTS:
    void triggered();
protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) = 0;

    virtual bool enabledIfReadOnly() const { return false; }
    virtual bool enabledIfProtected() const { return false; }
    virtual bool enabledForSelection(Selection *selection, const Cell &activeCell);

    /** Creates the QAction or a derived class. */
    virtual QAction *createAction();

    Actions *m_actions;
    QString m_name, m_caption, m_tooltip;
    QIcon m_icon;
    QAction *m_action;
    /** Should we close the editor when triggering this action? */
    bool m_closeEditor;
};

class CALLIGRA_SHEETS_UI_EXPORT ToggleableCellAction : public CellAction
{
    Q_OBJECT

public:
    ToggleableCellAction(Actions *actions, const QString &actionName, const QString &caption, const QIcon &icon, const QString &tooltip);
    virtual ~ToggleableCellAction();

    virtual bool shouldBeChecked(Selection *selection, const Cell &activeCell) override;
protected Q_SLOTS:

    void triggered(bool enabled);

protected:
    /** Empty implementation of the original execute. Do not override this one, use the below one instead. */
    virtual void execute(Selection * /*selection*/, Sheet * /*sheet*/, QWidget * /*canvasWidget*/) override final {}
    virtual void executeToggled(bool enabled, Selection *selection, Sheet *sheet, QWidget *canvasWidget) = 0;

    virtual QAction *createAction() override;

    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell);
};


} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_CELL_ACTION
