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

#include "kexidbcheckbox.h"

#include <kexiutils/utils.h>
#include <kexidb/queryschema.h>

KexiDBCheckBox::KexiDBCheckBox(const QString &text, QWidget *parent, const char *name)
 : QCheckBox(text, parent, name), KexiFormDataItemInterface()
{
	m_invalidState = false;
//! todo: tristate 
	setTristate(true);
	setFocusPolicy(QWidget::StrongFocus);
	connect(this, SIGNAL(stateChanged(int)), this, SLOT(slotStateChanged(int)));
}

KexiDBCheckBox::~KexiDBCheckBox()
{
}

void KexiDBCheckBox::setInvalidState( const QString& displayText )
{
	setEnabled(false);
	setState(NoChange);
	m_invalidState = true;
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
	if (focusPolicy() & TabFocus)
		setFocusPolicy(QWidget::ClickFocus);
	setText(displayText);
}

void
KexiDBCheckBox::setEnabled(bool enabled)
{
	if(enabled && m_invalidState)
		return;
	QCheckBox::setEnabled(enabled);
}

void
KexiDBCheckBox::setReadOnly(bool readOnly)
{
	setEnabled(!readOnly);
}

void KexiDBCheckBox::setValueInternal(const QVariant &add, bool )
{
	Q_UNUSED(add);
//	setState( add.isNull() ? NoChange : (add.toBool() ? On : Off) );
	setState( m_origValue.isNull() ? NoChange : (m_origValue.toBool() ? On : Off) );
}

QVariant
KexiDBCheckBox::value()
{
	if (state()==NoChange)
		return QVariant();
	return QVariant(state()==On, 1);
}

void KexiDBCheckBox::slotStateChanged(int )
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
	setState(NoChange);
}

#include "kexidbcheckbox.moc"
