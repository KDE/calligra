/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexidbintspinbox.h"

#include <qlineedit.h>
#include <knumvalidator.h>

KexiDBIntSpinBox::KexiDBIntSpinBox(QWidget *parent)
        : KIntSpinBox(parent) , KexiFormDataItemInterface()
{
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
}

KexiDBIntSpinBox::~KexiDBIntSpinBox()
{
}

void KexiDBIntSpinBox::setInvalidState(const QString& displayText)
{
    m_invalidState = true;
    setEnabled(false);
    setReadOnly(true);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
    if (focusPolicy() & Qt::TabFocus)
        setFocusPolicy(Qt::ClickFocus);
    setSpecialValueText(displayText);
    KIntSpinBox::setValue(minValue());
}

void
KexiDBIntSpinBox::setEnabled(bool enabled)
{
    // prevent the user from reenabling the widget when it is in invalid state
    if (enabled && m_invalidState)
        return;
    KIntSpinBox::setEnabled(enabled);
}

void KexiDBIntSpinBox::setValueInternal(const QVariant&, bool)
{
    KIntSpinBox::setValue(m_origValue.toInt());
}

QVariant
KexiDBIntSpinBox::value()
{
    return KIntSpinBox::value();
}

void KexiDBIntSpinBox::slotValueChanged()
{
    signalValueChanged();
}

bool KexiDBIntSpinBox::valueIsNull()
{
    return cleanText().isEmpty();
}

bool KexiDBIntSpinBox::valueIsEmpty()
{
    return false;
}

bool KexiDBIntSpinBox::isReadOnly() const
{
    return editor()->isReadOnly();
}

void KexiDBIntSpinBox::setReadOnly(bool set)
{
    editor()->setReadOnly(set);
}

QWidget*
KexiDBIntSpinBox::widget()
{
    return this;
}

bool KexiDBIntSpinBox::cursorAtStart()
{
    return false; //! \todo ?
}

bool KexiDBIntSpinBox::cursorAtEnd()
{
    return false; //! \todo ?
}

void KexiDBIntSpinBox::clear()
{
    KIntSpinBox::setValue(minValue()); //! \todo ?
}

#include "kexidbintspinbox.moc"
