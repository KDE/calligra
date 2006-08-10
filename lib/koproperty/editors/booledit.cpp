/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "booledit.h"
#include "../property.h"

#ifndef QT_ONLY
#include <kiconloader.h>
#include <klocale.h>
#endif

#include <qtoolbutton.h>
#include <qpainter.h>
#include <qvariant.h>
#include <qlayout.h>
#include <qbitmap.h>

#include <kdebug.h>

using namespace KoProperty;

//! @internal reimplemented to allow setting tristate mode
class BoolEditButton : public QToolButton
{
    public:
        BoolEditButton( QWidget * parent )
        : QToolButton( parent )
        {
        }
        virtual void setToggleType( ToggleType type ) { QToolButton::setToggleType(type); }
        virtual void setState( ToggleState s ) { QToolButton::setState(s); }
};

BoolEdit::BoolEdit(Property *property, QWidget *parent, const char *name)
 : Widget(property, parent, name)
 , m_yesIcon( SmallIcon("button_ok") )
 , m_noIcon( SmallIcon("button_no") )
{
    QVariant thirdState = property ? property->option("3rdState") : QVariant();
    if (!thirdState.toString().isEmpty()) {
        m_3rdStateText = thirdState.toString();
        m_nullIcon = QPixmap( m_noIcon.size() ); //transparent pixmap of appropriate size
        m_nullIcon.setMask(QBitmap(m_noIcon.size(), true));
    }
    m_toggle = new BoolEditButton(this);
    m_toggle->setToggleType( m_3rdStateText.isEmpty() ? QButton::Toggle : QButton::Tristate );
    m_toggle->setFocusPolicy(QWidget::WheelFocus);
    m_toggle->setUsesTextLabel(true);
    m_toggle->setTextPosition(QToolButton::Right);
    m_toggle->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    //we're not using layout to because of problems with button size
    m_toggle->move(0, 0);
    m_toggle->resize(width(), height());
    setFocusWidget(m_toggle);
    connect(m_toggle, SIGNAL(stateChanged(int)), this, SLOT(slotValueChanged(int)));
}

BoolEdit::~BoolEdit()
{
}

QVariant
BoolEdit::value() const
{
    if (m_toggle->state()==QButton::NoChange)
        return QVariant();
    return QVariant(m_toggle->isOn(), 4);
}

void
BoolEdit::setValue(const QVariant &value, bool emitChange)
{
    m_toggle->blockSignals(true);
    QButton::ToggleState s = (value.isNull() && !m_3rdStateText.isEmpty()) ? QButton::NoChange 
        : (value.toBool() ? QButton::On : QButton::Off);
    if (s!=QButton::NoChange)
        m_toggle->setOn(value.toBool());
    else
        m_toggle->setState( s );
    setState( s );
    m_toggle->blockSignals(false);
    if (emitChange)
        emit valueChanged(this);
}

void
BoolEdit::slotValueChanged(int state)
{
    setState(state);
    emit valueChanged(this);
}

void
BoolEdit::drawViewer(QPainter *p, const QColorGroup &, const QRect &r, const QVariant &value)
{
    p->eraseRect(r);
    QRect r2(r);
    r2.moveLeft(KIcon::SizeSmall + 6);

    if(value.isNull()) {
        p->drawText(r2, Qt::AlignVCenter | Qt::AlignLeft, m_3rdStateText);
    }
    else if(value.toBool()) {
        p->drawPixmap(3, (r.height()-1-KIcon::SizeSmall)/2, m_yesIcon);
        p->drawText(r2, Qt::AlignVCenter | Qt::AlignLeft, i18n("Yes"));
    }
    else  {
        p->drawPixmap(3, (r.height()-1-KIcon::SizeSmall)/2, m_noIcon);
        p->drawText(r2, Qt::AlignVCenter | Qt::AlignLeft, i18n("No"));
    }
}

void
BoolEdit::setState(int state)
{
    if(QButton::On == state) {
        m_toggle->setIconSet(QIconSet(m_yesIcon));
        m_toggle->setTextLabel(i18n("Yes"));
    }
    else if (QButton::Off == state) {
        m_toggle->setIconSet(QIconSet(m_noIcon));
        m_toggle->setTextLabel(i18n("No"));
    }
    else {
        m_toggle->setIconSet(QIconSet(m_nullIcon));
        m_toggle->setTextLabel(m_3rdStateText);
    }
}

void
BoolEdit::resizeEvent(QResizeEvent *ev)
{
    m_toggle->resize(ev->size());
}

bool
BoolEdit::eventFilter(QObject* watched, QEvent* e)
{
    if(e->type() == QEvent::KeyPress) {
        QKeyEvent* ev = static_cast<QKeyEvent*>(e);
        if(ev->key() == Key_Space) {
            if (m_toggle)
                m_toggle->toggle();
            return true;
        }
    }
    return Widget::eventFilter(watched, e);
}

void
BoolEdit::setReadOnlyInternal(bool readOnly)
{
    setVisibleFlag(!readOnly);
}

#include "booledit.moc"
