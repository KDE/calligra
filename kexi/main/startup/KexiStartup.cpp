/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "KexiStartup.h"
#include "kexiprojectdata.h"
#include "kexiprojectset.h"

#include <kexidb/driver.h>
#include <kexidb/drivermanager.h>
#include "KexiStartupDialog.h"
#include "KexiConnSelector.h"
#include "KexiProjectSelectorBase.h"
#include "KexiProjectSelector.h"
#include "KexiNewProjectWizard.h"

#include <kdebug.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kmessagebox.h>
#include <kcmdlineargs.h>
#include <kdeversion.h>

#if defined(Q_WS_WIN) || !KDE_IS_VERSION(3,1,9)
# include <unistd.h>
#else
# include <kuser.h>
#endif

#include <qfileinfo.h>
#include <qcstring.h>

namespace Kexi {
	KexiStartupHandler _startupHandler;
	
	KexiStartupHandler& startupHandler() { return _startupHandler; }
}

//---------------------------------

class KexiStartupHandlerPrivate
{
	public:
		KexiStartupHandlerPrivate()
		 : projectData(0)
		 , action(KexiStartupData::DoNothing)
		{
		}
		KexiProjectData *projectData;
		KexiStartupData::Action action;
		bool forcedFinalMode : 1;
		bool forcedDesignMode : 1;
};

//---------------------------------

static bool stripQuotes(const QString &item, QString &name)
{
	if (item.left(1)=="\"" && item.right(1)=="\"") {
		name = item.mid(1, item.length()-2);
		return true;
	}
	name = item;
	return false;
}

//---------------------------------
KexiStartupHandler::KexiStartupHandler()
 : QObject(0,"KexiStartupHandler")
 , KexiStartupData()
 , d( new KexiStartupHandlerPrivate() )
{
}

KexiStartupHandler::~KexiStartupHandler()
{
	delete d;
}

void KexiStartupHandler::getAutoopenObjects(KCmdLineArgs *args, const QCString &action_name)
{
	QCStringList list = args->getOptionList(action_name);
	QCStringList::const_iterator it;
	for ( it = list.begin(); it!=list.end(); ++it) {
		QString type_name, obj_name, item=*it;
		int idx;
		bool name_required = true;
		if (action_name=="new") {
			obj_name = "";
			stripQuotes(item, type_name);
			name_required = false;
		}
		else {//open, design, text
			//option with " " (default type == "table")
			if (stripQuotes(item, obj_name)) {
				type_name = "table";
			}
			else if ((idx = item.find(':'))!=-1) {
				//option with type name specified:
				type_name = item.left(idx).lower();
				obj_name = item.mid(idx+1);
				//optional: remove ""
				if (obj_name.left(1)=="\"" && obj_name.right(1)=="\"")
					obj_name = obj_name.mid(1, obj_name.length()-2);
			}
			else {
				//just obj. name: type name is "table" by default
				obj_name = item;
				type_name = "table";
			}
		}
		if (type_name.isEmpty())
			continue;
		if (name_required && obj_name.isEmpty())
			continue;

		KexiProjectData::ObjectInfo info;
		info["name"]=obj_name;
		info["type"]=type_name;
		info["action"]=action_name;
		//ok, now add info for this object
		projectData()->autoopenObjects.append( info );
//		projectData->autoopenObjects.append( QPair<QString,QString>(type_name, obj_name) );
	}
}

