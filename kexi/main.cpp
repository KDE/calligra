/*
                          main.cpp  -  description
                             -------------------
    begin                : Sun Jun  9 12:15:11 CEST 2002
    copyright            : (C) 2003 by lucijan busch, Joseph Wenninger
    email                : lucijan@gmx.at, jowenn@kde.org

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <dcopclient.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kmimetype.h>

#include "core/kexiaboutdata.h"
#include "core/kexiproject.h"
#include "core/keximainwindow.h"
#include "core/kexidialogbase.h"

#include "core/kexi.h"

#include <qfileinfo.h>

static KCmdLineOptions options[] =
{
  { "open [<object_type>:]<object_name>", I18N_NOOP("Open object of type <object_type>\nand name <object_name> from specified project\non application start.\n<object_type>: is optional, if omitted - table\ntype is assumed.\nOther object types can be query, report, form,\nscript (may be more or less, depending on your\nplugins installed).\nUse \"\" chars to specify names containing spaces.\nExamples: --open MyTable,\n --open query:\"My very big query\""), 0 },
  { "+[file]", I18N_NOOP("Database project file (or shortcut file) to open"), 0 },
  KCmdLineLastOption
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

bool startupActions(KexiProjectData * &projectData)
{
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs(0);
	if (!args || args->count()==0)
		return true;
	
	kdDebug() << "ARGC==" << args->count() << endl;
	for (int i=0;i<args->count();i++) {
		kdDebug() << "ARG" <<i<< "= " << args->arg(i) <<endl;
	}
	QString fname;
	fname = args->arg(0);
	QFileInfo finfo(fname);
	if (fname.isEmpty() || !finfo.isReadable()) {
		KMessageBox::sorry(0, i18n("<qt>The file <b>%1</b> does not exist.</qt>").arg(fname));
		return false;
	}
	if (!finfo.isWritable()) {
		//TODO: if file is ro: change project mode
	}
	KMimeType::Ptr ptr = KMimeType::findByFileContent(fname);
	QString mimename = ptr.data()->name();
	kdDebug() << "startupActions(): found mime is: " << ptr.data()->name() << endl;
	if (mimename=="application/x-kexiproject-shortcut") {
		//TODO: get information for xml shortcut file
	}
	// "application/x-kexiproject-sqlite", etc
	QString drivername = Kexi::driverManager.lookupByMime(mimename);
	kdDebug() << "KexiProject::open(): driver name: " << drivername << endl;
	if(drivername.isEmpty()) {
		KMessageBox::detailedSorry(0, i18n("<qt>File <b>%1</b> is not recognised as supported by Kexi.</qt>").arg(fname),
			i18n("Database driver for this file not found.<br>Detected MIME type: %1").arg(mimename));
		return false;
	}
	else {
		KexiDB::ConnectionData cdata;
		cdata.driverName = drivername;
		cdata.setFileName( fname );
		projectData = new KexiProjectData(cdata,fname);
		kdDebug() << "file is a living databse of engine " << drivername << endl;
	}
	if (args->count()>1) {
		//TODO: KRun another Kexi instances
	}
#if 0 //TODO(js)
	QString not_found_msg;
	QCStringList list = args->getOptionList("open");
	QCStringList::const_iterator it;
	for ( it = list.begin(); it!=list.end(); ++it) {
		QString type_name, obj_name, item=*it;
		int idx;
		//option with " " (type: default)
		if (item.left(1)=="\"" && item.right(1)=="\"") {
			obj_name = item.mid(1, item.length()-2);
			type_name = "table";
		}
		//option with type name specified:
		else if ((idx = item.find(':'))!=-1) {
			type_name = item.left(idx).lower();
			obj_name = item.mid(idx+1);
			//optional: remove ""
			if (obj_name.left(1)=="\"" && obj_name.right(1)=="\"")
				obj_name = obj_name.mid(1, obj_name.length()-2);
		}
		//just obj. name: type name is "table" by default
		else {
			obj_name = item;
			type_name = "table";
		}
		if (type_name.isEmpty() || obj_name.isEmpty())
			continue;
		//ok, now open this object
		QString obj_mime = QString("kexi/") + type_name;
		QString obj_identifier = obj_mime + "/" + obj_name;
		KexiProjectHandler *hd = handlerForMime(obj_mime);
		KexiProjectHandlerProxy *pr = hd ? hd->proxy(view) : 0;
		if (!pr || !pr->executeItem(obj_identifier)) {
			if (!not_found_msg.isEmpty())
				not_found_msg += ",<br>";
			not_found_msg += (pr ? pr->part()->name() : i18n("Unknown object")) + " \"" + obj_name + "\"";
		}
	}
	if (!not_found_msg.isEmpty())
		KMessageBox::sorry(0, "<p><b>" + i18n("Requested objects cannot be opened:") + "</b><p>" + not_found_msg );
#endif //0
	return true;
}

int main(int argc, char *argv[])
{
	KCmdLineArgs::init( argc, argv, newKexiAboutData() );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

	KApplication app;
	
	KexiProjectData *projectData=0;
	
	if (!startupActions(projectData))
		//return 1;
	kdDebug() << "startupActions(): OK" <<endl;
//	app.dcopClient()->attach();
//	app.dcopClient()->registerAs( "kexi" );

//	KexiProject *project = new KexiProject();
	KexiMainWindow *win = new KexiMainWindow();
	app.setMainWidget(win);
	win->show();
	app.processEvents();//allow refresh our app
	
	win->startup(projectData);

//	project->parseCmdLineOptions();

	return app.exec();
}
