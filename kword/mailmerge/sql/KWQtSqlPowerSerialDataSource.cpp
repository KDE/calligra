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

#include "KWQtSqlPowerSerialDataSource.h"
#include "KWQtSqlPowerSerialDataSource.moc"
#include "KWQtSqlMailMergeOpen.h"
#include <QLayout>
#include <qdom.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <kcombobox.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <qsqldatabase.h>
#include <QMessageBox>
#include <kpassworddialog.h>
#include <qsqlrecord.h>
#include <q3sqlcursor.h>
#include <q3datatable.h>
#include <kdebug.h>

#define KWQtSqlBarIcon( x ) BarIcon( x, db->KWInstance() )

/******************************************************************
 *
 * Class: KWQtSqlSerialDataSource
 *
 ******************************************************************/

KWQtSqlPowerSerialDataSource::KWQtSqlPowerSerialDataSource(KInstance *inst,QObject *parent)
	: KWQtSqlSerialDataSourceBase(inst,parent),myquery(0)
{
	port=i18n("default");
}

KWQtSqlPowerSerialDataSource::~KWQtSqlPowerSerialDataSource()
{
	if (myquery) delete myquery;
	QSqlDatabase::removeDatabase("KWQTSQLPOWER");
}

void KWQtSqlPowerSerialDataSource::refresh(bool force)
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
		myquery=new KWMySqlCursor(query,true,database);
		myquery->setMode(Q3SqlCursor::ReadOnly);
	}
	kDebug()<<QString("There were %1 rows in the query").arg(myquery->size())<<endl;
}

QString KWQtSqlPowerSerialDataSource::getValue( const QString &name, int record ) const
{
	int num=record;

	if (!myquery) return name;
	if ( num < 0 || num > (int)myquery->size() )
		return name;
	if (!myquery->seek(num,false)) return i18n(">>>Illegal position within datasource<<<");
	if (!myquery->contains(name)) return i18n(">>>Field %1 is unknown in the current database query<<<",name);
	return (myquery->value(name)).toString();
}

void KWQtSqlPowerSerialDataSource::save( QDomDocument &doc, QDomElement &parent)
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

void KWQtSqlPowerSerialDataSource::load( QDomElement& parentElem )
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

bool KWQtSqlPowerSerialDataSource::showConfigDialog(QWidget *par,int action)
{
   bool ret=false;
   if (action==KWSLEdit)
   {
        if ((!database) || (!database->isOpen()))openDatabase();
	KWQtSqlPowerMailMergeEditor *dia=new KWQtSqlPowerMailMergeEditor(par,this);
	ret=dia->exec();
	delete dia;
   }
	else ret=KWQtSqlSerialDataSourceBase::showConfigDialog(par,action);
   return ret;
}

void KWQtSqlPowerSerialDataSource::clearSampleRecord() {sampleRecord.clear();}

void KWQtSqlPowerSerialDataSource::addSampleRecordEntry(QString name)
{sampleRecord[name]=name; }//i18n("No Value");}


/******************************************************************
 *
 * Class: KWQtSqlMailMergeEditor
 *
 ******************************************************************/

KWQtSqlPowerMailMergeEditor::KWQtSqlPowerMailMergeEditor( QWidget *parent, KWQtSqlPowerSerialDataSource *db_ )
	:KDialogBase( Plain, i18n( "Mail Merge - Editor" ), Ok | Cancel, Ok, parent, "", true ), db( db_ )
{
        (new Q3VBoxLayout(plainPage()))->setAutoAdd(true);
        setMainWidget(widget=new KWQtSqlPowerWidget(plainPage()));
	connect(widget->setup,SIGNAL(clicked()),this,SLOT(openSetup()));
	connect(widget->tables,SIGNAL(currentChanged(Q3ListBoxItem*)),this,SLOT(slotTableChanged(Q3ListBoxItem*)));
	connect(widget->execute,SIGNAL(clicked()),this,SLOT(slotExecute()));
	connect(this,SIGNAL(okClicked()),this,SLOT(slotSetQuery()));
	widget->query->setText(db->query);
	updateDBViews();
}

void KWQtSqlPowerMailMergeEditor::slotSetQuery()
{
	db->query=widget->query->text();
	db->refresh(true);
}

void KWQtSqlPowerMailMergeEditor::slotExecute()
{
	if (!db->database) if (!db->openDatabase()) return;
	QString tmp=widget->query->text().upper();
	if (!tmp.startsWith("SELECT")) return;
	KWMySqlCursor *cur=new KWMySqlCursor(widget->query->text(),true,db->database);
	cur->setMode(Q3SqlCursor::ReadOnly);

	db->clearSampleRecord();
	kDebug()<<QString("Fieldname count %1").arg(cur->count())<<endl;
	for (uint i=0;i<cur->count();i++)
		db->addSampleRecordEntry(cur->fieldName(i));

	widget->queryresult->setSqlCursor(cur,true,true);
	widget->queryresult->refresh(Q3DataTable::RefreshAll);
}

void KWQtSqlPowerMailMergeEditor::slotTableChanged ( Q3ListBoxItem * item )
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

void KWQtSqlPowerMailMergeEditor::openSetup()
{
        KWQtSqlMailMergeOpen *dia=new KWQtSqlMailMergeOpen(this,db);
        if (dia->exec())
	{
		 db->openDatabase();
		 updateDBViews();
	}
        delete dia;
}


void KWQtSqlPowerMailMergeEditor::updateDBViews()
{
	widget->fields->clear();
	widget->tables->clear();
	if (!db->database) return;
	widget->tables->insertStringList(db->database->tables());
}

KWQtSqlPowerMailMergeEditor::~KWQtSqlPowerMailMergeEditor(){;}


extern "C" {
        KWORD_MAILMERGE_EXPORT KWMailMergeDataSource *create_kwmailmerge_qtsqldb_power(KInstance *inst,QObject *parent)
        {
                return new KWQtSqlPowerSerialDataSource(inst,parent);
        }
}
