/* This file is part of the KDE project
   Copyright (C) 2001 Joseph Wenninger <jowenn@kde.org>

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

#include "serialletter_qtsql_base.h"
#include "serialletter_qtsql_base.moc"
#include "qtsqlopeneditor.h"
#include <qlayout.h>
#include <qdom.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <qsqldatabase.h>
#include <qmessagebox.h>
#include <kpassdlg.h>
#include <qsqlrecord.h>
#include <qsqlcursor.h>
#include <qdatatable.h>
#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>




/******************************************************************
 *
 * Class: KWQTSQLSerialDataSourceBase
 *
 ******************************************************************/

int KWQTSQLSerialDataSourceBase::connectionId=0;

KWQTSQLSerialDataSourceBase::KWQTSQLSerialDataSourceBase(KInstance *inst,QObject *parent)
	: KWMailMergeDataSource(inst,parent)
{
	DataBaseConnection=QString("KWQTSQLPOWER")+parent->name()+QString("--%1").arg(connectionId++);
	port=i18n("default");
}

KWQTSQLSerialDataSourceBase::~KWQTSQLSerialDataSourceBase()
{
	QSqlDatabase::removeDatabase(DataBaseConnection);
}


bool KWQTSQLSerialDataSourceBase::showConfigDialog(QWidget *par,int action)
{
   bool ret=false;
   if (action==KWSLOpen)
   {
   	KWQTSQLMailMergeOpen *dia=new KWQTSQLMailMergeOpen(par,this);

	ret=dia->exec();
	if (ret) openDatabase();
	delete dia;
   }
   return ret;
}

bool  KWQTSQLSerialDataSourceBase::openDatabase()
{
	QCString pwd;
	QSqlDatabase::removeDatabase(DataBaseConnection);
        database=QSqlDatabase::addDatabase(driver,DataBaseConnection);
        if (database)
        {
                if (database->lastError().type()!=QSqlError::None)
                {
			QMessageBox::critical(0,i18n("Error"),database->lastError().databaseText(),QMessageBox::Abort,QMessageBox::NoButton,QMessageBox::NoButton);
			return false;
                }
                database->setDatabaseName(databasename);
                database->setUserName(username);
                database->setHostName(hostname);
		if ((port!=i18n("default"))&& (!port.isEmpty()))
			database->setPort(port.toInt());

		if (KPasswordDialog::getPassword(pwd, i18n("Please enter the password for the database connection"))
			== KPasswordDialog::Accepted) database->setPassword(pwd);
                if (database->open())
                {
                        return true;
                }
		QMessageBox::critical(0,i18n("Error"),database->lastError().databaseText(),QMessageBox::Abort,QMessageBox::NoButton,QMessageBox::NoButton);
		return false;
        }
	QMessageBox::critical(0,i18n("Error"),i18n("Unable to create database object"),QMessageBox::Abort,QMessageBox::NoButton,QMessageBox::NoButton);
        return false;
}
