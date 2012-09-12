/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003-2004,2006 Jarosław Staniek <staniek@kde.org>

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

#include "kexidatetimetableedit.h"

#include <QApplication>
#include <QPainter>
#include <QVariant>
#include <QRect>
#include <QPalette>
#include <QColor>
#include <QFontMetrics>
#include <QDateTime>
#include <QCursor>
#include <QPoint>
#include <QLayout>
#include <QToolButton>
#include <QClipboard>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdatepicker.h>
#include <kdatetable.h>
#include <klineedit.h>
#include <kmenu.h>
#include <kdatewidget.h>

#include <kexiutils/utils.h>

KexiDateTimeTableEdit::KexiDateTimeTableEdit(KexiTableViewColumn &column, QWidget *parent)
        : KexiInputTableEdit(column, parent)
{
    setObjectName("KexiDateTimeTableEdit");

//! @todo add QValidator so time like "99:88:77" cannot be even entered

    kDebug() << KexiDateTimeFormatter::inputMask(m_dateFormatter, m_timeFormatter);
    m_lineedit->setInputMask(
        KexiDateTimeFormatter::inputMask(m_dateFormatter, m_timeFormatter));
}

KexiDateTimeTableEdit::~KexiDateTimeTableEdit()
{
}

void KexiDateTimeTableEdit::setValueInInternalEditor(const QVariant &value)
{
    m_lineedit->setText(
        KexiDateTimeFormatter::toString(m_dateFormatter, m_timeFormatter, value.toDateTime()));
}

void KexiDateTimeTableEdit::setValueInternal(const QVariant& add_, bool removeOld)
{
    if (removeOld) {
        //new time entering... just fill the line edit
//! @todo cut string if too long..
        QString add(add_.toString());
        m_lineedit->setText(add);
        m_lineedit->setCursorPosition(add.length());
        return;
    }
    setValueInInternalEditor(m_origValue);
    m_lineedit->setCursorPosition(0); //ok?
}

void KexiDateTimeTableEdit::setupContents(QPainter *p, bool focused, const QVariant& val,
        QString &txt, int &align, int &x, int &y_offset, int &w, int &h)
{
    Q_UNUSED(p);
    Q_UNUSED(focused);
    Q_UNUSED(x);
    Q_UNUSED(w);
    Q_UNUSED(h);
#ifdef Q_WS_WIN
    y_offset = -1;
#else
    y_offset = 0;
#endif
    txt = KexiDateTimeFormatter::toString(m_dateFormatter, m_timeFormatter, val.toDateTime());
    align |= Qt::AlignLeft;
}

bool KexiDateTimeTableEdit::valueIsNull()
{
    if (textIsEmpty())
        return true;
    return !KexiDateTimeFormatter::isValid(
               m_dateFormatter, m_timeFormatter, m_lineedit->text());
}

bool KexiDateTimeTableEdit::valueIsEmpty()
{
    return valueIsNull();//js OK? TODO (nonsense?)
}

QVariant KexiDateTimeTableEdit::value()
{
    if (textIsEmpty())
        return QVariant();
    return KexiDateTimeFormatter::fromString(
               m_dateFormatter, m_timeFormatter, m_lineedit->text());
}

bool KexiDateTimeTableEdit::valueIsValid()
{
    return KexiDateTimeFormatter::isValid(m_dateFormatter, m_timeFormatter, m_lineedit->text());
}

bool KexiDateTimeTableEdit::textIsEmpty() const
{
    return KexiDateTimeFormatter::isEmpty(m_dateFormatter, m_timeFormatter, m_lineedit->text());
}

void KexiDateTimeTableEdit::handleCopyAction(const QVariant& value, const QVariant& visibleValue)
{
    Q_UNUSED(visibleValue);
    qApp->clipboard()->setText(
        KexiDateTimeFormatter::toString(m_dateFormatter, m_timeFormatter, value.toDateTime()));
}

void KexiDateTimeTableEdit::handleAction(const QString& actionName)
{
    const bool alreadyVisible = m_lineedit->isVisible();

    if (actionName == "edit_paste") {
        const QVariant newValue(KexiDateTimeFormatter::fromString(
            m_dateFormatter, m_timeFormatter, qApp->clipboard()->text()));
        if (!alreadyVisible) { //paste as the entire text if the cell was not in edit mode
            emit editRequested();
            m_lineedit->clear();
        }
        setValueInInternalEditor(newValue);
    } else
        KexiInputTableEdit::handleAction(actionName);
}

KEXI_CELLEDITOR_FACTORY_ITEM_IMPL(KexiDateTimeEditorFactoryItem, KexiDateTimeTableEdit)

#include "kexidatetimetableedit.moc"
