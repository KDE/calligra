/* This file is part of the KDE project
   Copyright (C) 2002 Peter Simonsson <psn@linux.se>
   Copyright (C) 2003-2006 Jarosław Staniek <staniek@kde.org>

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

#include "kexitableedit.h"
#include <widget/dataviewcommon/kexidataawareobjectiface.h>
#include <db/field.h>
#include <db/utils.h>

#include <QPalette>
#include <QPainter>
#include <QKeyEvent>
#include <QEvent>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

#ifdef KEXI_MOBILE
KexiTableEdit::KexiTableEdit(KexiTableViewColumn &column, QWidget* parent)
        : QWidget(parent)
        , m_column(&column)
// ,m_field(&f)
// ,m_type(f.type()) //copied because the rest of code uses m_type
        , m_usesSelectedTextColor(true)
        , m_view(0)
// ,m_hasFocusableWidget(true)
// ,m_acceptEditorAfterDeleteContents(false)
#else
        KexiTableEdit::KexiTableEdit(KexiTableViewColumn &column, QWidget* parent)
        : QWidget(dynamic_cast<Q3ScrollView*>(parent) ? dynamic_cast<Q3ScrollView*>(parent)->viewport() : parent)
        , m_column(&column)
        // ,m_field(&f)
        // ,m_type(f.type()) //copied because the rest of code uses m_type
        , m_scrollView(dynamic_cast<Q3ScrollView*>(parent))
        , m_usesSelectedTextColor(true)
        , m_view(0)
        // ,m_hasFocusableWidget(true)
        // ,m_acceptEditorAfterDeleteContents(false)   
#endif
{
//    setPaletteBackgroundColor(palette().color(QPalette::Active, QColorGroup::Base));
    QPalette pal(palette());
    pal.setBrush(backgroundRole(), pal.brush(QPalette::Base));
    setPalette(pal);
    //installEventFilter(this);

    //margins
    if (displayedField()->isFPNumericType()) {
#ifdef Q_WS_WIN
        m_leftMargin = 0;
#else
        m_leftMargin = 0;
#endif
    } else if (displayedField()->isIntegerType()) {
#ifdef Q_WS_WIN
        m_leftMargin = 1;
#else
        m_leftMargin = 0;
#endif
    } else {//default
#ifdef Q_WS_WIN
        m_leftMargin = 5;
#else
        m_leftMargin = 5;
#endif
    }

    m_rightMargin = 0;
    m_rightMarginWhenFocused = 0;
}

KexiTableEdit::~KexiTableEdit()
{
}

KexiDB::Field *KexiTableEdit::displayedField() const
{
    if (m_column->visibleLookupColumnInfo())
        return m_column->visibleLookupColumnInfo()->field; //mainly for lookup field in KexiComboBoxTableEdit:

    return m_column->field(); //typical case
}

void KexiTableEdit::setViewWidget(QWidget *v)
{
    m_view = v;
    m_view->move(0, 0);
    //m_view->installEventFilter(this);
    setFocusProxy(m_view);
}

void KexiTableEdit::moveChild(QWidget * child, int x, int y)
{
#ifndef KEXI_MOBILE
    if (m_scrollView)
        m_scrollView->moveChild(child, x, y);
#endif
}

void KexiTableEdit::resize(int w, int h)
{
    QWidget::resize(w, h);
    if (m_view) {
        if (!layout()) { //if there is layout (eg. KexiInputTableEdit), resize is automatic
            m_view->move(0, 0);
            m_view->resize(w, h);
        }
    }
}

#if 0
bool
KexiTableEdit::eventFilter(QObject* watched, QEvent* e)
{
    /* if (watched == m_view) {
        if(e->type() == QEvent::KeyPress) {
          QKeyEvent* ev = static_cast<QKeyEvent*>(e);
    //   if (ev->key()==Qt::Key_Tab) {

    //   }
        }
      }*/

    if (watched == this) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent* ev = static_cast<QKeyEvent*>(e);

            if (ev->key() == Qt::Key_Escape) {
                return false;
            }
        } else {
            return false;
        }
    }
    return false;
// return QWidget::eventFilter(watched, e);
}
#endif

void KexiTableEdit::paintFocusBorders(QPainter *p, QVariant &, int x, int y, int w, int h)
{
    p->drawRect(x, y, w, h);
}

void KexiTableEdit::setupContents(QPainter *p, bool focused, const QVariant& val,
                                  QString &txt, int &align, int &/*x*/, int &y_offset, int &w, int &h)
{
    Q_UNUSED(p);
    Q_UNUSED(focused);
    Q_UNUSED(h);
    KexiDB::Field *realField = displayedField();

#ifdef Q_WS_WIN
// x = 1;
    y_offset = -1;
#else
// x = 1;
    y_offset = 0;
#endif

    if (realField->isFPNumericType()) {
//! @todo ADD OPTION to displaying NULL VALUES as e.g. "(null)"
        if (!val.isNull()) {
            txt = KexiDB::formatNumberForVisibleDecimalPlaces(
                      val.toDouble(), realField->visibleDecimalPlaces());
        }
        w -= 6;
        align |= Qt::AlignRight;
    } else if (realField->isIntegerType()) {
        qint64 num = val.toLongLong();
        w -= 6;
        align |= Qt::AlignRight;
        if (!val.isNull())
            txt = QString::number(num);
    } else {//default:
        if (!val.isNull()) {
            txt = val.toString();
        }
        align |= Qt::AlignLeft;
    }
}

void KexiTableEdit::paintSelectionBackground(QPainter *p, bool /*focused*/,
        const QString& txt, int align, int x, int y_offset, int w, int h, const QColor& fillColor,
        const QFontMetrics &fm, bool readOnly, bool fullRecordSelection)
{
    if (!readOnly && !fullRecordSelection && !txt.isEmpty()) {
        QRect bound = fm.boundingRect(x, y_offset, w - (x + x), h, align, txt);
        bound.setY(0);
        bound.setWidth(qMin(bound.width() + 2, w - (x + x) + 1));
        if (align & Qt::AlignLeft) {
            bound.setX(bound.x() - 1);
        } else if (align & Qt::AlignRight) {
            bound.moveLeft(w - bound.width());   //move to left, if too wide
        }
//TODO align center
        bound.setHeight(h - 1);
        p->fillRect(bound, fillColor);
    } else if (fullRecordSelection) {
        p->fillRect(0, 0, w, h, fillColor);
    }
}

int KexiTableEdit::widthForValue(const QVariant &val, const QFontMetrics &fm)
{
    return fm.width(val.toString());
}

void KexiTableEdit::repaintRelatedCell()
{
#ifndef KEXI_MOBILE
    if (dynamic_cast<KexiDataAwareObjectInterface*>(m_scrollView))
        dynamic_cast<KexiDataAwareObjectInterface*>(m_scrollView)->updateCurrentCell();
#endif
}

bool KexiTableEdit::showToolTipIfNeeded(const QVariant& value, const QRect& rect, const QFontMetrics& fm,
                                        bool focused)
{
    Q_UNUSED(value);
    Q_UNUSED(rect);
    Q_UNUSED(fm);
    Q_UNUSED(focused);
    return false;
}

int KexiTableEdit::rightMargin(bool focused) const
{
    return focused ? m_rightMarginWhenFocused : m_rightMargin;
}

#include "kexitableedit.moc"
