/* This file is part of the KDE project
 * Copyright (C) 2015 Yue Liu <yue.liu@mail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QFontDatabase>
#include <QToolBar>
#include <QToolButton>

#include <QDebug>
#include <klocalizedstring.h>
#include <QIcon>

#include "FormulaDebug.h"
#include "FormulaFontFamilyAction.h"

class FormulaFontFamilyAction::Private
{
    public:
        Private(FormulaFontFamilyAction *parent)
            : q(parent)
        {
            q->setEditable(true);
            QFontDatabase fontDB;
            QStringList availableFamilies = fontDB.families();
            QStringList fontFamilies = {"XITS Math", "Asana Math", "Neo Euler", "Latin Modern Math", "TeX Gyre Bonum Math", "TeX Gyre Pagella Math", "TeX Gyre Schola Math", "TeX Gyre Termes Math"};
            QMutableListIterator<QString> i(fontFamilies);
            while (i.hasNext()) {
                if (!availableFamilies.contains(i.next())) { i.remove(); }
            }
            debugFormula << fontFamilies;
            q->setItems(fontFamilies);
        }

        FormulaFontFamilyAction *q;
};

FormulaFontFamilyAction::FormulaFontFamilyAction(QObject *parent)
  : KSelectAction(parent),
    d(new Private(this))
{
}

FormulaFontFamilyAction::FormulaFontFamilyAction(const QString &text, QObject *parent)
  : KSelectAction(text, parent),
    d(new Private(this))
{
}

FormulaFontFamilyAction::FormulaFontFamilyAction(const QIcon &icon, const QString &text, QObject *parent)
  : KSelectAction(icon, text, parent),
    d(new Private(this))
{
}

FormulaFontFamilyAction::~FormulaFontFamilyAction()
{
  delete d;
}

QString FormulaFontFamilyAction::fontFamily() const
{
  return currentText();
}

void FormulaFontFamilyAction::actionTriggered(QAction* action)
{
    emit fontFamilyChanged(action->text());
    KSelectAction::actionTriggered( action );
}
