/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KexiSearchLineEdit.h"

#include <KLocale>

#include <QCompleter>
#include <QShortcut>
#include <QKeySequence>

class KexiSearchLineEdit::Private
{
public:
    Private(KexiSearchLineEdit *_q)
     : q(_q), clearShortcut(QKeySequence(Qt::Key_Escape), _q)
    {
        // make Escape key clear the search box
        QObject::connect(&clearShortcut, SIGNAL(activated()),
                         q, SLOT(clear()));
    }
private:
    KexiSearchLineEdit *q;
    QShortcut clearShortcut; 
};

KexiSearchLineEdit::KexiSearchLineEdit(QWidget *parent)
 : KLineEdit(parent), d(new Private(this))
{
    //setCompletionModeDisabled
    //setCompletionMode(KGlobalSettings::CompletionPopup);
    QStringList wordList;
/*tmp*/ wordList << "alpha" << "omega" << "omicron" << "zeta";
    QCompleter *completer = new QCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setMaxVisibleItems(12);
    setCompleter(completer);
    setFocusPolicy(Qt::ClickFocus);
    setClearButtonShown(true);
    setClickMessage(i18n("Search"));
}

KexiSearchLineEdit::~KexiSearchLineEdit()
{
    delete d;
}
