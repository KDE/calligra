/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_FONT
#define CALLIGRA_SHEETS_ACTION_FONT

#include "CellAction.h"

#include "ui/commands/AbstractRegionCommand.h"

class KFontAction;
class KFontSizeAction;
class KoColorPopupAction;
class KoColor;

namespace Calligra
{
namespace Sheets
{

class Font : public CellAction
{
    Q_OBJECT
public:
    Font(Actions *actions);
    virtual ~Font();

    virtual void updateOnChange(Selection *selection, const Cell &activeCell) override;
protected Q_SLOTS:
    void triggeredFont(const QString &name);

protected:
    virtual void execute(Selection *, Sheet *, QWidget *) override
    {
    } // never called
    QAction *createAction() override;

    KFontAction *m_fontAction;
};

class FontSize : public CellAction
{
    Q_OBJECT
public:
    FontSize(Actions *actions);
    virtual ~FontSize();

    virtual void updateOnChange(Selection *selection, const Cell &activeCell) override;
protected Q_SLOTS:
    void triggeredSize(int size);

protected:
    virtual void execute(Selection *, Sheet *, QWidget *) override
    {
    } // never called
    QAction *createAction() override;

    KFontSizeAction *m_fontAction;
};

class FontColor : public CellAction
{
    Q_OBJECT
public:
    FontColor(Actions *actions);
    virtual ~FontColor();

    virtual void updateOnChange(Selection *selection, const Cell &activeCell) override;
protected Q_SLOTS:
    void triggeredColor(const KoColor &color);

protected:
    virtual void execute(Selection *, Sheet *, QWidget *) override
    {
    } // never called
    QAction *createAction() override;

    KoColorPopupAction *m_fontAction;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_FONT
