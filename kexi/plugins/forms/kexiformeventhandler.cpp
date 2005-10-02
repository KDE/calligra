/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexiformeventhandler.h"

#include <qwidget.h>
#include <qobjectlist.h>

#include <kdebug.h>
#include <klocale.h>

#include <tableview/kexitableitem.h>
#include <tableview/kexitableviewdata.h>
#include <kexidb/queryschema.h>
#include <keximainwindow.h>

KexiFormEventHandler::KexiFormEventHandler()
 : m_mainWidget(0)
{
}

KexiFormEventHandler::~KexiFormEventHandler()
{
}

void KexiFormEventHandler::setMainWidgetForEventHandling(KexiMainWindow *mainWin, QWidget* mainWidget)
{
	m_mainWidget = mainWidget;
	if (!m_mainWidget)
		return;

	//find widgets whose will work as data items
//! @todo look for other widgets too
	QObjectList *l = m_mainWidget->queryList( "KexiPushButton" );
	QObjectListIt it( *l );
	QObject *obj;
	QDict<char> tmpSources;
	for ( ; (obj = it.current()) != 0; ++it ) {
		QCString actionName( obj->property("onClickAction").toCString() );
//! @todo support also other action types!
		if (QString(actionName).startsWith("kaction:")) {
			//this is kaction:
			actionName = actionName.mid(QCString("kaction:").length()); //cut prefix
			KAction *action = mainWin->actionCollection()->action( actionName );
			if (!action) {
				//! @todo show error?
				continue;
			}
			QObject::disconnect( obj, SIGNAL(clicked()), action, SLOT(activate()) ); //safety
			QObject::connect( obj, SIGNAL(clicked()), action, SLOT(activate()) );
		}
	}
	delete l;
}

