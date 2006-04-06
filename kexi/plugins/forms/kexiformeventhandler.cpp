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
#include <kaction.h>

#include <tableview/kexitableitem.h>
#include <tableview/kexitableviewdata.h>
#include <kexidb/queryschema.h>
#include <keximainwindow.h>
#include <kexidialogbase.h>
#include <kexipart.h>
//#include <kexipartitem.h>

class KexiFormEventScriptAction : public KAction
{
private:
	KexiMainWindow *m_mainWin;
	QString m_scripturi;
public:
	KexiFormEventScriptAction(KexiMainWindow *mainWin, QObject* parent, const QString& scripturi)
		: KAction(parent), m_mainWin(mainWin), m_scripturi(scripturi) {}
public slots:
	void activate() {
		KexiPart::Part* scriptpart = Kexi::partManager().partForMimeType("kexi/script");
		KexiProject* project = m_mainWin->project();
		if( (! scriptpart) || (! project) )
			return;

		KexiPart::ItemDict* itemdict = project->items( scriptpart->info() );
		if(! itemdict)
			return;

		KexiPart::Item* item = 0;
		for(KexiPart::ItemDictIterator it(*itemdict); it.current(); ++it) {
			if(it.current()->name() == m_scripturi) {
				item = it.current();
				break;
			}
		}

		if(item)
			scriptpart->execute(item);
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
	QDict<char> tmpSources;
	for ( ; (obj = it.current()) != 0; ++it ) {
		QString actionName = obj->property("onClickAction").toString();
		if(actionName.isEmpty())
			continue;

		if (actionName.startsWith("kaction:")) {
			actionName = actionName.mid(QString("kaction:").length()); //cut prefix
			KAction *action = mainWin->actionCollection()->action( actionName.latin1() );
			if (!action)
				continue;
			QObject::disconnect( obj, SIGNAL(clicked()), action, SLOT(activate()) ); //safety
			QObject::connect( obj, SIGNAL(clicked()), action, SLOT(activate()) );
		}
		else if (actionName.startsWith("script:")) {
			actionName = actionName.mid(QString("script:").length()); //cut prefix
			KexiFormEventScriptAction* action = new KexiFormEventScriptAction(mainWin, obj, actionName);
			QObject::disconnect( obj, SIGNAL(clicked()), action, SLOT(activate()) );
			QObject::connect( obj, SIGNAL(clicked()), action, SLOT(activate()) );
		}
	}
	delete l;
}

