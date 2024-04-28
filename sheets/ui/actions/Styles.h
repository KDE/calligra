/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_STYLES
#define CALLIGRA_SHEETS_ACTION_STYLES

#include "DialogCellAction.h"

class KSelectAction;

namespace Calligra
{
namespace Sheets
{

class StyleFromCell : public CellAction
{
    Q_OBJECT
public:
    StyleFromCell(Actions *actions);
    virtual ~StyleFromCell();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;
    virtual bool enabledForSelection(Selection *selection, const Cell &activeCell) override;
};

class ShowStyleManager : public DialogCellAction
{
    Q_OBJECT
public:
    ShowStyleManager(Actions *actions);
    virtual ~ShowStyleManager();

protected Q_SLOTS:
    void setStyle(const QString &style);

protected:
    virtual ActionDialog *createDialog(QWidget *canvasWidget) override;
};

class SetStyle : public CellAction
{
    Q_OBJECT
public:
    SetStyle(Actions *actions);
    virtual ~SetStyle();

    void init() override;

protected Q_SLOTS:
    void triggeredSelect(const QString &name);
    void fillStyles();

protected:
    virtual void execute(Selection *, Sheet *, QWidget *) override
    {
    } // never called
    QAction *createAction() override;

    KSelectAction *m_selectAction;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_STYLES
