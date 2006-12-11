/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jaroslaw Staniek <js@iidea.pl>

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

#include <tableview/kexitableitem.h>
#include <tableview/kexitableviewdata.h>
#include <kexidb/queryschema.h>
#include <keximainwindow.h>
#include <kexidialogbase.h>
#include <kexipart.h>
#include <kexipartinfo.h>
#include <kexipartitem.h>

KexiFormEventAction::ActionData::ActionData()
{
}

bool KexiFormEventAction::ActionData::isEmpty() const
{
	return string.isEmpty();
}

KexiPart::Info* KexiFormEventAction::ActionData::decodeString(
	QString& actionType, QString& actionArg, bool& ok) const
{
	const int idx = string.find(':');
	ok = false;
	if (idx==-1)
		return 0;
	const QString _actionType = string.left(idx);
	const QString _actionArg = string.mid(idx+1);
	if (_actionType.isEmpty() || _actionArg.isEmpty())
		return 0;
	KexiPart::Info *info = 0;
	if (_actionType!="kaction") {
		info = Kexi::partManager().infoForMimeType( QString("kexi/%1").arg(_actionType) );
		if (!info)
			return 0;
	}
	actionType = _actionType;
	actionArg = _actionArg;
	ok = true;
	return info;
}

//-------------------------------------

KexiFormEventAction::KexiFormEventAction(KexiMainWindow *mainWin, QObject* parent, 
	const QString& actionName, const QString& objectName, const QString& actionOption)
 : KAction(parent), m_mainWin(mainWin), m_actionName(actionName), m_objectName(objectName)
 , m_actionOption(actionOption)
{
}

KexiFormEventAction::~KexiFormEventAction()
{
}

void KexiFormEventAction::activate()
{
	KexiProject* project = m_mainWin->project();
	if (!project)
		return;
	KexiPart::Part* part = Kexi::partManager().partForMimeType( 
		QString("kexi/%1").arg(m_actionName) );
	if (!part)
		return;
	KexiPart::Item* item = project->item( part->info(), m_objectName );
	if (!item)
		return;
	bool openingCancelled;
	int supportedViewModes = part->supportedViewModes();
	if (m_actionOption.isEmpty()) { // backward compatibility (good defaults)
		if (part->info()->isExecuteSupported())
			part->execute(item, parent());
		else
			m_mainWin->openObject(item, Kexi::DataViewMode, openingCancelled);
	}
	else {
//! @todo react on failure...
		if (m_actionOption == "open")
			m_mainWin->openObject(item, Kexi::DataViewMode, openingCancelled);
		else if (m_actionOption == "execute")
			part->execute(item, parent());
		else if (m_actionOption == "print") {
			if (part->info()->isPrintingSupported())
				m_mainWin->printItem(item);
		}
		else if (m_actionOption == "printPreview") {
			if (part->info()->isPrintingSupported())
				m_mainWin->printPreviewForItem(item);
		}
		else if (m_actionOption == "pageSetup") {
			if (part->info()->isPrintingSupported())
				m_mainWin->showPageSetupForItem(item);
		}
		else if (m_actionOption == "exportToCSV"
			|| m_actionOption == "copyToClipboardAsCSV")
		{
			if (part->info()->isDataExportSupported())
				m_mainWin->executeCustomActionForObject(item, m_actionOption);
		}
		else if (m_actionOption == "design")
			m_mainWin->openObject(item, Kexi::DesignViewMode, openingCancelled);
		else if (m_actionOption == "editText")
			m_mainWin->openObject(item, Kexi::TextViewMode, openingCancelled);
	}
}

//------------------------------------------

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
	for ( ; (obj = it.current()) != 0; ++it ) {
		bool ok;
		KexiFormEventAction::ActionData data;
		data.string = obj->property("onClickAction").toString();
		data.option = obj->property("onClickActionOption").toString();
		if (data.isEmpty())
			continue;

		QString actionType, actionArg;
		KexiPart::Info* partInfo = data.decodeString(actionType, actionArg, ok);
		if (!ok)
			continue;
		if (actionType=="kaction") {
			KAction *action = mainWin->actionCollection()->action( actionArg.latin1() );
			if (!action)
				continue;
			QObject::disconnect( obj, SIGNAL(clicked()), action, SLOT(activate()) ); //safety
			QObject::connect( obj, SIGNAL(clicked()), action, SLOT(activate()) );
		}
		else if (partInfo) { //'open or execute' action
			KexiFormEventAction* action = new KexiFormEventAction(mainWin, obj, actionType, actionArg, 
				data.option);
			QObject::disconnect( obj, SIGNAL(clicked()), action, SLOT(activate()) );
			QObject::connect( obj, SIGNAL(clicked()), action, SLOT(activate()) );
		}
	}
	delete l;
}
