/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_ADJUST_ROW_COL
#define CALLIGRA_SHEETS_ACTION_ADJUST_ROW_COL

#include "CellAction.h"

namespace Calligra
{
namespace Sheets
{
class ShowColRowDialog;
class ResizeColumnDialog;
class ResizeRowDialog;

class InsertRemoveRowCol : public CellAction
{
    Q_OBJECT
public:
    InsertRemoveRowCol(Actions *actions, bool insert, bool row);
    virtual ~InsertRemoveRowCol();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool enabledForSelection(Selection *selection, const Cell &activeCell) override;
    virtual QAction *createAction() override;

    bool m_insert, m_row;
};

class ShowHideRowCol : public CellAction
{
    Q_OBJECT
public:
    ShowHideRowCol(Actions *actions, bool show, bool row);
    virtual ~ShowHideRowCol();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool enabledForSelection(Selection *selection, const Cell &activeCell) override;
    virtual QAction *createAction() override;

    bool m_show, m_row;
};

class ShowRowColQuery : public CellAction
{
    Q_OBJECT
public:
    ShowRowColQuery(Actions *actions, bool row);
    virtual ~ShowRowColQuery();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;

    bool m_row;
    ShowColRowDialog *m_dlg;
};

class EqualizeRowCol : public CellAction
{
    Q_OBJECT
public:
    EqualizeRowCol(Actions *actions, bool row);
    virtual ~EqualizeRowCol();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool enabledForSelection(Selection *selection, const Cell &activeCell) override;
    virtual QAction *createAction() override;

    bool m_row;
};

class AdjustRowCol : public CellAction
{
    Q_OBJECT
public:
    AdjustRowCol(Actions *actions, bool row, bool col);
    virtual ~AdjustRowCol();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual QAction *createAction() override;

    bool m_row, m_col;
};

class ResizeRowCol : public CellAction
{
    Q_OBJECT
public:
    ResizeRowCol(Actions *actions, bool row);
    virtual ~ResizeRowCol();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool enabledForSelection(Selection *selection, const Cell &activeCell) override;

    bool m_row;
    ResizeColumnDialog *m_dlgCol;
    ResizeRowDialog *m_dlgRow;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_ADJUST_ROW_COL
