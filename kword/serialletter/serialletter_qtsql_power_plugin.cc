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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "serialletter_qtsql_power_plugin.h"
#include "serialletter_qtsql_power_plugin.moc"
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

#define KWQTSQLBarIcon( x ) BarIcon( x, db->KWInstance() )

/******************************************************************
 *
 * Class: KWQTSQLSerialDataSource
 *
 ******************************************************************/

KWQTSQLPowerSerialDataSource::KWQTSQLPowerSerialDataSource(KInstance *inst)
	: KWSerialLetterDataSource(inst),myquery(0)
{
	port=i18n("default");
}

KWQTSQLPowerSerialDataSource::~KWQTSQLPowerSerialDataSource()
{
	if (myquery) delete myquery;
	QSqlDatabase::removeDatabase("KWQTSQLPOWER");
}

void KWQTSQLPowerSerialDataSource::refresh(bool force)
{
	if ((force) || (myquery==0))
	{
		if (myquery)
		{
			delete myquery;
			myquery=0;
		}
		QString tmp=query.upper();
		if (!tmp.startsWith("SELECT")) return;
		if ((!database) || (!database->isOpen()))openDatabase();
		myquery=new QMySqlCursor(query,true,database);
		myquery->setMode(QSqlCursor::ReadOnly);
	}
	kdDebug()<<QString("There were %1 rows in the query").arg(myquery->size())<<endl;
}

QString KWQTSQLPowerSerialDataSource::getValue( const QString &name, int record ) const
{
	int num=record;

	if (!myquery) return name;
	if ( num < 0 || num > (int)myquery->size() )
		return name;
	if (!myquery->seek(num,false)) return i18n(">>>Illegal position within datasource<<<");
	if (!myquery->contains(name)) return i18n(">>>Field %1 is unknown in the current database query<<<").arg(name);
	return (myquery->value(name)).toString();
}

void KWQTSQLPowerSerialDataSource::save( QDomDocument &doc, QDomElement &parent)
{
}

void KWQTSQLPowerSerialDataSource::load( QDomElement& parentElem )
{
}

bool KWQTSQLPowerSerialDataSource::showConfigDialog(QWidget *par,int action)
{
   bool ret=false;
   if (action==KWSLOpen)
   {
   	KWQTSQLPowerSerialLetterOpen *dia=new KWQTSQLPowerSerialLetterOpen(par,this);

	ret=dia->exec();
	if (ret) openDatabase();
	delete dia;
   }
   else
   {
	KWQTSQLPowerSerialLetterEditor *dia=new KWQTSQLPowerSerialLetterEditor(par,this);
	ret=dia->exec();
	delete dia;
   }
   return ret;
}

void KWQTSQLPowerSerialDataSource::clearSampleRecord() {sampleRecord.clear();}

void KWQTSQLPowerSerialDataSource::addSampleRecordEntry(QString name)
{sampleRecord[name]=i18n("No Value");}

bool  KWQTSQLPowerSerialDataSource::openDatabase()
{
	QCString pwd;
	QSqlDatabase::removeDatabase("KWQTSQLPOWER");
        database=QSqlDatabase::addDatabase(driver,"KWQTSQLPOWER");
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
	QMessageBox::critical(0,i18n("Error"),i18n("Couldn't create database object"),QMessageBox::Abort,QMessageBox::NoButton,QMessageBox::NoButton);

}

/******************************************************************
 *
 * Class: KWQTSQLPowerSerialLetterOpen
 *
 ******************************************************************/

KWQTSQLPowerSerialLetterOpen::KWQTSQLPowerSerialLetterOpen( QWidget *parent, KWQTSQLPowerSerialDataSource *db_ )
        :KDialogBase( Plain, i18n( "Serial Letter - Setup database connection" ), Ok | Cancel, Ok, parent, "", true ), db( db_ )
{
	(new QVBoxLayout(plainPage()))->setAutoAdd(true);
	setMainWidget(widget=new KWQTSQLOpenWidget(plainPage()));
	widget->drivers->insertStringList(QSqlDatabase::drivers());
	widget->hostname->setText(db->hostname);
	widget->username->setText(db->username);
	widget->port->setText(db->port);
	widget->databasename->setText(db->databasename);
	connect(this,SIGNAL(okClicked()),this,SLOT(handleOk()));
}

