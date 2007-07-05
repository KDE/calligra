/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifdef __GNUC__
#warning KexiComboBoxDropDownButton ported to Qt4 but not tested
#else
#pragma WARNING( KexiComboBoxDropDownButton ported to Qt4 but not tested )
#endif

KexiComboBoxDropDownButton::KexiComboBoxDropDownButton( QWidget *parent )
 : KPushButton(parent)
{
	m_paintedCombo = new KComboBox(this);
	m_paintedCombo->hide();
	m_paintedCombo->setEditable(true);

	setCheckable(true);
	styleChanged();
	m_paintedCombo->move(0,0);
	m_paintedCombo->setFixedSize(size());
}

KexiComboBoxDropDownButton::~KexiComboBoxDropDownButton()
{
}

void KexiComboBoxDropDownButton::paintEvent(QPaintEvent *pe)
{
	KPushButton::paintEvent(pe);
	
	QPainter p(this);
	QRect r = rect();
	r.setHeight(r.height()+m_fixForHeight);
	if (m_drawComplexControl) {
		if (m_fixForHeight>0 && m_paintedCombo->size()!=size()) {
			m_paintedCombo->move(0,0);
			m_paintedCombo->setFixedSize(size()+QSize(0, m_fixForHeight)); //last chance to fix size
		}
		QStyleOptionComplex option;
		option.initFrom( m_fixForHeight>0 ? (const QWidget*)m_paintedCombo : this );
		option.rect = r;
		option.state = QStyle::State_HasFocus 
			| (isDown() ? QStyle::State_Raised : QStyle::State_Sunken);

#ifdef __GNUC__
#warning TODO compare to Qt code for QStyles
#else
#pragma WARNING( TODO compare to Qt code for QStyles )
#endif
		style()->drawComplexControl( QStyle::CC_ComboBox, &option, &p,
			m_fixForHeight>0 ? (const QWidget*)m_paintedCombo : this);
// TODO flags, (uint)(QStyle::SC_ComboBoxArrow), QStyle::SC_None );
	}
	else {
#ifdef __GNUC__
#warning TODO compare to Qt code for QStyles
#else
#pragma WARNING( TODO compare to Qt code for QStyles )
#endif
		r.setWidth(r.width()+2);
		QStyleOption option;
		option.initFrom(this);
		option.rect = r;
		p.drawPixmap( r, style()->standardPixmap(QStyle::SP_ArrowDown, &option) );
		//style().drawPrimitive( QStyle::PE_ArrowDown, p, r, colorGroup(), flags);
	}
}

bool KexiComboBoxDropDownButton::event( QEvent *event )
{
	if ( event->type() == QEvent::StyleChange )
		styleChanged();
	return KPushButton::event( event );
}

void KexiComboBoxDropDownButton::styleChanged()
{
#ifdef __GNUC__
#warning TODO simplify KexiComboBoxDropDownButton::styleChanged()
#else
#pragma WARNING( TODO simplify KexiComboBoxDropDownButton::styleChanged() )
#endif
	//<hack>
	if (style()->objectName().toLower()=="thinkeramik") {
		m_fixForHeight = 3;
	}
	else
		m_fixForHeight = 0;
	//</hack>
	m_drawComplexControl =
		(style()->inherits("KStyle") && style()->objectName().toLower()!="qtcurve")
		|| style()->objectName().toLower()=="platinum";
	if (m_fixForHeight==0) {
/*TODO		setFixedWidth( style()->subControlRect( QStyle::CC_ComboBox, 0, QStyle::SC_ComboBoxArrow,
			(const QWidget*)m_paintedCombo ).width() +1 );*/
	}
}
