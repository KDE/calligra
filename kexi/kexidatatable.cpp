/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include <qsqlquery.h>
#include <qsqlrecord.h>
#include <qsqlcursor.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qvariant.h>

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "kexiapplication.h"

#include "kexidatatable.h" 
 
KexiDataTable::KexiDataTable(QWidget *parent, QString caption, const char *name)
	: KexiTableView(parent, name)
{
	setCaption(i18n(caption + " - table"));

	if(width() > parent->width())
	{
		kdDebug() << "the width is higher..." << endl;
		setBaseSize(parent->width(), height());
	}

	if(height() > parent->height())
	{
		setBaseSize(width(), parent->height());
	}

	connect(this, SIGNAL(itemChanged(KexiTableItem *, int)), this, SLOT(slotItemChanged(KexiTableItem *, int)));
}

bool
KexiDataTable::executeQuery(QString queryStatement)
{
	QSqlDatabase *db = kexi->project()->db();
	QSqlQuery query(queryStatement);

	QSqlRecord record = db->record(query);

	QSqlError error = query.lastError();
	if(error.type() != QSqlError::None)
	{
		QString errorText = error.databaseText();
		KMessageBox::sorry(this, i18n("<qt>Error in your sql-statement:<br><br><b>" + errorText + "</b>"), i18n("Query Statement"));
		return false; 
	}

	kdDebug() << record.count() << " column(s) to execute" << endl;
	unsigned int fields = record.count();

	for (unsigned int i = 0; i < record.count(); i++)
	{
		//WARNING: look for the type!!!
		kdDebug() << "KexiDataTable::executeQuery: " << record.field(i)->name() << endl;
		
		addColumn(record.field(i)->name(), record.field(i)->type(), true);
	}

	while(query.next())
	{
		KexiTableItem *it = new KexiTableItem(this);
		for(unsigned int i=0; i < fields; i++)
		{
			it->setValue(i, query.value(i));
		}
	}
	return true;
}

void
KexiDataTable::slotItemChanged(KexiTableItem *i, int col)
{
	kdDebug() << "CHANGED!!!" << endl;
}

KexiDataTable::~KexiDataTable()
{
}

#include "kexidatatable.moc"
