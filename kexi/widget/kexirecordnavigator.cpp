/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

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

#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>

#include "kexirecordnavigator.h"

KexiRecordNavigator::KexiRecordNavigator(KexiDBRecordSet *record, QWidget *parent, const char *name)
{
//	setFixedHeight(20);

	QPushButton *btnFirst = new QPushButton("<<", this);
	QPushButton *btnPrev = new QPushButton("<", this);
	QPushButton *btnNext = new QPushButton(">", this);
	QPushButton *btnLast = new QPushButton(">>", this);


	QGridLayout *g = new QGridLayout(this);
	g->addWidget(btnFirst,	0,	0);
	g->addWidget(btnPrev,	0,	1);
	g->addWidget(btnNext,	0,	2);
	g->addWidget(btnLast,	0,	3);

}

KexiRecordNavigator::~KexiRecordNavigator()
{
}

#include "kexirecordnavigator.moc"
