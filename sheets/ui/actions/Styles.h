/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_STYLES
#define CALLIGRA_SHEETS_ACTION_STYLES


#include "CellAction.h"

class KSelectAction;


namespace Calligra
{
namespace Sheets
{

class StyleFromCell : public CellAction {
Q_OBJECT
public:
    StyleFromCell(Actions *actions);
    virtual ~StyleFromCell();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;
    virtual bool enabledForSelection(Selection *selection, const Cell &activeCell) override;
};


class StyleManagerDialog;

class ShowStyleManager : public CellAction {
Q_OBJECT
public:
    ShowStyleManager(Actions *actions);
    virtual ~ShowStyleManager();

protected Q_SLOTS:
    void styleDialogClosed();
    void setStyle(const QString &style);

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;

    StyleManagerDialog *m_dlg;
};


class SetStyle : public CellAction {
Q_OBJECT
public:
    SetStyle(Actions *actions);
    virtual ~SetStyle();

    void init() override;

protected Q_SLOTS:
    void triggeredSelect(const QString &name);
    void fillStyles();
protected:
    virtual void execute(Selection *, Sheet *, QWidget *) override {}  // never called
    QAction *createAction() override;

    KSelectAction *m_selectAction;
};


} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_ACTION_STYLES
