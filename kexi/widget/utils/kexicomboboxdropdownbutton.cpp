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

#include <kpopupmenu.h>
#include <kdebug.h>

#include <qstyle.h>
#include <qapplication.h>

KexiComboBoxDropDownButton::KexiComboBoxDropDownButton( QWidget *parent )
 : KPushButton(parent)
{
	setToggleButton(true);
	styleChange(style());
}

KexiComboBoxDropDownButton::~KexiComboBoxDropDownButton()
{
}

void KexiComboBoxDropDownButton::drawButton(QPainter *p)
{
//			QStyle::PrimitiveElement e = QStyle::PE_ArrowDown;
	int flags = QStyle::Style_Enabled | QStyle::Style_HasFocus;
	if (isDown())
		flags |= QStyle::Style_Down;

	KPushButton::drawButton(p);

	QRect r = rect();
	r.setHeight(r.height()+m_fixForHeight);
	if (m_drawComplexControl) {
//				style().drawControl(QStyle::CE_PushButton, p, this, rect(), colorGroup(), flags);
		style().drawComplexControl( QStyle::CC_ComboBox, p, this, r, colorGroup(),
			flags, (uint)(QStyle::SC_ComboBoxArrow /*| QStyle::SC_ComboBoxFrame*/), //QStyle::SC_All ^ QStyle::SC_ComboBoxFrame,
			 /*QStyle::SC_ComboBoxArrow */ QStyle::SC_None );
	}
	else {
		r.setWidth(r.width()+2);
		style().drawPrimitive( QStyle::PE_ArrowDown, p, r, colorGroup(), flags);
	}
}

void KexiComboBoxDropDownButton::styleChange( QStyle & oldStyle )
{
	//<hack>
	if (qstricmp(style().name(),"thinkeramik")==0) {
		m_fixForHeight = 3;
	}
	else
		m_fixForHeight = 0;
	//</hack>
	m_drawComplexControl = style().inherits("KStyle") || qstricmp(style().name(),"platinum")==0;
	setFixedWidth( style().querySubControlMetrics( QStyle::CC_ComboBox, 
		this, QStyle::SC_ComboBoxArrow ).width() +1 );
	KPushButton::styleChange(oldStyle);
}

