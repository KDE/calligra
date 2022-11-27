/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_BORDER
#define CALLIGRA_SHEETS_ACTION_BORDER


#include "CellAction.h"


namespace Calligra
{
namespace Sheets
{

class BorderLeft : public CellAction {
Q_OBJECT
public:
    BorderLeft(Actions *actions);
    virtual ~BorderLeft();

protected:
    QAction *createAction() override;
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

class BorderRight : public CellAction {
Q_OBJECT
public:
    BorderRight(Actions *actions);
    virtual ~BorderRight();

protected:
    QAction *createAction() override;
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

class BorderTop : public CellAction {
Q_OBJECT
public:
    BorderTop(Actions *actions);
    virtual ~BorderTop();

protected:
    QAction *createAction() override;
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

class BorderBottom : public CellAction {
Q_OBJECT
public:
    BorderBottom(Actions *actions);
    virtual ~BorderBottom();

protected:
    QAction *createAction() override;
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

class BorderAll : public CellAction {
Q_OBJECT
public:
    BorderAll(Actions *actions);
    virtual ~BorderAll();

protected:
    QAction *createAction() override;
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

class BorderNone : public CellAction {
Q_OBJECT
public:
    BorderNone(Actions *actions);
    virtual ~BorderNone();

protected:
    QAction *createAction() override;
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

class BorderOutline : public CellAction {
Q_OBJECT
public:
    BorderOutline(Actions *actions);
    virtual ~BorderOutline();

protected:
    QAction *createAction() override;
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};



} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_ACTION_BORDER
