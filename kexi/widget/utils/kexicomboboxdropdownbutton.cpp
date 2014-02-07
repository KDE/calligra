/* This file is part of the KDE project
   Copyright (C) 2006-2014 Jarosław Staniek <staniek@kde.org>

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

#include "kexicomboboxdropdownbutton.h"

#include <kdebug.h>
#include <kcombobox.h>

#include <QStyle>
#include <QStyleOptionToolButton>
#include <QPainter>
#include <QEvent>

#include <kexiutils/styleproxy.h>



//! @internal A style that allows to alter some painting in KexiComboBoxDropDownButton.
class KexiComboBoxDropDownButtonStyle : public KexiUtils::StyleProxy
{
public:
    KexiComboBoxDropDownButtonStyle(QStyle *parentStyle, QWidget*)
            : KexiUtils::StyleProxy(parentStyle)
    {
    }
    virtual ~KexiComboBoxDropDownButtonStyle() {}
    virtual void drawComplexControl( ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const
    {
        QStyleOptionToolButton opt(*qstyleoption_cast<const QStyleOptionToolButton*>(option));
        opt.state |= (State_MouseOver | State_DownArrow | State_Sunken);
        opt.state ^= (State_MouseOver | State_DownArrow | State_Sunken);
        KexiUtils::StyleProxy::drawComplexControl(control, &opt, painter, widget);
    }
};

// ----

class KexiComboBoxDropDownButton::Private
{
public:
    Private() : styleChangeEnabled(true) {}
    QPointer<QStyle> privateStyle;
    bool styleChangeEnabled;
};

KexiComboBoxDropDownButton::KexiComboBoxDropDownButton(QWidget *parent)
        : QToolButton(parent)
        , d(new Private)
{
    setAutoRaise(true);
    setArrowType(Qt::DownArrow);
    styleChanged();
}

KexiComboBoxDropDownButton::~KexiComboBoxDropDownButton()
{
    delete d;
}

void KexiComboBoxDropDownButton::styleChanged()
{
    if (!d->styleChangeEnabled)
        return;
    d->styleChangeEnabled = false;
    if (d->privateStyle) {
        setStyle(0);
        delete static_cast<QStyle*>(d->privateStyle);
    }
    setStyle(d->privateStyle = new KexiComboBoxDropDownButtonStyle(style(), this));
    d->styleChangeEnabled = true;
}

bool KexiComboBoxDropDownButton::event(QEvent *event)
{
    if (event->type() == QEvent::StyleChange)
        styleChanged();
    return QToolButton::event(event);
}
