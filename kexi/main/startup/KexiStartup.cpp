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
#ifdef Q_WS_WIN
# include "KexiStartup_p_win.h"
#else
# include "KexiStartup_p.h"
#endif

#include "kexiproject.h"
#include "kexiprojectdata.h"
#include "kexiprojectset.h"
#include "kexiguimsghandler.h"

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
#include <kpassdlg.h>
#include <kprogress.h>

#include <unistd.h>

#if KDE_IS_VERSION(3,1,9)
# include <kuser.h>
#endif

#include <qcstring.h>
#include <qapplication.h>

namespace Kexi {
	static KexiStartupHandler _startupHandler;
	
	KexiStartupHandler& startupHandler() { return _startupHandler; }
}

//---------------------------------

class KexiStartupHandlerPrivate
{
	public:
		KexiStartupHandlerPrivate()
		{
		}

		int dummy;
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

void updateProgressBar(KProgressDialog *pd, char *buffer, int buflen)
{
	char *p = buffer;
	QCString line(80);
	for (int i=0; i<buflen; i++, p++) {
		if ((i==0 || buffer[i-1]=='\n') && buffer[i]=='%') {
			bool ok;
			int j=0;
//			char *q=++p;
			++i;
			line="";
			for (;i<buflen && *p>='0' && *p<='9'; j++, i++, p++)
				line+=QChar(*p);
			--i; --p;
			int percent = line.toInt(&ok);
			if (ok && percent>=0 && percent<=100 && pd->progressBar()->progress()<percent) {
//				kdDebug() << percent << endl;
				pd->progressBar()->setProgress(percent);
				qApp->processEvents(100);
			}
		}
	}
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

bool KexiStartupHandler::getAutoopenObjects(KCmdLineArgs *args, const QCString &action_name)
{
	QCStringList list = args->getOptionList(action_name);
	QCStringList::const_iterator it;
	bool atLeastOneFound = false;
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
		atLeastOneFound = true;
	}
	return atLeastOneFound;
}

bool KexiStartupHandler::init(int argc, char **argv)
{
	m_action = DoNothing;
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs(0);
	if (!args)
		return true;

	KexiDB::ConnectionData cdata;
	cdata.driverName = args->getOption("dbdriver");
//	if (cdata.driverName.isEmpty())
//		cdata.driverName = KexiDB::Driver::defaultFileBasedDriverName();
	cdata.hostName = args->getOption("host");
	cdata.localSocketFileName = args->getOption("local-socket");
	cdata.userName = args->getOption("user");
	cdata.password = args->getOption("password");
	bool fileDriverSelected;
	if (cdata.driverName.isEmpty())
		fileDriverSelected = true;
	else {
		KexiDB::DriverManager dm;
		fileDriverSelected = dm.driverInfo(cdata.driverName).fileBased;
	}
//	const bool fileDriverSelected = cdata.driverName.lower()==KexiDB::Driver::defaultFileBasedDriverName();
	bool projectFileExists = false;

	//obfuscate the password, if present
//TODO: remove --password
	for (int i=1; i<(argc-1); i++) {
		if (qstrcmp("--password",argv[i])==0
			|| qstrcmp("-password",argv[i])==0)
		{
			QCString pwd(argv[i+1]);
			if (!pwd.isEmpty()) {
				pwd.fill(' ');
				pwd[0]='x';
				qstrcpy(argv[i+1], (const char*)pwd);
			}
			break;
		}
	}
	
	const QString portStr = args->getOption("port");
	if (!portStr.isEmpty()) {
		bool ok;
		const int p = portStr.toInt(&ok);
		if (ok && p > 0)
			cdata.port = p;
		else {
			KMessageBox::sorry( 0, 
				i18n("You have specified invalid port number \"%1\"."));
			return false;
		}
	}

	m_forcedFinalMode = args->isSet("final-mode");
	m_forcedDesignMode = args->isSet("design-mode");
	m_createDB = args->isSet("createdb");
	m_dropDB = args->isSet("dropdb");
	const bool openExisting = !m_createDB && !m_dropDB;
	const QString couldnotMsg = QString::fromLatin1("\n")
		+i18n("Could not start Kexi application this way.");
	
	if (m_createDB && m_dropDB) {
		KMessageBox::sorry( 0, i18n(
			"You have used both \"createdb\" and \"dropdb\" startup options.")+couldnotMsg);
		return false;
	};

	if (m_createDB || m_dropDB) {
		if (args->count()<1) {
			KMessageBox::sorry( 0, i18n("No project name specified.") );
			return false;
		}
		m_action = Exit;
	}

//TODO: add option for non-gui; integrate with KWallet; 
//      move to static KexiProject method
	if (!fileDriverSelected && !cdata.driverName.isEmpty() && cdata.password.isEmpty()) {
		QString msg = cdata.userName.isEmpty() ?
			"<p>"+i18n("Please enter the password.")
			: "<p>"+i18n("Please enter the password for user %1.").arg(cdata.userName);

		QString srv = cdata.serverInfoString(false);
		if (srv.isEmpty() || srv.lower()=="localhost")
			srv = i18n("local database server");
		msg += ("</p><p>"+i18n("Database server: %1.").arg(srv)+"</p>");

		QCString pwd;
		if (QDialog::Accepted == KPasswordDialog::getPassword(pwd, msg)) {
			cdata.password = QString(pwd);
		}
		else {
			m_action = Exit;
			return true;
		}
	}

	kdDebug() << "ARGC==" << args->count() << endl;
	for (int i=0;i<args->count();i++) {
		kdDebug() << "ARG" <<i<< "= " << args->arg(i) <<endl;
	}

	if (m_forcedFinalMode && m_forcedDesignMode) {
		KMessageBox::sorry( 0, i18n(
		"You have used both \"final-mode\" and \"design-mode\" ""startup options.")+couldnotMsg);
		return false;
	}

	//database filenames, shortcut filenames or db names on a server
	if (args->count()>=1) {
		QString prjName = args->arg(0);
		if (fileDriverSelected) {
			cdata.setFileName( prjName );
			QFileInfo finfo(cdata.dbFileName());
			projectFileExists = finfo.exists();

			if (m_dropDB && !projectFileExists) {
				KMessageBox::sorry(0, i18n("Could not remove project. The file \"%1\" does not exist.")
					.arg(cdata.dbFileName()));
				return 0;
			}
		}

		if (m_createDB) {
			if (cdata.driverName.isEmpty())
				cdata.driverName = KexiDB::Driver::defaultFileBasedDriverName();
			m_projectData = new KexiProjectData(cdata, prjName); //dummy
		}
		else {
			if (fileDriverSelected) {
				cdata.driverName = KexiStartupHandler::detectDriverForFile( cdata.driverName,
					cdata.dbFileName() );
				if (cdata.driverName.isEmpty())
					return false;
			}
			m_projectData = new KexiProjectData(cdata, prjName);
		}
//		if (!m_projectData)
//			return false;
	}
	if (args->count()>1) {
		//TODO: KRun another Kexi instances
	}

	//---autoopen objects:
	const bool atLeastOneAOOFound = getAutoopenObjects(args, "open")
		|| getAutoopenObjects(args, "design")
		|| getAutoopenObjects(args, "edittext")
		|| getAutoopenObjects(args, "new");

	if (atLeastOneAOOFound && !openExisting) {
		KMessageBox::information( 0, 
			i18n("You have specified a few database objects to be opened automatically, "
				"using startup options.\n"
				"These options will be ignored because it is not available while creating "
				"or dropping projects."));
	}

	if (m_createDB) {
		bool cancelled;
		KexiGUIMessageHandler gui;
		KexiProject *prj = KexiProject::createBlankProject(cancelled, projectData(), &gui);
		bool ok = prj!=0;
		if (ok) {
			KMessageBox::information( 0, i18n("Project \"%1\" created successfully.")
				.arg( projectData()->databaseName() ));
		}
		delete prj;
		return ok;
	}
	else if (m_dropDB) {
		KexiGUIMessageHandler gui;
		tristate res = KexiProject::dropProject(projectData(), &gui, false/*ask*/);
		if (res)
			KMessageBox::information( 0, i18n("Project \"%1\" dropped successfully.")
				.arg( projectData()->databaseName() ));
		return res!=false;
	}

	//------

/*	if (m_forcedFinalMode || (m_projectData && projectData->finalMode())) {
		//TODO: maybe also auto allow to open objects...
		KexiMainWindowImpl::initFinal(m_projectData);
		return;
	}*/

	if (!m_projectData) {
		cdata = KexiDB::ConnectionData(); //clear
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
#if !KDE_IS_VERSION(3,1,9)
			conndata->userName = getlogin(); //-- temporary
#else
			conndata->userName = KUser().loginName(); //-- temporary
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
				cdata.setFileName( selFile );
				cdata.driverName = KexiStartupHandler::detectDriverForFile( cdata.driverName, selFile );
				if (cdata.driverName.isEmpty())
					return false;
				m_projectData = new KexiProjectData(cdata, selFile);
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
	
	if (m_projectData && openExisting) {
		m_action = OpenProject;
	}
	//show if wasn't show yet
//	importantInfo(true);
	
	return true;
}

QString KexiStartupHandler::detectDriverForFile( 
	const QString& driverName, const QString &dbFileName, QWidget *parent )
{
	QString ret;
	QFileInfo finfo(dbFileName);
	if (dbFileName.isEmpty() || !finfo.isReadable()) {
		KMessageBox::sorry(parent, i18n(
			"Could not load project. The file \"%1\" does not exist.").arg(dbFileName));
		return QString::null;
	}
	if (!finfo.isWritable()) {
		//TODO: if file is ro: change project mode
	}
	KMimeType::Ptr ptr = KMimeType::findByFileContent(dbFileName);
	QString mimename = ptr.data()->name();
	kdDebug() << "KexiStartupHandler::detectProjectData(): found mime is: " 
		<< ptr.data()->name() << endl;
	if (mimename=="application/x-kexiproject-shortcut") {
		return QString::null;//TODO: get information for xml shortcut file
	}
	// "application/x-kexiproject-sqlite", etc
	QString detectedDriverName = Kexi::driverManager().lookupByMime(mimename).latin1();
	if (/*cdata.driverName.isEmpty() 
		||*/ (!driverName.isEmpty() && driverName.lower()!=detectedDriverName.lower() 
			&& KMessageBox::Yes == KMessageBox::warningYesNo(parent, i18n(
			"The project file \"%1\" is recognized as compatible with \"%2\" database driver, "
			"while you have asked for \"%3\" database driver to be used.\n"
			"Do you want to use \"%4\" database driver?")
			.arg(dbFileName).arg(detectedDriverName).arg(driverName).arg(detectedDriverName)) ))
	{
		ret = detectedDriverName;
	}
	kdDebug() << "KexiStartupHandler::detectProjectData(): driver name: " << ret << endl;
//hardcoded for convenience:
	const QString newFileFormat = "SQLite3";
	if (detectedDriverName.lower()=="sqlite2"
		  && KMessageBox::Yes == KMessageBox::questionYesNo(parent, i18n(
			"Previous version of database file format (\"%1\") is detected in the \"%2\" project file.\n"
			"Do you want to convert the project to a new \"%3\" format (recommended)?")
			.arg(detectedDriverName).arg(dbFileName).arg(newFileFormat)) )
	{
//		SQLite2ToSQLite3Migration *migr = new 
		SQLite2ToSQLite3Migration migr( finfo.absFilePath() );
		tristate res = migr.run();
		kdDebug() << "--- migr.run() END ---" <<endl;
		if (!res) {
			//TODO msg
			KMessageBox::sorry(parent, i18n(
				"Failed converting project file \"%1\" to a new \"%2\" format.\n"
				"The file format remain unchanged.")
				.arg(dbFileName).arg(newFileFormat) );
			//continue...
		}
		if (res)
			detectedDriverName = newFileFormat;
	}
	ret = detectedDriverName;
	if (ret.isEmpty()) {
		KMessageBox::detailedSorry(parent, 
			i18n( "The file \"%1\" is not recognized as being supported by Kexi.").arg(dbFileName),
			i18n("Database driver for this file type not found.\nDetected MIME type: %1").arg(mimename));
		return QString::null;
	}
	return ret;
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

