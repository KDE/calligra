/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
                 2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qtoolbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvalidator.h>
#include <qtooltip.h>

#include <klocale.h>
#include <kiconloader.h>
#include <klineedit.h>

#include <kexidb/cursor.h>

#include "kexirecordnavigator.h"

KexiRecordNavigator::KexiRecordNavigator(KexiDB::Cursor * /*cursor?? TODO */, 
	QWidget *parent, const char *name)
 : QFrame(parent, name)
{
	setFrameStyle(QFrame::Panel|QFrame::Raised);
	setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

	QHBoxLayout *navPanelLyr = new QHBoxLayout(this,0,0,"nav_lyr");
	navPanelLyr->setAutoAdd(true);

	new QLabel(QString(" ")+i18n("Row:")+" ",this);
	int bw = 6+SmallIcon("navigator_first").width(); //QMIN( horizontalScrollBar()->height(), 20);
	QFont f = font();
	f.setPixelSize((bw > 12) ? 12 : bw);
	QFontMetrics fm(f);
	m_nav1DigitWidth = fm.width("8");

	m_navBtnFirst = new QToolButton(this);
	m_navBtnFirst->setMaximumWidth(bw);
	m_navBtnFirst->setFocusPolicy(NoFocus);
	m_navBtnFirst->setIconSet( SmallIconSet("navigator_first") );
	QToolTip::add(m_navBtnFirst, i18n("First row"));
	
	m_navBtnPrev = new QToolButton(this);
	m_navBtnPrev->setMaximumWidth(bw);
	m_navBtnPrev->setFocusPolicy(NoFocus);
	m_navBtnPrev->setIconSet( SmallIconSet("navigator_prev") );
	QToolTip::add(m_navBtnPrev, i18n("Previous row"));
	
	QWidget *spc = new QFrame(this);
	spc->setFixedWidth(6);
	
	m_navRowNumber = new KLineEdit(this);
	m_navRowNumber->setAlignment(AlignRight | AlignVCenter);
	m_navRowNumber->setFocusPolicy(ClickFocus);
//	m_navRowNumber->setFixedWidth(fw);
	m_navRowNumberValidator = new QIntValidator(1, 1, this);
	m_navRowNumber->setValidator(m_navRowNumberValidator);
	m_navRowNumber->installEventFilter(this);
	QToolTip::add(m_navRowNumber, i18n("Current row number"));
	
	KLineEdit *lbl_of = new KLineEdit(i18n("of"), this);
	lbl_of->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
	lbl_of->setMaximumWidth(fm.width(lbl_of->text())+8);
	lbl_of->setReadOnly(true);
	lbl_of->setLineWidth(0);
	lbl_of->setFocusPolicy(NoFocus);
	lbl_of->setAlignment(AlignCenter);
	
	m_navRowCount = new KLineEdit(this);
//	m_navRowCount->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
	m_navRowCount->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
//	m_navRowCount->setMaximumWidth(fw);
	m_navRowCount->setReadOnly(true);
	m_navRowCount->setLineWidth(0);
	m_navRowCount->setFocusPolicy(NoFocus);
	m_navRowCount->setAlignment(AlignLeft | AlignVCenter);

	lbl_of->setFont(f);
	m_navRowNumber->setFont(f);
	m_navRowCount->setFont(f);
	setFont(f);

	m_navBtnNext = new QToolButton(this);
	m_navBtnNext->setMaximumWidth(bw);
	m_navBtnNext->setFocusPolicy(NoFocus);
	m_navBtnNext->setIconSet( SmallIconSet("navigator_next") );
	QToolTip::add(m_navBtnNext, i18n("Next row"));
	
	m_navBtnLast = new QToolButton(this);
	m_navBtnLast->setMaximumWidth(bw);
	m_navBtnLast->setFocusPolicy(NoFocus);
	m_navBtnLast->setIconSet( SmallIconSet("navigator_last") );
	QToolTip::add(m_navBtnLast, i18n("Last row"));
	
	spc = new QWidget(this);
	spc->setFixedWidth(6);
	m_navBtnNew = new QToolButton(this);
	m_navBtnNew->setMaximumWidth(bw);
	m_navBtnNew->setFocusPolicy(NoFocus);
	m_navBtnNew->setIconSet( SmallIconSet("navigator_new") );
	QToolTip::add(m_navBtnNew, i18n("New row"));
	m_navBtnNext->setEnabled(false/*isInsertingEnabled()*/);
	
	spc = new QFrame(this);
	spc->setFixedWidth(6);
}

KexiRecordNavigator::~KexiRecordNavigator()
{
}

#include "kexirecordnavigator.moc"
