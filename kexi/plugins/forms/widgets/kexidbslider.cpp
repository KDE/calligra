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
#include "kexidbslider.h"
#include <knumvalidator.h>

KexiDBSlider::KexiDBSlider(QWidget *parent)
    : KexiSlider(parent) , KexiFormDataItemInterface()
{
    setOrientation(Qt::Horizontal);
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
}

KexiDBSlider::~KexiDBSlider()
{
}

void KexiDBSlider::setInvalidState(const QString& displayText)
{
    Q_UNUSED(displayText);
    m_invalidState = true;
    setEnabled(false);
    setReadOnly(true);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
    if (focusPolicy() & Qt::TabFocus)
        setFocusPolicy(Qt::ClickFocus);
    KexiSlider::setValue(minimum());
}

void
KexiDBSlider::setEnabled(bool enabled)
{
    // prevent the user from reenabling the widget when it is in invalid state
    if (enabled && m_invalidState)
        return;
    KexiSlider::setEnabled(enabled);
}

void KexiDBSlider::setValueInternal(const QVariant&, bool)
{
    KexiSlider::setValue(m_origValue.toInt());
}

QVariant
KexiDBSlider::value()
{
    return KexiSlider::value();
}

void KexiDBSlider::slotValueChanged()
{
    signalValueChanged();
}

bool KexiDBSlider::valueIsNull()
{
    return false;
}

bool KexiDBSlider::valueIsEmpty()
{
    return false;
}

bool KexiDBSlider::isReadOnly() const
{
    return !isEnabled();
}

void KexiDBSlider::setReadOnly(bool set)
{
    setEnabled( !set );
}

QWidget*
KexiDBSlider::widget()
{
    return this;
}

bool KexiDBSlider::cursorAtStart()
{
    return false; //! \todo ?
}

bool KexiDBSlider::cursorAtEnd()
{
    return false; //! \todo ?
}

void KexiDBSlider::clear()
{
    KexiSlider::setValue(minimum());
}

#include "kexidbslider.moc"
