/* This file is part of the KDE project
   Copyright (C) 2006-2008 Jarosław Staniek <staniek@kde.org>

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

#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QApplication>
#include <QKeyEvent>
#include <QStyleOptionToolButton>

#include <kdebug.h>

#include <kexi_global.h>
#include <kexiutils/styleproxy.h>

//! @internal A style that removes menu indicator from KexiDropDownButton.
class KexiDropDownButtonStyle : public KexiUtils::StyleProxy
{
public:
    KexiDropDownButtonStyle(QStyle *parentStyle)
            : KexiUtils::StyleProxy(parentStyle)
    {
    }
    virtual ~KexiDropDownButtonStyle() {}

    virtual void drawComplexControl( ComplexControl control, const QStyleOptionComplex * option,
        QPainter * painter, const QWidget * widget = 0 ) const
    {
        if (control == CC_ToolButton && qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
            QStyleOptionToolButton newOption(*qstyleoption_cast<const QStyleOptionToolButton *>(option));
            newOption.features &= ~QStyleOptionToolButton::HasMenu;

            StyleProxy::drawComplexControl(control, &newOption, painter, widget);
            return;
        }
        StyleProxy::drawComplexControl(control, option, painter, widget);
    }

    virtual int styleHint( StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const
    {
        if (hint == QStyle::SH_ToolButton_PopupDelay) {
            return 0;
        }
        return StyleProxy::styleHint(hint, option, widget, returnData);
    }
};


KexiDropDownButton::KexiDropDownButton(QWidget *parent)
        : QToolButton(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    KexiDropDownButtonStyle *s = new KexiDropDownButtonStyle(style());
    s->setParent(this);
    setStyle(s);
//! @todo get this from a KStyle
// setFixedWidth(QMAX(18, qApp->globalStrut().width()));
//    int fixedWidth;
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
    QStyleOptionToolButton option;
    option.initFrom(this);
    //option.state |= isDown() ? QStyle::State_Sunken : QStyle::State_Raised;
    style()->drawPrimitive(QStyle::PE_IndicatorButtonDropDown, &option, &p);

    //! @todo use tableview's appearance parameters for color
    QRect r = rect();
    QPen linePen(Qt::black);
    linePen.setWidth(1);
    p.setPen(linePen);
    p.drawLine(r.topLeft(), r.topRight());
}

void KexiDropDownButton::keyPressEvent(QKeyEvent * e)
{
    const int k = e->key();
    const bool dropDown =
        (e->modifiers() == Qt::NoButton
         && (k == Qt::Key_Space || k == Qt::Key_Enter || k == Qt::Key_Return || k == Qt::Key_F2
             || k == Qt::Key_F4)
        )
        || (e->modifiers() == Qt::AltModifier && k == Qt::Key_Down);

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
