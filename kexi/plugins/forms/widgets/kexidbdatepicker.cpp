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

#include "kexidbdatepicker.h"
#include "klineedit.h"
#include <QInputMethodEvent>

KexiDBDatePicker::KexiDBDatePicker(QWidget *parent)
        : KDatePicker(parent)
        , KexiFormDataItemInterface()
        , m_dateEditFocused(false)
{
    connect(this, SIGNAL(dateChanged(QDate)), this, SLOT(slotValueChanged()));
    connect(this, SIGNAL(dateEntered(QDate)), this, SLOT(slotValueChanged()));
}

KexiDBDatePicker::~KexiDBDatePicker()
{
}

void KexiDBDatePicker::setInvalidState(const QString& displayText)
{
    m_invalidState = true;
    setEnabled(false);
    setReadOnly(true);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
    if (focusPolicy() & Qt::TabFocus)
        setFocusPolicy(Qt::ClickFocus);
    KDatePicker::setDate(QDate());
}

void KexiDBDatePicker::setEnabled(bool enabled)
{
    // prevent the user from reenabling the widget when it is in invalid state
    if (enabled && m_invalidState)
        return;
    KDatePicker::setEnabled(enabled);
}

void KexiDBDatePicker::setValueInternal(const QVariant&, bool)
{
    KDatePicker::setDate(m_origValue.toDate());
}

QVariant KexiDBDatePicker::value()
{
    return KDatePicker::date();
}

void KexiDBDatePicker::slotValueChanged()
{
    signalValueChanged();
}

bool KexiDBDatePicker::valueIsNull()
{
    return !KDatePicker::date().isValid();
}

bool KexiDBDatePicker::valueIsEmpty()
{
    return !KDatePicker::date().isValid();
}

bool KexiDBDatePicker::isReadOnly() const
{
    return m_readOnly;
}

void KexiDBDatePicker::setReadOnly(bool set)
{
    m_readOnly = set;
}

QWidget* KexiDBDatePicker::widget()
{
    return this;
}

bool KexiDBDatePicker::cursorAtStart()
{
    const KLineEdit *lineEdit=findChild<KLineEdit*>();
    return lineEdit && lineEdit->hasFocus() && lineEdit->cursorPosition() == 0;
}

bool KexiDBDatePicker::cursorAtEnd()
{
    const KLineEdit *lineEdit=findChild<KLineEdit*>();
    return lineEdit && lineEdit->hasFocus() && lineEdit->cursorPosition() == lineEdit->text().length();


void KexiDBDatePicker::clear()
{
    KDatePicker::setDate(QDate());
}

#include "kexidbdatepicker.moc"
