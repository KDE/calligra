/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2012 Oleg Kukharchuk <oleg.kuh@gmail.com>

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
#include "kexidbprogressbar.h"


KexiDBProgressBar::KexiDBProgressBar(QWidget *parent)
        :QProgressBar(parent), KexiFormDataItemInterface()
{
}

KexiDBProgressBar::~KexiDBProgressBar()
{
}

void KexiDBProgressBar::setInvalidState(const QString& displayText)
{
    Q_UNUSED(displayText);
    m_invalidState = true;
    setEnabled(false);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
    if (focusPolicy() & Qt::TabFocus)
        setFocusPolicy(Qt::ClickFocus);
    QProgressBar::setValue(minimum());
}

void
KexiDBProgressBar::setEnabled(bool enabled)
{
    // prevent the user from reenabling the widget when it is in invalid state
    if (enabled && m_invalidState)
        return;
    QProgressBar::setEnabled(enabled);
}

void KexiDBProgressBar::setValueInternal(const QVariant&, bool)
{
    QProgressBar::setValue(m_origValue.toInt());
}

QVariant
KexiDBProgressBar::value()
{
    return QProgressBar::value();
}

void KexiDBProgressBar::slotValueChanged()
{
    // read only widget
}

bool KexiDBProgressBar::valueIsNull()
{
    return false;
}

bool KexiDBProgressBar::valueIsEmpty()
{
    return false;
}

bool KexiDBProgressBar::isReadOnly() const
{
    return true;
}

void KexiDBProgressBar::setReadOnly(bool set)
{
    Q_UNUSED(set);
}

QWidget*
KexiDBProgressBar::widget()
{
    return this;
}

bool KexiDBProgressBar::cursorAtStart()
{
    return false; 
}

bool KexiDBProgressBar::cursorAtEnd()
{
    return false;
}

void KexiDBProgressBar::clear()
{
    //read only widget
}

#include "kexidbprogressbar.moc"