KWQTSQLPowerSerialLetterOpen::~KWQTSQLPowerSerialLetterOpen(){;}

void KWQTSQLPowerSerialLetterOpen::handleOk()
{
	db->hostname=widget->hostname->text();
	db->username=widget->username->text();
	db->port=widget->port->text();
	db->databasename=widget->databasename->text();
	db->driver=widget->drivers->currentText();
}

/******************************************************************
 *
 * Class: KWQTSQLSerialLetterEditor
 *
 ******************************************************************/

KWQTSQLPowerSerialLetterEditor::KWQTSQLPowerSerialLetterEditor( QWidget *parent, KWQTSQLPowerSerialDataSource *db_ )
	:KDialogBase( Plain, i18n( "Serial Letter - Editor" ), Ok | Cancel, Ok, parent, "", true ), db( db_ )
{
        (new QVBoxLayout(plainPage()))->setAutoAdd(true);
        setMainWidget(widget=new KWQTSQLPowerWidget(plainPage()));
	connect(widget->setup,SIGNAL(clicked()),this,SLOT(openSetup()));
	connect(widget->tables,SIGNAL(currentChanged(QListBoxItem*)),this,SLOT(slotTableChanged(QListBoxItem*)));
	connect(widget->execute,SIGNAL(clicked()),this,SLOT(slotExecute()));
	connect(this,SIGNAL(okClicked()),this,SLOT(slotSetQuery()));
	widget->query->setText(db->query);
	updateDBViews();
}

void KWQTSQLPowerSerialLetterEditor::slotSetQuery()
{
	db->query=widget->query->text();
	db->refresh(true);
}

void KWQTSQLPowerSerialLetterEditor::slotExecute()
{
	QString tmp=widget->query->text().upper();
	if (!tmp.startsWith("SELECT")) return;
	QMySqlCursor *cur=new QMySqlCursor(widget->query->text(),true,db->database);
	cur->setMode(QSqlCursor::ReadOnly);

	db->clearSampleRecord();
	kdDebug()<<QString("Fieldname count %1").arg(cur->count())<<endl;
	for (int i=0;i<cur->count();i++)
		db->addSampleRecordEntry(cur->fieldName(i));

	widget->queryresult->setSqlCursor(cur,true,true);
	widget->queryresult->refresh(QDataTable::RefreshAll);
}

void KWQTSQLPowerSerialLetterEditor::slotTableChanged ( QListBoxItem * item )
{
	widget->fields->clear();
	if (item)
	{
		if (!db->database) return;
		QSqlRecord rec=db->database->record(item->text());
		for (int i=0;i<rec.count();i++)
		{
			widget->fields->insertItem(rec.fieldName(i));
		}
	}
}

void KWQTSQLPowerSerialLetterEditor::openSetup()
{
        KWQTSQLPowerSerialLetterOpen *dia=new KWQTSQLPowerSerialLetterOpen(this,db);
        if (dia->exec())
	{
		 db->openDatabase();
		 updateDBViews();
	}
        delete dia;
}


void KWQTSQLPowerSerialLetterEditor::updateDBViews()
{
	widget->fields->clear();
	widget->tables->clear();
	if (!db->database) return;
	widget->tables->insertStringList(db->database->tables());
}

KWQTSQLPowerSerialLetterEditor::~KWQTSQLPowerSerialLetterEditor(){;}


extern "C" {
        KWSerialLetterDataSource *create_kwserialletter_qtsqldb_power(KInstance *inst)
        {
                return new KWQTSQLPowerSerialDataSource(inst);
        }
}
