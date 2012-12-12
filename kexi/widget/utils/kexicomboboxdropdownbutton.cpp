/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

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

#include <KDebug>
#include <KComboBox>

#include <QStyle>
#include <QPainter>
#include <QEvent>
#include <kexi_global.h>

#ifdef __GNUC__
#warning KexiComboBoxDropDownButton ported to Qt4 but not tested
#else
#pragma WARNING( KexiComboBoxDropDownButton ported to Qt4 but not tested )
#endif

class KexiComboBoxDropDownButton::Private
{
public:
    Private() {}

    int fixForHeight;

    bool drawComplexControl;

    //! fake combo used only to pass it as 'this' for QStyle
    //! (because styles use \<static_cast\>)
    KComboBox *paintedCombo;
};

KexiComboBoxDropDownButton::KexiComboBoxDropDownButton(QWidget *parent)
        : KPushButton(parent)
        , d(new Private)
{
    d->paintedCombo = new KComboBox(this);
    d->paintedCombo->hide();
    d->paintedCombo->setEditable(true);

    setCheckable(true);
    styleChanged();
    d->paintedCombo->move(0, 0);
    d->paintedCombo->setFixedSize(size());
}

KexiComboBoxDropDownButton::~KexiComboBoxDropDownButton()
{
    delete d;
}

void KexiComboBoxDropDownButton::paintEvent(QPaintEvent *pe)
{
    KPushButton::paintEvent(pe);

    QPainter p(this);
    QRect r = rect();
    r.setHeight(r.height() + d->fixForHeight);
    if (d->drawComplexControl) {
        if (d->fixForHeight > 0 && d->paintedCombo->size() != size()) {
            d->paintedCombo->move(0, 0);
            d->paintedCombo->setFixedSize(size() + QSize(0, d->fixForHeight)); //last chance to fix size
        }
        QStyleOptionComplex option;
        option.initFrom(d->fixForHeight > 0 ? (const QWidget*)d->paintedCombo : this);
        option.rect = r;
        option.state = QStyle::State_HasFocus
                       | (isDown() ? QStyle::State_Raised : QStyle::State_Sunken);

#ifdef __GNUC__
#warning TODO compare to Qt code for QStyles
#else
#pragma WARNING( TODO compare to Qt code for QStyles )
#endif
        style()->drawComplexControl(QStyle::CC_ComboBox, &option, &p,
                                    d->fixForHeight > 0 ? (const QWidget*)d->paintedCombo : this);
// TODO flags, (uint)(QStyle::SC_ComboBoxArrow), QStyle::SC_None );
    } else {
#ifdef __GNUC__
#warning TODO compare to Qt code for QStyles
#else
#pragma WARNING( TODO compare to Qt code for QStyles )
#endif
        r.setWidth(r.width() + 2);
//  QStyleOption option;
        QStyleOptionComboBox option;
        option.initFrom(this);
        option.rect = r;
        option.subControls = QStyle::SC_ComboBoxArrow;
        style()->drawComplexControl(QStyle::CC_ComboBox, &option, &p, this);
//  p.drawPixmap( r, style()->standardPixmap(QStyle::SP_ArrowDown, &option) );

        //style().drawPrimitive( QStyle::PE_ArrowDown, p, r, colorGroup(), flags);
    }
//! @todo use tableview's appearance parameters for color
    QPen linePen(Qt::black);
    linePen.setWidth(1);
    p.setPen(linePen);
    p.drawLine(r.topLeft(), r.topRight());
}

bool KexiComboBoxDropDownButton::event(QEvent *event)
{
    if (event->type() == QEvent::StyleChange)
        styleChanged();
    return KPushButton::event(event);
}

void KexiComboBoxDropDownButton::styleChanged()
{
#ifdef __GNUC__
#warning TODO simplify KexiComboBoxDropDownButton::styleChanged()
#else
#pragma WARNING( TODO simplify KexiComboBoxDropDownButton::styleChanged() )
#endif
    //<hack>
    if (style()->objectName().toLower() == "thinkeramik") {
        d->fixForHeight = 3;
    } else
        d->fixForHeight = 0;
    //</hack>
    d->drawComplexControl = false;
    /* d->drawComplexControl =
        (style()->inherits("KStyle") && style()->objectName().toLower()!="qtcurve")
        || style()->objectName().toLower()=="platinum";*/
    if (d->fixForHeight == 0) {
        /*TODO  setFixedWidth( style()->subControlRect( QStyle::CC_ComboBox, 0, QStyle::SC_ComboBoxArrow,
              (const QWidget*)d->paintedCombo ).width() +1 );*/
    }
}
