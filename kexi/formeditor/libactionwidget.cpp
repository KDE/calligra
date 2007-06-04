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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <kdebug.h>
#include <KIcon>
#include <KActionCollection>

#include "libactionwidget.h"
#include "widgetfactory.h"
#include "formmanager.h"

using namespace KFormDesigner;

LibActionWidget::LibActionWidget(WidgetInfo *w, KActionCollection *c)
 : KToggleAction(KIcon(w->pixmap()), w->name(), c)
{
	setObjectName( QString("library_widget_" + w->className()) );
	FormManager::self()->widgetActionGroup()->addAction( this );
//	kDebug() << "LibActionWidget::LibActionWidget(): " << QString("library_widget_" + w->className()).toLatin1() << endl;
	m_className = w->className();
//kde4 not needed	setExclusiveGroup("LibActionWidgets");
	setToolTip(w->name());
	setWhatsThis(w->description());
//	connect(this, SIGNAL(activated()), this, SLOT(slotWidget()));
}

void
LibActionWidget::slotToggled(bool checked)
{
	KToggleAction::slotToggled(checked);
	if (checked)
		emit prepareInsert(m_className);
}

LibActionWidget::~LibActionWidget()
{
}

#include "libactionwidget.moc"
