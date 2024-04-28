/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_SPELLCHECK
#define CALLIGRA_SHEETS_ACTION_SPELLCHECK

#include "CellAction.h"
#include <sonnet/backgroundchecker.h>

namespace Calligra
{
namespace Sheets
{

class SpellChecker : public Sonnet::BackgroundChecker
{
    Q_OBJECT
public:
    SpellChecker(QObject *parent);
    ~SpellChecker();

    void check(Selection *selection, Sheet *sheet, QWidget *canvasWidget);

protected:
    QString fetchMoreText() override;
    void finishedCurrentFeed() override;

private Q_SLOTS:
    void finishCommand();

private:
    void advanceToNextCell();
    bool nextSheet();

    class Private;
    Private *d;
};

class SpellCheck : public CellAction
{
    Q_OBJECT
public:
    SpellCheck(Actions *actions);
    virtual ~SpellCheck();

    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;

private:
    SpellChecker *checker;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_SPELLCHECK
