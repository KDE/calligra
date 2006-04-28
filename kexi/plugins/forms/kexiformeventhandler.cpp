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
#include <qobject.h>

#include <kdebug.h>
#include <klocale.h>
#include <kaction.h>

#include <tableview/kexitableitem.h>
#include <tableview/kexitableviewdata.h>
#include <kexidb/queryschema.h>
#include <keximainwindow.h>
#include <kexidialogbase.h>
#include <kexipart.h>
//#include <kexipartitem.h>

class KexiFormEventAction : public KAction
{
private:
	KexiMainWindow *m_mainWin;
	QString m_actionName, m_actionUri;
public:
	KexiFormEventAction(KexiMainWindow *mainWin, QObject* parent, const QString& actionName, const QString& actionUri)
		: KAction(parent), m_mainWin(mainWin), m_actionName(actionName), m_actionUri(actionUri) {}
public slots:
	void activate() {
		KexiPart::Part* part = Kexi::partManager().partForMimeType( QString("kexi/%1").arg(m_actionName) );
		KexiProject* project = m_mainWin->project();
		if( (! part) || (! project) )
			return;

		KexiPart::ItemDict* itemdict = project->items( part->info() );
		if(! itemdict)
			return;

		KexiPart::Item* item = 0;
		for(KexiPart::ItemDictIterator it(*itemdict); it.current(); ++it) {
			if(it.current()->name() == m_actionUri) {
				item = it.current();
				break;
			}
		}

		if(item)
			part->execute(item);
	}
};

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
	Q3Dict<char> tmpSources;
	for ( ; (obj = it.current()) != 0; ++it ) {
		QString actionName = obj->property("onClickAction").toString();
		if (actionName.startsWith("kaction:")) {
			actionName = actionName.mid(QString("kaction:").length()); //cut prefix
			KAction *action = mainWin->actionCollection()->action( actionName.latin1() );
			if (!action)
				continue;
			QObject::disconnect( obj, SIGNAL(clicked()), action, SLOT(activate()) ); //safety
			QObject::connect( obj, SIGNAL(clicked()), action, SLOT(activate()) );
		}
		else {
			QString actionUri;
			if (actionName.startsWith("macro:")) {
				actionUri = actionName.mid(QString("macro:").length()); //cut prefix
				actionName = "macro";
			}
			else if (actionName.startsWith("script:")) {
				actionUri = actionName.mid(QString("script:").length()); //cut prefix
				actionName = "script";
			}
			else {
				continue; // ignore unknown actionName
			}
			KexiFormEventAction* action = new KexiFormEventAction(mainWin, obj, actionName, actionUri);
			QObject::disconnect( obj, SIGNAL(clicked()), action, SLOT(activate()) );
			QObject::connect( obj, SIGNAL(clicked()), action, SLOT(activate()) );
		}
	}
	delete l;
}

