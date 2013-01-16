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

#include "kexidbtimeedit.h"

#include <QToolButton>
#include <QLayout>
#include <QPainter>

#include <kmenu.h>
#include <kdatepicker.h>
#include <kdatetable.h>
#include <kexiutils/utils.h>

KexiDBTimeEdit::KexiDBTimeEdit(const QTime &time, QWidget *parent)
        : Q3TimeEdit(time, parent), KexiFormDataItemInterface()
{
    m_invalidState = false;
    setAutoAdvance(true);
    m_cleared = false;

#ifdef QDateTimeEditor_HACK
    m_dte_time = KexiUtils::findFirstChild<QDateTimeEditor>(this, "QDateTimeEditor");
#else
    m_dte_time = 0;
#endif

    connect(this, SIGNAL(valueChanged(const QTime&)), this, SLOT(slotValueChanged(const QTime&)));
}

KexiDBTimeEdit::~KexiDBTimeEdit()
{
}

void KexiDBTimeEdit::setInvalidState(const QString&)
{
    setEnabled(false);
    setReadOnly(true);
    m_invalidState = true;
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
    if (focusPolicy() & Qt::TabFocus)
        setFocusPolicy(Qt::ClickFocus);
}

void
KexiDBTimeEdit::setEnabled(bool enabled)
{
    // prevent the user from reenabling the widget when it is in invalid state
    if (enabled && m_invalidState)
        return;
    Q3TimeEdit::setEnabled(enabled);
}

void KexiDBTimeEdit::setValueInternal(const QVariant &add, bool removeOld)
{
    m_cleared = !m_origValue.isValid();

    int setNumberOnFocus = -1;
    QTime t;
    QString addString(add.toString());
    if (removeOld) {
        if (!addString.isEmpty() && addString[0].toLatin1() >= '0' && addString[0].toLatin1() <= '9') {
            setNumberOnFocus = addString[0].toLatin1() - '0';
            t = QTime(setNumberOnFocus, 0, 0);
        }
    } else
        t = m_origValue.toTime();

    setTime(t);
}

QVariant
KexiDBTimeEdit::value()
{
    //QDateTime - a hack needed because QVariant(QTime) has broken isNull()
    return QVariant(QDateTime(m_cleared ? QDate() : QDate(0, 1, 2)/*nevermind*/, time()));
}

bool KexiDBTimeEdit::valueIsNull()
{
    return !time().isValid() || time().isNull();
}

bool KexiDBTimeEdit::valueIsEmpty()
{
    return m_cleared;
}

bool KexiDBTimeEdit::isReadOnly() const
{
    //! @todo: data/time edit API has no readonly flag,
    //!        so use event filter to avoid changes made by keyboard or mouse when m_readOnly==true
    return m_readOnly; //!isEnabled();
}

void KexiDBTimeEdit::setReadOnly(bool set)
{
    m_readOnly = set;
}

QWidget*
KexiDBTimeEdit::widget()
{
    return this;
}

bool KexiDBTimeEdit::cursorAtStart()
{
#ifdef QDateTimeEditor_HACK
    return m_dte_time && hasFocus() && m_dte_time->focusSection() == 0;
#else
    return false;
#endif
}

bool KexiDBTimeEdit::cursorAtEnd()
{
#ifdef QDateTimeEditor_HACK
    return m_dte_time && hasFocus()
           && m_dte_time->focusSection() == int(m_dte_time->sectionCount() - 1);
#else
    return false;
#endif
}

void KexiDBTimeEdit::clear()
{
    setTime(QTime());
    m_cleared = true;
}

void
KexiDBTimeEdit::slotValueChanged(const QTime&)
{
    m_cleared = false;
}

#include "kexidbtimeedit.moc"
