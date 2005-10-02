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

#include "serialletter_qtsql_power_plugin.h"
#include "serialletter_qtsql_power_plugin.moc"
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

#define KWQTSQLBarIcon( x ) BarIcon( x, db->KWInstance() )

/******************************************************************
 *
 * Class: KWQTSQLSerialDataSource
 *
 ******************************************************************/

KWQTSQLPowerSerialDataSource::KWQTSQLPowerSerialDataSource(KInstance *inst,QObject *parent)
	: KWQTSQLSerialDataSourceBase(inst,parent),myquery(0)
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
        QDomElement def=doc.createElement(QString::fromLatin1("DEFINITION"));
        parent.appendChild(def);
        {
                QDomElement defEnt=doc.createElement(QString::fromLatin1("DATABASE"));
                defEnt.setAttribute(QString::fromLatin1("hostname"),hostname);
                defEnt.setAttribute(QString::fromLatin1("port"),port);
                defEnt.setAttribute(QString::fromLatin1("driver"),driver);
                defEnt.setAttribute(QString::fromLatin1("databasename"),databasename);
                defEnt.setAttribute(QString::fromLatin1("username"),username);
                def.appendChild(defEnt);

		defEnt=doc.createElement(QString::fromLatin1("QUERY"));
		defEnt.setAttribute(QString::fromLatin1("value"),query);
		def.appendChild(defEnt);

		QDomElement sampleEnt=doc.createElement(QString::fromLatin1("SAMPLERECORD"));
		parent.appendChild(sampleEnt);
	        for (DbRecord::Iterator it=sampleRecord.begin();it!=sampleRecord.end();++it)
        	{
                	QDomElement fieldEnt=doc.createElement(QString::fromLatin1("FIELD"));
	                fieldEnt.setAttribute(QString::fromLatin1("name"),it.key());
	                sampleEnt.appendChild(fieldEnt);
	        }
        }
}

void KWQTSQLPowerSerialDataSource::load( QDomElement& parentElem )
{
        clearSampleRecord();
        QDomNode defNd=parentElem.namedItem("DEFINITION");
        if (!defNd.isNull())
	{
	        QDomElement def=defNd.toElement();
		QDomNode dbNd=def.namedItem("DATABASE");
		if (!dbNd.isNull())
		{
			QDomElement dbEnt=dbNd.toElement();
			if (dbEnt.tagName()==QString::fromLatin1("DATABASE"))
			{
				hostname=dbEnt.attribute(QString::fromLatin1("hostname"));
				port=dbEnt.attribute(QString::fromLatin1("port"));
				driver=dbEnt.attribute(QString::fromLatin1("driver"));
				databasename=dbEnt.attribute(QString::fromLatin1("databasename"));
				username=dbEnt.attribute(QString::fromLatin1("username"));
			}
		}
		QDomNode queryNd=def.namedItem("QUERY");
		if (!queryNd.isNull())
		{
			query=queryNd.toElement().attribute(QString::fromLatin1("value"));
		}
        }

        defNd=parentElem.namedItem("SAMPLERECORD");
        if (!defNd.isNull())
	{
		QDomElement def1=defNd.toElement();
	        for (QDomElement defEnt=defNd.firstChild().toElement();!defEnt.isNull();defEnt=defEnt.nextSibling().toElement())
	        {
	               addSampleRecordEntry(defEnt.attribute(QString::fromLatin1("name")));
	        }
	}
}

bool KWQTSQLPowerSerialDataSource::showConfigDialog(QWidget *par,int action)
{
   bool ret=false;
   if (action==KWSLEdit)
   {
        if ((!database) || (!database->isOpen()))openDatabase();
	KWQTSQLPowerMailMergeEditor *dia=new KWQTSQLPowerMailMergeEditor(par,this);
	ret=dia->exec();
	delete dia;
   }
	else ret=KWQTSQLSerialDataSourceBase::showConfigDialog(par,action);
   return ret;
}

void KWQTSQLPowerSerialDataSource::clearSampleRecord() {sampleRecord.clear();}

void KWQTSQLPowerSerialDataSource::addSampleRecordEntry(QString name)
{sampleRecord[name]=name; }//i18n("No Value");}


/******************************************************************
 *
 * Class: KWQTSQLMailMergeEditor
 *
 ******************************************************************/

KWQTSQLPowerMailMergeEditor::KWQTSQLPowerMailMergeEditor( QWidget *parent, KWQTSQLPowerSerialDataSource *db_ )
	:KDialogBase( Plain, i18n( "Mail Merge - Editor" ), Ok | Cancel, Ok, parent, "", true ), db( db_ )
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

void KWQTSQLPowerMailMergeEditor::slotSetQuery()
{
	db->query=widget->query->text();
	db->refresh(true);
}

void KWQTSQLPowerMailMergeEditor::slotExecute()
{
	if (!db->database) if (!db->openDatabase()) return;
	QString tmp=widget->query->text().upper();
	if (!tmp.startsWith("SELECT")) return;
	QMySqlCursor *cur=new QMySqlCursor(widget->query->text(),true,db->database);
	cur->setMode(QSqlCursor::ReadOnly);

	db->clearSampleRecord();
	kdDebug()<<QString("Fieldname count %1").arg(cur->count())<<endl;
	for (uint i=0;i<cur->count();i++)
		db->addSampleRecordEntry(cur->fieldName(i));

	widget->queryresult->setSqlCursor(cur,true,true);
	widget->queryresult->refresh(QDataTable::RefreshAll);
}

void KWQTSQLPowerMailMergeEditor::slotTableChanged ( QListBoxItem * item )
{
	widget->fields->clear();
	if (item)
	{
		if (!db->database) return;
		QSqlRecord rec=db->database->record(item->text());
		for (uint i=0;i<rec.count();i++)
		{
			widget->fields->insertItem(rec.fieldName(i));
		}
	}
}

void KWQTSQLPowerMailMergeEditor::openSetup()
{
        KWQTSQLMailMergeOpen *dia=new KWQTSQLMailMergeOpen(this,db);
        if (dia->exec())
	{
		 db->openDatabase();
		 updateDBViews();
	}
        delete dia;
}


void KWQTSQLPowerMailMergeEditor::updateDBViews()
{
	widget->fields->clear();
	widget->tables->clear();
	if (!db->database) return;
	widget->tables->insertStringList(db->database->tables());
}

KWQTSQLPowerMailMergeEditor::~KWQTSQLPowerMailMergeEditor(){;}


extern "C" {
        KWORD_MAILMERGE_EXPORT KWMailMergeDataSource *create_kwmailmerge_qtsqldb_power(KInstance *inst,QObject *parent)
        {
                return new KWQTSQLPowerSerialDataSource(inst,parent);
        }
}
