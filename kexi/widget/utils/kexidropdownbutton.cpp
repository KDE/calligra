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

#include "kexidropdownbutton.h"

#include <KDebug>

#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QApplication>
#include <QKeyEvent>

#include <kexi_global.h>

#ifdef __GNUC__
#warning KexiDropDownButton ported but not tested
#else
#pragma WARNING( KexiDropDownButton ported but not tested )
#endif

KexiDropDownButton::KexiDropDownButton(QWidget *parent)
        : QToolButton(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
//! @todo get this from a KStyle
// setFixedWidth(QMAX(18, qApp->globalStrut().width()));
    int fixedWidth;
    //hack
#ifdef __GNUC__
#warning TODO use subControlRect
#else
#pragma WARNING( TODO use subControlRect )
#endif
    /*TODO
      if (style()->objectName().toLower()=="thinkeramik")
        fixedWidth = 18; //typical width as in "windows" style
      else
        fixedWidth = style()->subControlRect( QStyle::CC_ComboBox, 0,
          QStyle::SC_ComboBoxArrow ).width();
      setFixedWidth( fixedWidth );
      */
#ifdef __GNUC__
#warning setPopupDelay(10/*ms*/);
#else
#pragma WARNING( setPopupDelay(10/*ms*/); )
#endif
}

KexiDropDownButton::~KexiDropDownButton()
{
}

QSize KexiDropDownButton::sizeHint() const
{
    return QSize(fontMetrics().maxWidth() + 2*2, fontMetrics().height()*2 + 2*2);
}

void KexiDropDownButton::paintEvent(QPaintEvent *e)
{
    QToolButton::paintEvent(e);
    QPainter p(this);
    /* QStyle::SFlags arrowFlags = QStyle::Style_Default;
      if (isDown() || state()==On)
        arrowFlags |= QStyle::Style_Down;
      if (isEnabled())
        arrowFlags |= QStyle::Style_Enabled;*/
    QStyleOption option;
    option.initFrom(this);
    style()->drawPrimitive(QStyle::PE_IndicatorButtonDropDown, &option, &p);
    /*  style()->drawPrimitive(QStyle::PE_IndicatorButtonDropDown, 0, &p,
        QRect((width()-7)/2, height()-9, 7, 7), colorGroup(),
        arrowFlags, QStyleOption() );*/
}

void KexiDropDownButton::keyPressEvent(QKeyEvent * e)
{
    const int k = e->key();
    const bool dropDown =
        (e->modifiers() == Qt::NoButton
         && (k == Qt::Key_Space || k == Qt::Key_Enter || k == Qt::Key_Return || k == Qt::Key_F2
             || k == Qt::Key_F4)
        )
        || (e->modifiers() == Qt::AltButton && k == Qt::Key_Down);

    if (dropDown) {
        e->accept();
        animateClick();
        QMouseEvent me(QEvent::MouseButtonPress, QPoint(2, 2), Qt::LeftButton, Qt::NoButton,
                       Qt::NoModifier);
        QApplication::sendEvent(this, &me);
        return;
    }
    QToolButton::keyPressEvent(e);
}

#include "kexidropdownbutton.moc"
