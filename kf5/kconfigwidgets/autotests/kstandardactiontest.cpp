/*
    Copyright 2007 Simon Hausmann <hausmann@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kstandardactiontest.h"

#include <QAction>
#include <QtTestWidgets>

#include "kstandardaction.h"

void tst_KStandardAction::shortcutForActionId()
{
    QList<QKeySequence> stdShortcut = KStandardShortcut::shortcut(KStandardShortcut::Cut);

    QAction *cut = KStandardAction::cut(NULL);
    QList<QKeySequence> actShortcut = cut->shortcuts();
    QVERIFY(stdShortcut == actShortcut);
    delete cut;

    cut = KStandardAction::create(KStandardAction::Cut, NULL, NULL, NULL);
    actShortcut = cut->shortcuts();
    QVERIFY(stdShortcut == actShortcut);
    delete cut;
}

QTEST_MAIN(tst_KStandardAction)