bool KexiStartupHandler::init()
{
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs(0);
	if (!args || args->count()==0)
		return true;

	m_forcedFinalMode = args->isSet("final-mode");
	m_forcedDesignMode = args->isSet("design-mode");
	
	if (m_forcedFinalMode && m_forcedDesignMode) {
		KMessageBox::sorry( 0, i18n("You have set both \"final-mode\" and \"design-mode\" startup options."
		"Cannot start Kexi application this way."));
		return false;
	};
	
	kdDebug() << "ARGC==" << args->count() << endl;
	for (int i=0;i<args->count();i++) {
		kdDebug() << "ARG" <<i<< "= " << args->arg(i) <<endl;
	}
	QString fname;
	fname = args->arg(0);
	m_projectData = KexiStartupHandler::detectProjectData( fname, 0 );
	if (!m_projectData)
		return false;
	if (args->count()>1) {
		//TODO: KRun another Kexi instances
	}

	//---autoopen objects:
	getAutoopenObjects(args, "open");
	getAutoopenObjects(args, "design");
	getAutoopenObjects(args, "edittext");
	getAutoopenObjects(args, "new");

	//------

/*	if (m_forcedFinalMode || (m_projectData && projectData->finalMode())) {
		//TODO: maybe also auto allow to open objects...
		KexiMainWindowImpl::initFinal(m_projectData);
		return;
	}*/

	kdDebug() << "KexiMainWindowImpl::startup()..." << endl;
	if (!m_projectData) {
//		importantInfo(true);
//<TEMP>
		//some connection data
		KexiDB::ConnectionData *conndata;
		conndata = new KexiDB::ConnectionData();
			conndata->connName = "My connection";
			conndata->driverName = "mysql";
			conndata->hostName = "myhost.org";
			conndata->userName = "otheruser";
			conndata->port = 53121;
		Kexi::connset().addConnectionData(conndata);
		conndata = new KexiDB::ConnectionData();
			conndata->connName = "Local pgsql connection";
			conndata->driverName = "postgresql";
			conndata->hostName = "localhost"; // -- default //"host.net";
#if defined(Q_WS_WIN) || !KDE_IS_VERSION(3,1,9)
			conndata->userName = getlogin(); //-- temporary e.g."jarek"
#else
			conndata->userName = KUser().loginName(); //-- temporary e.g."jarek"
#endif
		Kexi::connset().addConnectionData(conndata);

		//some recent projects data
		KexiProjectData *projectData = new KexiProjectData( *conndata, "bigdb", "Big DB" );
		projectData->setCaption("My Big Project");
		projectData->setDescription("This is my first biger project started yesterday. Have fun!");
		Kexi::recentProjects().addProjectData(projectData);
	//</TEMP>

		if (!KexiStartupDialog::shouldBeShown())
			return true;

		KexiStartupDialog dlg(KexiStartupDialog::Everything, KexiStartupDialog::CheckBoxDoNotShowAgain,
			Kexi::connset(), Kexi::recentProjects(), 0, "dlg");
		if (dlg.exec()!=QDialog::Accepted)
			return true;

		int r = dlg.result();
		if (r==KexiStartupDialog::TemplateResult) {
			kdDebug() << "Template key == " << dlg.selectedTemplateKey() << endl;
			if (dlg.selectedTemplateKey()=="blank") {
				m_action = CreateBlankProject;
				//createBlankDatabase();
				return true;
			}
			
			return true;//todo - templates: m_action = UseTemplate;
		}
		else if (r==KexiStartupDialog::OpenExistingResult) {
			kdDebug() << "Existing project --------" << endl;
			QString selFile = dlg.selectedExistingFile();
			if (!selFile.isEmpty()) {
				//file-based project
				kdDebug() << "Project File: " << selFile << endl;
				m_projectData = KexiStartupHandler::detectProjectData( selFile, 0 );
			}
			else if (dlg.selectedExistingConnection()) {
				kdDebug() << "Existing connection: " << dlg.selectedExistingConnection()->serverInfoString() << endl;
				KexiDB::ConnectionData *cdata = dlg.selectedExistingConnection();
				//ok, now we will try to show projects for this connection to the user
				m_projectData = selectProject( cdata );
			}
		}
		else if (r==KexiStartupDialog::OpenRecentResult) {
			kdDebug() << "Recent project --------" << endl;
			const KexiProjectData *data = dlg.selectedProjectData();
			if (data) {
				kdDebug() << "Selected project: database=" << data->databaseName()
					<< " connection=" << data->constConnectionData()->serverInfoString() << endl;
			}
			//js: TODO
			return true;
		}

		if (!m_projectData)
			return true;
	}
	
	if (m_projectData) {
		m_action = OpenProject;
	}
//	openProject(projectData);

	//show if wasn't show yet
//	importantInfo(true);
	
	return true;
}

KexiProjectData* KexiStartupHandler::detectProjectData( const QString &fname, QWidget *parent)
{
	KexiProjectData *projectData = 0;
	QFileInfo finfo(fname);
	if (fname.isEmpty() || !finfo.isReadable()) {
		KMessageBox::sorry(parent, i18n("<qt>The file \"%1\" does not exist.").arg(fname));
		return 0;
	}
	if (!finfo.isWritable()) {
		//TODO: if file is ro: change project mode
	}
	KMimeType::Ptr ptr = KMimeType::findByFileContent(fname);
	QString mimename = ptr.data()->name();
	kdDebug() << "KexiStartupHandler::detectProjectData(): found mime is: " << ptr.data()->name() << endl;
	if (mimename=="application/x-kexiproject-shortcut") {
		return 0;//TODO: get information for xml shortcut file
	}
	// "application/x-kexiproject-sqlite", etc
	QString drivername = Kexi::driverManager().lookupByMime(mimename);
	kdDebug() << "KexiStartupHandler::detectProjectData(): driver name: " << drivername << endl;
	if(drivername.isEmpty()) {
		KMessageBox::detailedSorry(parent, i18n( "The file \"%1\" is not recognized as being supported by Kexi.").arg(fname),
		i18n("<qt>Database driver for this file type not found. <p>Detected MIME type: %1").arg(mimename));
		return 0;
	}
	KexiDB::ConnectionData cdata;
	cdata.driverName = drivername;
	cdata.setFileName( fname );
	projectData = new KexiProjectData(cdata,fname,fname);
	kdDebug() << "KexiStartupHandler::detectProjectData(): file is a living database of engine " << drivername << endl;
	return projectData;
}

KexiProjectData*
KexiStartupHandler::selectProject(KexiDB::ConnectionData *cdata, QWidget *parent)
{
	clearStatus();
	if (!cdata)
		return 0;
	KexiProjectData* projectData = 0;
	//dialog for selecting a project
	KexiProjectSelectorDialog prjdlg( parent, "prjdlg", cdata, true, false );
	if (!prjdlg.projectSet() || prjdlg.projectSet()->error()) {
//		showErrorMessage(i18n("Could not load list of available projects for connection \"%1\"")
//		.arg(cdata->serverInfoString()), prjdlg.projectSet());
		setStatus(i18n("Could not load list of available projects for connection \"%1\"")
		.arg(cdata->serverInfoString()), prjdlg.projectSet()->errorMsg());
		return 0;
	}
	if (prjdlg.exec()!=QDialog::Accepted)
		return 0;
	if (prjdlg.selectedProjectData()) {
		//deep copy
		projectData = new KexiProjectData(*prjdlg.selectedProjectData());
	}
	return projectData;
}
