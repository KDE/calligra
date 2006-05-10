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

#include "KWQtSqlSerialDataSourceBase.h"
#include "KWQtSqlSerialDataSourceBase.moc"
#include "KWQtSqlMailMergeOpen.h"
#include <QLayout>
#include <qdom.h>
//Added by qt3to4:
#include <QSqlError>
#include <kcombobox.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <qsqldatabase.h>
#include <qmessagebox.h>
#include <kpassworddialog.h>
#include <qsqlrecord.h>
#include <q3sqlcursor.h>
#include <q3datatable.h>
#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>




/******************************************************************
 *
 * Class: KWQtSqlSerialDataSourceBase
 *
 ******************************************************************/

int KWQtSqlSerialDataSourceBase::connectionId=0;

KWQtSqlSerialDataSourceBase::KWQtSqlSerialDataSourceBase(KInstance *inst,QObject *parent)
	: KWMailMergeDataSource(inst,parent)
{
	DataBaseConnection=QString("KWQTSQLPOWER")+parent->name()+QString("--%1").arg(connectionId++);
	port=i18n("default");
}

KWQtSqlSerialDataSourceBase::~KWQtSqlSerialDataSourceBase()
{
	QSqlDatabase::removeDatabase(DataBaseConnection);
}


bool KWQtSqlSerialDataSourceBase::showConfigDialog(QWidget *par,int action)
{
   bool ret=false;
   if (action==KWSLOpen)
   {
   	KWQtSqlMailMergeOpen *dia=new KWQtSqlMailMergeOpen(par,this);

	ret=dia->exec();
	if (ret) openDatabase();
	delete dia;
   }
   return ret;
}

bool  KWQtSqlSerialDataSourceBase::openDatabase()
{
	QByteArray pwd;
	QSqlDatabase::removeDatabase(DataBaseConnection);
        database=QSqlDatabase::addDatabase(driver,DataBaseConnection);
        if (database)
        {
                if (database->lastError().type()!=QSqlError::NoError)
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
