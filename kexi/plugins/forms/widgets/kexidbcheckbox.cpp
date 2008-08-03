/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexidbcheckbox.h"

#include <kexiutils/utils.h>
#include <kexidb/queryschema.h>

KexiDBCheckBox::KexiDBCheckBox(const QString &text, QWidget *parent)
        : QCheckBox(text, parent), KexiFormDataItemInterface()
        , m_invalidState(false)
        , m_tristateChanged(false)
        , m_tristate(TristateDefault)
{
    setFocusPolicy(Qt::StrongFocus);
    updateTristate();
    connect(this, SIGNAL(stateChanged(int)), this, SLOT(slotStateChanged(int)));
}

KexiDBCheckBox::~KexiDBCheckBox()
{
}

void KexiDBCheckBox::setInvalidState(const QString& displayText)
{
    setEnabled(false);
    setCheckState(Qt::PartiallyChecked);
    m_invalidState = true;
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
    if (focusPolicy() & Qt::TabFocus)
        setFocusPolicy(Qt::ClickFocus);
    setText(displayText);
}

void
KexiDBCheckBox::setEnabled(bool enabled)
{
    if (enabled && m_invalidState)
        return;
    QCheckBox::setEnabled(enabled);
}

void KexiDBCheckBox::setReadOnly(bool readOnly)
{
    setEnabled(!readOnly);
}

void KexiDBCheckBox::setValueInternal(const QVariant &add, bool removeOld)
{
    Q_UNUSED(add);
    Q_UNUSED(removeOld);
    if (isTristateInternal())
        setCheckState(m_origValue.isNull()
                      ? Qt::PartiallyChecked : (m_origValue.toBool() ? Qt::Checked : Qt::Unchecked));
    else
        setCheckState(m_origValue.toBool() ? Qt::Checked : Qt::Unchecked);
}

QVariant KexiDBCheckBox::value()
{
    if (state() == NoChange)
        return QVariant();
    return QVariant(state() == On, 1);
}

void KexiDBCheckBox::slotStateChanged(int)
{
    signalValueChanged();
}

bool KexiDBCheckBox::valueIsNull()
{
    return state() == NoChange;
}

bool KexiDBCheckBox::valueIsEmpty()
{
    return false;
}

bool KexiDBCheckBox::isReadOnly() const
{
    return !isEnabled();
}

QWidget*
KexiDBCheckBox::widget()
{
    return this;
}

bool KexiDBCheckBox::cursorAtStart()
{
    return false; //! \todo ?
}

bool KexiDBCheckBox::cursorAtEnd()
{
    return false; //! \todo ?
}

void KexiDBCheckBox::clear()
{
    setCheckState(Qt::PartiallyChecked);
}

void KexiDBCheckBox::setTristate(KexiDBCheckBox::Tristate tristate)
{
    m_tristateChanged = true;
    m_tristate = tristate;
    updateTristate();
}

KexiDBCheckBox::Tristate KexiDBCheckBox::isTristate() const
{
    return m_tristate;
}

bool KexiDBCheckBox::isTristateInternal() const
{
    if (m_tristate == TristateDefault)
        return !dataSource().isEmpty();

    return m_tristate == TristateOn;
}

void KexiDBCheckBox::updateTristate()
{
    if (m_tristate == TristateDefault) {
//! @todo the data source may be defined as NOT NULL... thus disallowing NULL state
        QCheckBox::setTristate(!dataSource().isEmpty());
    } else {
        QCheckBox::setTristate(m_tristate == TristateOn);
    }
}

void KexiDBCheckBox::setDataSource(const QString &ds)
{
    KexiFormDataItemInterface::setDataSource(ds);
    updateTristate();
}

void KexiDBCheckBox::setDisplayDefaultValue(QWidget *widget, bool displayDefaultValue)
{
    KexiFormDataItemInterface::setDisplayDefaultValue(widget, displayDefaultValue);
    // initialize display parameters for default / entered value
    KexiDisplayUtils::DisplayParameters * const params
    = displayDefaultValue ? m_displayParametersForDefaultValue : m_displayParametersForEnteredValue;
// setFont(params->font);
    QPalette pal(palette());
// pal.setColor(QPalette::Active, QColorGroup::Text, params->textColor);
    pal.setColor(QPalette::Active, QColorGroup::Foreground, params->textColor);
    setPalette(pal);
}

#include "kexidbcheckbox.moc"
