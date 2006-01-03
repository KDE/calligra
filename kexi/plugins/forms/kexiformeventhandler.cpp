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
#include <kurl.h>

#include <tableview/kexitableitem.h>
#include <tableview/kexitableviewdata.h>
#include <kexidb/queryschema.h>
#include <keximainwindow.h>
#include <kexidialogbase.h>
#include <kexipart.h>
#include <kexipartitem.h>
#include <kexiproject.h>

class ScriptAction : public KAction
{
private:
	KexiMainWindow* m_mainWin;
	KexiPart::Item* m_item;
public:
	ScriptAction(QObject* parent, KexiMainWindow *mainWin, KexiPart::Item* item)
		: KAction(parent), m_mainWin(mainWin), m_item(item) {}
public slots:
	void activate() {
		QMap<QString,QString> map;
		map["do"] = "execute";

		/*KexiDialogBase* dialog =*/ m_mainWin->openObject(m_item, Kexi::DesignViewMode, &map);
		//dialog->minimize();
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
		KURL actionurl = obj->property("onClickAction").toString();

		if (actionurl.protocol() == "kaction") {
			//this is kaction:
			QString actionName = actionurl.fileName();
			KAction *action = (actionName.isEmpty()) ? 0 : mainWin->actionCollection()->action( actionName.latin1() );
			if (!action) {
				//! @todo show error?
				continue;
			}
			QObject::disconnect( obj, SIGNAL(clicked()), action, SLOT(activate()) ); //safety
			QObject::connect( obj, SIGNAL(clicked()), action, SLOT(activate()) );
		}
		else if (actionurl.protocol() == "script") {
			QString scriptName = actionurl.fileName();
			if(scriptName.isNull())
				continue;

			KexiPart::Part* scriptpart = Kexi::partManager().partForMimeType("kexi/script");
			KexiProject* project = mainWin->project();
			if( !scriptpart || !project)
				continue;

			KexiPart::Info* info = scriptpart->info();
			KexiPart::ItemDict* itemdict = project->items(info);
			if(! itemdict)
				continue;

			for (KexiPart::ItemDictIterator it( *itemdict ); it.current(); ++it) {
				if(it.current()->name() == scriptName) {
					ScriptAction* action = new ScriptAction(obj, mainWin, it.current());
					QObject::disconnect( obj, SIGNAL(clicked()), action, SLOT(activate()) );
					QObject::connect( obj, SIGNAL(clicked()), action, SLOT(activate()) );
					break;
				}
			}
		}
	}
	delete l;
}

