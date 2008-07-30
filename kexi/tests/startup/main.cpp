/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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
#include <kapplication.h>

//#include <tableview/kexitableview.h>
/*#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>*/

#include "main/startup/KexiStartupDialog.h"
#include "main/startup/KexiConnSelector.h"
#include "core/kexiprojectset.h"
#include "core/kexiprojectdata.h"

int main(int argc, char* argv[])
{
  KApplication app(argc, argv, "startup");
  
//	Widget w;
//	app.setMainWidget(&w);
  
/*	KexiTableView tv;
  app.setMainWidget(&tv);

  KexiTableViewData data;
  KexiTableViewColumn col;
  col.type = QVariant::Int; col.caption = "Id"; data.addColumn( col );
  col.type = QVariant::String; col.caption = "Name"; data.addColumn( col );
  col.type = QVariant::Int; col.caption = "Age"; data.addColumn( col );
  tv.setData(&data, false);
  tv.show();*/

  //some connection data
  KexiDBConnectionSet connset;
  KexiDB::ConnectionData *conndata;
  conndata = new KexiDB::ConnectionData();
    conndata->name = "My connection 1";
    conndata->driverName = "mysql";
    conndata->hostName = "host.net";
    conndata->userName = "user";
  connset.addConnectionData(conndata);
  conndata = new KexiDB::ConnectionData();
    conndata->name = "My connection 2";
    conndata->driverName = "mysql";
    conndata->hostName = "myhost.org";
    conndata->userName = "otheruser";
    conndata->port = 53121;
  connset.addConnectionData(conndata);
  
  //some recent projects data
  KexiProjectData *prjdata;
  prjdata = new KexiProjectData( *conndata, "bigdb", "Big DB" );
  prjdata->setCaption("My Big Project");
  prjdata->setDescription("This is my first biger project started yesterday. Have fun!");
  KexiProjectSet prj_set;
  prj_set.addProjectData(prjdata);
  
  KexiStartupDialog startup(KexiStartupDialog::Everything, 0, connset, prj_set, 0, "dlg");
  int e=startup.exec();
  kDebug() << (e==QDialog::Accepted ? "Accepted" : "Rejected") << endl;
  
  if (e==QDialog::Accepted) {
    int r = startup.result();
    if (r==KexiStartupDialog::TemplateResult) {
      kDebug() << "Template key == " << startup.selectedTemplateKey() << endl;
      if (startup.selectedTemplateKey()=="blank") {
#if 0				
        KexiConnSelectorDialog sel(connset, 0,"sel");
        e = sel.exec();
        kDebug() << (e==QDialog::Accepted ? "Accepted" : "Rejected") << endl;
        if (e==QDialog::Accepted) {
          kDebug() << "Selected conn. type: " << (sel.selectedConnectionType()==KexiConnSelectorWidget::FileBased ? "File based" : "Server based") << endl;
          if (sel.selectedConnectionType()==KexiConnSelectorWidget::ServerBased) {
            kDebug() << "SERVER: " << sel.selectedConnectionData()->serverInfoString() << endl;
          }
        }
#endif				
      }
    }
    else if (r==KexiStartupDialog::OpenExistingResult) {
      kDebug() << "Existing project --------" << endl;
      QString selFile = startup.selectedExistingFile();
      if (!selFile.isEmpty())
        kDebug() << "Project File: " << selFile << endl;
      else if (startup.selectedExistingConnection()) {
        kDebug() << "Existing connection: " << startup.selectedExistingConnection()->serverInfoString() << endl;
        //ok, now we are trying to show daabases for this conenction to this user
        //todo
      }
    }
    else if (r==KexiStartupDialog::OpenRecentResult) {
      kDebug() << "Recent project --------" << endl;
      const KexiProjectData *data = startup.selectedProjectData();
      if (data) {
        kDebug() << "Selected project: database=" << data->databaseName()
          << " connection=" << data->constConnectionData()->serverInfoString() << endl;
      }
    }
  }
}
