/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>

#include "libactionwidget.h"
#include "widgetfactory.h"

using namespace KFormDesigner;

LibActionWidget::LibActionWidget(Widget *w, KActionCollection *c)
 : KToggleAction(w->name(), w->pixmap(), 0/*Key_F5*/, 0, 0 /*SLOT(slotWidget())*/, c, QString("library_widget_" + w->className()).latin1())
{
	kdDebug() << "LibActionWidget::LibActionWidget(): " << QString("library_widget_" + w->className()).latin1() << endl;
	m_className = w->className();
	setExclusiveGroup("LibActionWidgets");
//	connect(this, SIGNAL(activated()), this, SLOT(slotWidget()));
}

void
LibActionWidget::slotActivated()
{
	KToggleAction::slotActivated();
	if (isChecked())
		emit prepareInsert(m_className);
}

LibActionWidget::~LibActionWidget()
{
}

#include "libactionwidget.moc"
