/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <klocale.h>
#include <kgenericfactory.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kio/netaccess.h>

#include <qtable.h>
#include <qwidget.h>
#include <qcombobox.h>
#include <qheader.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include "kexiDB/kexidb.h"
#include "kexiDB/kexidbfield.h"
#include "kexiDB/kexidbrecordset.h"

#include "preview.h"
#include "kexicsvimport.h"

#include "core/filters/kexifiltermanager.h"
#include "core/kexiproject.h"

KexiCSVImport::KexiCSVImport(QObject *parent, const char *name, const QStringList &l)
 : KexiFilter(KEXIFILTERMANAGER(parent), name,l)
{
}

QString
KexiCSVImport::name()
{
	return i18n("csv file");
}


bool
KexiCSVImport::import(const KURL& url,unsigned long allowedTypes)
{
	m_db = filterManager()->project()->db();

	QString tmpFile;
     	if( ! KIO::NetAccess::download( url, tmpFile ) )
	{
#ifndef Q_WS_WIN
#warning DISPLAY SOME ERROR
#endif
		return false;
	}

	m_dlg = new ImportDlg();
	connect(m_dlg->delimiter, SIGNAL(activated(const QString &)), this, SLOT(reparse(const QString &)));
	connect(m_dlg->delimiter, SIGNAL(textChanged(const QString &)), this, SLOT(reparse(const QString &)));
	connect(m_dlg->nameInRow, SIGNAL(clicked()), this, SLOT(reparse()));
	connect(m_dlg->preview, SIGNAL(currentChanged(int, int)), this, SLOT(colChanged(int, int)));
	connect(m_dlg->field, SIGNAL(textChanged(const QString &)), this, SLOT(headerChanged(const QString &)));
	connect(m_dlg->datatype, SIGNAL(activated(int)), this, SLOT(typeChanged(int)));

	connect(m_dlg->btnImport, SIGNAL(clicked()), this, SLOT(import()));


	bool result=false;

	m_file=tmpFile;

	if(parseFile(m_file, m_dlg))
	{
		result=m_dlg->exec();
	}

	KIO::NetAccess::removeTempFile( tmpFile );

	return result;
}

bool
KexiCSVImport::parseFile(const QString &file, ImportDlg *p)
{


	QFile f(file);
	if(!f.exists() || !f.open(IO_ReadOnly))
	{
		KMessageBox::sorry(0, i18n("Coudn't open file"), i18n("CSV import"));
		return false;
	}

	QTextStream stream(&f);
	QString line;
	int row=0;
	while(!stream.eof())
	{
		line = stream.readLine();
		QStringList columns = QStringList::split(p->delimiter->currentText(), line, true);

		if(p->preview->numCols() <= columns.count())
		{
			int no = columns.count() - p->preview->numCols();
			int cols = p->preview->numCols();
			p->preview->insertColumns(cols, no);
			for(int i=cols; i < no; i++)
			{
				p->preview->horizontalHeader()->setLabel(i, "Field_" + QString::number(i));
			}
		}

		p->preview->insertRows(row);

		int col=0;
		for(QStringList::Iterator it = columns.begin(); it != columns.end(); ++it)
		{
			if(row == 0 && p->nameInRow->isChecked())
			{
				p->preview->horizontalHeader()->setLabel(col, *it);
				if(col == 0)
					p->preview->removeRow(row);
			}
			else
			{
				p->preview->setText(row, col, *it);
			}
			col++;
		}

		row++;
	}

	return true;
}

void
KexiCSVImport::reparse()
{
	QTable *t = m_dlg->preview;
	for(int c=t->numCols(); t->numCols() > 0; c--)
	{
		t->removeColumn(c);
	}

	for(int r=t->numRows(); t->numRows() > 0; r--)
	{
		t->removeRow(r);
	}

	kdDebug() << "KexiCSVImport::reparse(): clean done!" << endl;
	parseFile(m_file, m_dlg);
}

void
KexiCSVImport::reparse(const QString &)
{
	reparse();
}

void
KexiCSVImport::colChanged(int row, int col)
{
	m_dlg->curCol->setValue(col + 1);
	m_dlg->field->setText(m_dlg->preview->horizontalHeader()->label(col));
	m_dlg->datatype->setCurrentItem(m_dataTypes[col]);
}

void
KexiCSVImport::headerChanged(const QString &text)
{
	m_dlg->preview->horizontalHeader()->setLabel(m_dlg->preview->currentColumn(), text);
}

void
KexiCSVImport::typeChanged(int type)
{
	kdDebug() << "KexiCSVImport::typeChanged(): " << m_dlg->preview->currentColumn() << "," << type << endl;
	m_dataTypes.insert(m_dlg->preview->currentColumn(), type, true);
}


void
KexiCSVImport::import()
{
	if(m_dlg->table->text().isEmpty())
	{
		KMessageBox::sorry(0, i18n("Please enter a tablename"), i18n("CSV import"));
		return;
	}

	KexiDBTableStruct *table = new KexiDBTableStruct();
	KexiDBField *fieldID = new KexiDBField(m_dlg->table->text());
	fieldID->setName("ID");
	fieldID->setColumnType(KexiDBField::SQLInteger);
	fieldID->setPrimaryKey(true);
	if(!m_db->createField(*fieldID, *table, true))
		return;

	table->append(fieldID);

	for(int i=0; i < m_dlg->preview->numCols(); i++)
	{
		KexiDBField *field = new KexiDBField(m_dlg->table->text());
		field->setName(m_dlg->preview->horizontalHeader()->label(i));

		switch(m_dataTypes[i])
		{
			case 0:
				field->setColumnType(KexiDBField::SQLVarchar);
				field->setLength(255);
				break;
			case 1:
				field->setColumnType(KexiDBField::SQLInteger);
				field->setLength(20);
				break;
			case 2:
				field->setColumnType(KexiDBField::SQLDouble);
				field->setLength(20);
				break;
		}

		if(!m_db->createField(*field, *table))
			return;

		table->append(field);
	}

	fieldID->setAutoIncrement(true);
	m_db->alterField(*fieldID, 0, *table);


	KexiDBRecordSet *rec = m_db->queryRecord("SELECT * FROM " + m_dlg->table->text(), true);
	if(!rec)
		return;

	for(int row=0; row < m_dlg->preview->numRows(); row++)
	{
		KexiDBUpdateRecord *ur=rec->insert();
		if (!ur)
		{
			//show a nice error
			delete rec;
			return;
		}
		for(int col=0; col < m_dlg->preview->numCols(); col++)
		{
			ur->setValue(col + 1, QVariant(m_dlg->preview->text(row, col)));
		}
	}
	rec->writeOut();
	delete rec;

//	m_dlg->accept();
	m_dlg->hide();
}

KexiCSVImport::~KexiCSVImport()
{
}

K_EXPORT_COMPONENT_FACTORY(kexicsvimport, KGenericFactory<KexiCSVImport>)

#include "kexicsvimport.moc"
