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

#include "kexidbdatetimeedit.h"

#include <qtoolbutton.h>
#include <qlayout.h>
#include <QKeyEvent>
#include <QEvent>
#include <QHBoxLayout>
#include <kmenu.h>
#include <kdatepicker.h>
#include <kdatetable.h>
#include <kexiutils/utils.h>

KexiDBDateTimeEdit::KexiDBDateTimeEdit(const QDateTime &datetime, QWidget *parent)
        : QWidget(parent), KexiFormDataItemInterface()
{
    m_invalidState = false;
    m_cleared = false;
    m_readOnly = false;

    m_dateEdit = new Q3DateEdit(datetime.date(), this);
    m_dateEdit->setAutoAdvance(true);
    m_dateEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
// m_dateEdit->setFixedWidth( QFontMetrics(m_dateEdit->font()).width("8888-88-88___") );
    connect(m_dateEdit, SIGNAL(valueChanged(const QDate&)), this, SLOT(slotValueChanged()));
    connect(m_dateEdit, SIGNAL(valueChanged(const QDate&)), this, SIGNAL(dateTimeChanged()));

    QToolButton* btn = new QToolButton(this);
    btn->setText("...");
    btn->setFixedWidth(QFontMetrics(btn->font()).width(" ... "));
    btn->setPopupDelay(1); //1 ms

    m_timeEdit = new Q3TimeEdit(datetime.time(), this);;
    m_timeEdit->setAutoAdvance(true);
    m_timeEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    connect(m_timeEdit, SIGNAL(valueChanged(const QTime&)), this, SLOT(slotValueChanged()));
    connect(m_timeEdit, SIGNAL(valueChanged(const QTime&)), this, SIGNAL(dateTimeChanged()));

#ifdef QDateTimeEditor_HACK
    m_dte_date = KexiUtils::findFirstChild<QDateTimeEditor>(m_dateEdit, "QDateTimeEditor");
    m_dte_time = KexiUtils::findFirstChild<QDateTimeEditor>(m_timeEdit, "QDateTimeEditor");
#else
    m_dte_date = 0;
#endif

    m_datePickerPopupMenu = new KMenu(0, "date_popup");
    connect(m_datePickerPopupMenu, SIGNAL(aboutToShow()), this, SLOT(slotShowDatePicker()));
    m_datePicker = new KDatePicker(m_datePickerPopupMenu, QDate::currentDate(), 0);

    KDateTable *dt = KexiUtils::findFirstChild<KDateTable>(m_datePicker, "KDateTable");
    if (dt)
        connect(dt, SIGNAL(tableClicked()), this, SLOT(acceptDate()));
    m_datePicker->setCloseButton(true);
    m_datePicker->installEventFilter(this);
    m_datePickerPopupMenu->insertItem(m_datePicker);
    btn->setPopup(m_datePickerPopupMenu);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(m_dateEdit, 0);
    layout->addWidget(btn, 0);
    layout->addWidget(m_timeEdit, 0);
    //layout->addStretch(1);

    setFocusProxy(m_dateEdit);
}

KexiDBDateTimeEdit::~KexiDBDateTimeEdit()
{
}

void KexiDBDateTimeEdit::setInvalidState(const QString & /*! @todo paint this text: text*/)
{
    setEnabled(false);
    setReadOnly(true);
    m_invalidState = true;
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
    if (focusPolicy() & Qt::TabFocus)
        setFocusPolicy(Qt::ClickFocus);
}

void
KexiDBDateTimeEdit::setEnabled(bool enabled)
{
    // prevent the user from reenabling the widget when it is in invalid state
    if (enabled && m_invalidState)
        return;
    QWidget::setEnabled(enabled);
}

void KexiDBDateTimeEdit::setValueInternal(const QVariant &, bool)
{
    m_dateEdit->setDate(m_origValue.toDate());
    m_timeEdit->setTime(m_origValue.toTime());
}

QVariant
KexiDBDateTimeEdit::value()
{
    return QDateTime(m_dateEdit->date(), m_timeEdit->time());
}

bool KexiDBDateTimeEdit::valueIsNull()
{
    return !m_dateEdit->date().isValid() || m_dateEdit->date().isNull()
           || !m_timeEdit->time().isValid() || m_timeEdit->time().isNull();
}

bool KexiDBDateTimeEdit::valueIsEmpty()
{
    return m_cleared;
}

bool KexiDBDateTimeEdit::isReadOnly() const
{
    //! @todo: data/time edit API has no readonly flag,
    //!        so use event filter to avoid changes made by keyboard or mouse when m_readOnly==true
    return m_readOnly; //!isEnabled();
}

void KexiDBDateTimeEdit::setReadOnly(bool set)
{
    m_readOnly = set;
}

QWidget*
KexiDBDateTimeEdit::widget()
{
    return m_dateEdit;
}

bool KexiDBDateTimeEdit::cursorAtStart()
{
#ifdef QDateTimeEditor_HACK
    return m_dte_date && m_dateEdit->hasFocus() && m_dte_date->focusSection() == 0;
#else
    return false;
#endif
}

bool KexiDBDateTimeEdit::cursorAtEnd()
{
#ifdef QDateTimeEditor_HACK
    return m_dte_time && m_timeEdit->hasFocus()
           && m_dte_time->focusSection() == int(m_dte_time->sectionCount() - 1);
#else
    return false;
#endif
}

void KexiDBDateTimeEdit::clear()
{
    m_dateEdit->setDate(QDate());
    m_timeEdit->setTime(QTime());
    m_cleared = true;
}

void
KexiDBDateTimeEdit::slotValueChanged()
{
    m_cleared = false;
}

void
KexiDBDateTimeEdit::slotShowDatePicker()
{
    QDate date = m_dateEdit->date();

    m_datePicker->setDate(date);
    m_datePicker->setFocus();
    m_datePicker->show();
    m_datePicker->setFocus();
}

void
KexiDBDateTimeEdit::acceptDate()
{
    m_dateEdit->setDate(m_datePicker->date());
    m_datePickerPopupMenu->hide();
}

bool
KexiDBDateTimeEdit::eventFilter(QObject *o, QEvent *e)
{
    if (o != m_datePicker)
        return false;

    switch (e->type()) {
    case QEvent::Hide:
        m_datePickerPopupMenu->hide();
        break;
    case QEvent::KeyPress:
    case QEvent::KeyRelease: {
        QKeyEvent *ke = (QKeyEvent *)e;
        if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return) {
            //accepting picker
            acceptDate();
            return true;
        } else if (ke->key() == Qt::Key_Escape) {
            //canceling picker
            m_datePickerPopupMenu->hide();
            return true;
        } else
            m_datePickerPopupMenu->setFocus();
        break;
    }
    default:
        break;
    }
    return false;
}

QDateTime
KexiDBDateTimeEdit::dateTime() const
{
    return QDateTime(m_dateEdit->date(), m_timeEdit->time());
}

void
KexiDBDateTimeEdit::setDateTime(const QDateTime &dt)
{
    m_dateEdit->setDate(dt.date());
    m_timeEdit->setTime(dt.time());
}

#include "kexidbdatetimeedit.moc"
