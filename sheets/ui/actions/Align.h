/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_ALIGN
#define CALLIGRA_SHEETS_ACTION_ALIGN

#include "CellAction.h"

namespace Calligra
{
namespace Sheets
{

class AlignLeft : public ToggleableCellAction
{
    Q_OBJECT
public:
    AlignLeft(Actions *actions);
    virtual ~AlignLeft();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class AlignRight : public ToggleableCellAction
{
    Q_OBJECT
public:
    AlignRight(Actions *actions);
    virtual ~AlignRight();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class AlignCenter : public ToggleableCellAction
{
    Q_OBJECT
public:
    AlignCenter(Actions *actions);
    virtual ~AlignCenter();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class AlignTop : public ToggleableCellAction
{
    Q_OBJECT
public:
    AlignTop(Actions *actions);
    virtual ~AlignTop();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class AlignBottom : public ToggleableCellAction
{
    Q_OBJECT
public:
    AlignBottom(Actions *actions);
    virtual ~AlignBottom();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class AlignMiddle : public ToggleableCellAction
{
    Q_OBJECT
public:
    AlignMiddle(Actions *actions);
    virtual ~AlignMiddle();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_ALIGN
