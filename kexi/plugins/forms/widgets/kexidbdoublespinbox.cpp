/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jarosław Staniek <staniek@kde.org>

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

#include "kexidbdoublespinbox.h"

#include <qlineedit.h>

KexiDBDoubleSpinBox::KexiDBDoubleSpinBox(QWidget *parent)
        : KDoubleSpinBox(parent) , KexiFormDataItemInterface()
{
    connect(this, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged()));
}

KexiDBDoubleSpinBox::~KexiDBDoubleSpinBox()
{
}

void KexiDBDoubleSpinBox::setInvalidState(const QString& displayText)
{
    m_invalidState = true;
    setEnabled(false);
    setReadOnly(true);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
    if (focusPolicy() & Qt::TabFocus)
        setFocusPolicy(Qt::ClickFocus);
    setSpecialValueText(displayText);
    KDoubleSpinBox::setValue(minimum());
}

void
KexiDBDoubleSpinBox::setEnabled(bool enabled)
{
    // prevent the user from reenabling the widget when it is in invalid state
    if (enabled && m_invalidState)
        return;
    KDoubleSpinBox::setEnabled(enabled);
}

void KexiDBDoubleSpinBox::setValueInternal(const QVariant&, bool)
{
    KDoubleSpinBox::setValue(m_origValue.toDouble());
}

QVariant
KexiDBDoubleSpinBox::value()
{
    return KDoubleSpinBox::value();
}

void KexiDBDoubleSpinBox::slotValueChanged()
{
    signalValueChanged();
}

bool KexiDBDoubleSpinBox::valueIsNull()
{
    return cleanText().isEmpty();
}

bool KexiDBDoubleSpinBox::valueIsEmpty()
{
    return false;
}

bool KexiDBDoubleSpinBox::isReadOnly() const
{
    return editor()->isReadOnly();
}

void KexiDBDoubleSpinBox::setReadOnly(bool set)
{
    editor()->setReadOnly(set);
}

QWidget*
KexiDBDoubleSpinBox::widget()
{
    return this;
}

bool KexiDBDoubleSpinBox::cursorAtStart()
{
    return false; //! \todo ?
}

bool KexiDBDoubleSpinBox::cursorAtEnd()
{
    return false; //! \todo ?
}

void KexiDBDoubleSpinBox::clear()
{
    KDoubleSpinBox::setValue(minValue());
}

#include "kexidbdoublespinbox.moc"
