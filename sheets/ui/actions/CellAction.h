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
class KToggleAction;

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
    QString name()
    {
        return m_name;
    }
    bool shouldBeEnabled(bool readWrite, Selection *selection, const Cell &activeCell);
    virtual bool shouldBeChecked(Selection *, const Cell &)
    {
        return false;
    }
    virtual void updateOnChange(Selection *, const Cell & /*activeCell*/)
    {
    }
    virtual void onEditorDeleted()
    {
    }
    virtual void trigger();
    /** For actions that need access to the cell tool, which isn't yet constructed fully in the constructor. */
    virtual void init(){};
protected Q_SLOTS:
    void triggered();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) = 0;

    virtual bool enabledIfReadOnly() const
    {
        return false;
    }
    virtual bool enabledIfProtected() const
    {
        return false;
    }
    virtual bool enabledForSelection(Selection *selection, const Cell &activeCell);

    QRect shrinkToUsedArea(QRect rect, Sheet *sheet);
    QRect extendSelectionToColumn(const CellBase &cell, bool numeric);
    QRect extendSelectionToRow(const CellBase &cell, bool numeric);
    QRect extendSelectionToRange(const CellBase &cell, bool numeric);

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
    virtual void trigger() override;
protected Q_SLOTS:

    void triggered(bool enabled);

protected:
    /** Empty implementation of the original execute. Do not override this one, use the below one instead. */
    virtual void execute(Selection * /*selection*/, Sheet * /*sheet*/, QWidget * /*canvasWidget*/) override final
    {
    }
    virtual void executeToggled(bool enabled, Selection *selection, Sheet *sheet, QWidget *canvasWidget) = 0;

    virtual QAction *createAction() override;

    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell);

    KToggleAction *m_toggleAction;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CELL_ACTION
