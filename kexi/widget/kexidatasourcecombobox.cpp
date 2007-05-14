/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexidatasourcecombobox.h"

#include <QPixmap>
#include <kdebug.h>
#include <kiconloader.h>

#include <kexi.h>
#include <kexiproject.h>
#include <kexipart.h>
#include <kexipartmanager.h>
#include <kexipartinfo.h>
#include <kexipartitem.h>

#include <kexidb/connection.h>

#ifdef KEXI_SHOW_UNIMPLEMENTED
#define ADD_DEFINEQUERY_ROW
#endif

//! @internal
class KexiDataSourceComboBox::Private
{
	public:
		Private()
		 : tablesCount(0)
		 , prevIndex(-1)
		 , showTables(true)
		 , showQueries(true)
		{
		}
		int firstTableIndex() const {
			int index = 1; //skip empty row
#ifdef ADD_DEFINEQUERY_ROW
			index++; /*skip 'define query' row*/
#endif
			return index;
		}
		int firstQueryIndex() const {
			return firstTableIndex() + tablesCount;
		}

		QPointer<KexiProject> prj;
		QPixmap tableIcon, queryIcon;
		int tablesCount;
		int prevIndex; //!< Used in slotActivated()
		bool showTables : 1;
		bool showQueries : 1;
};

//------------------------

KexiDataSourceComboBox::KexiDataSourceComboBox(QWidget *parent)
 : KComboBox(true/*rw*/, parent)
 , d(new Private())
{
	setInsertPolicy(NoInsert);
	setCompletionMode(KGlobalSettings::CompletionPopupAuto);
	setMaxVisibleItems( 16 );
	connect(this, SIGNAL(activated(int)), this, SLOT(slotActivated(int)));
	connect(this, SIGNAL(returnPressed(const QString &)),
		this, SLOT(slotReturnPressed(const QString &)));

	d->tableIcon = SmallIcon("table");
	d->queryIcon = SmallIcon("query");
}

KexiDataSourceComboBox::~KexiDataSourceComboBox()
{
	delete d;
}

KexiProject* KexiDataSourceComboBox::project() const
{
	return d->prj;
}

void KexiDataSourceComboBox::setProject(KexiProject *prj, bool showTables, bool showQueries)
{
	if ((KexiProject*)d->prj == prj)
		return;

	if (d->prj) {
		disconnect(d->prj, 0, this, 0);
	}
	d->prj = prj;
	d->showTables = showTables;
	d->showQueries = showQueries;
	clear();
	d->tablesCount = 0;
	if (!d->prj)
		return;

	//needed for updating contents of the combo box
	connect(d->prj, SIGNAL(newItemStored(KexiPart::Item&)),
		this, SLOT(slotNewItemStored(KexiPart::Item&)));
	connect(d->prj, SIGNAL(itemRemoved(const KexiPart::Item&)),
		this, SLOT(slotItemRemoved(const KexiPart::Item&)));
	connect(d->prj, SIGNAL(itemRenamed(const KexiPart::Item&, const QString&)),
		this, SLOT(slotItemRenamed(const KexiPart::Item&, const QString&)));

	KexiDB::Connection *conn = d->prj->dbConnection();
	if (!conn)
		return;

	//special item: empty
	addItem("");
#ifdef ADD_DEFINEQUERY_ROW
	//special item: define query
	addItem(i18n("Define Query..."));
#endif

	KCompletion *comp = completionObject();

	if (d->showTables) {
		//tables
		KexiPart::Info* partInfo = Kexi::partManager().infoForMimeType("kexi/table");
		if (!partInfo)
			return;
		KexiPart::ItemList list;
		prj->getSortedItems(list, partInfo);
		list.sort();
		d->tablesCount = 0;
		for (KexiPart::ItemListIterator it(list); it.current(); ++it, d->tablesCount++) {
			addItem(d->tableIcon, it.current()->name()); //or caption()? 
			comp->addItem(it.current()->name());
		}
	}

	if (d->showQueries) {
		//queries
		KexiPart::Info* partInfo = Kexi::partManager().infoForMimeType("kexi/query");
		if (!partInfo)
			return;
		KexiPart::ItemList list;
		prj->getSortedItems(list, partInfo);
		list.sort();
		for (KexiPart::ItemListIterator it(list); it.current(); ++it) {
			addItem(d->queryIcon, it.current()->name()); //or caption()? 
			comp->addItem(it.current()->name());
		}
	}
//	setCurrentText("");
	setCurrentIndex(0);
}

void KexiDataSourceComboBox::setDataSource(const QString& mimeType, const QString& name)
{
	if (name.isEmpty()) {
		clearEditText();
		setCurrentIndex(0);
		d->prevIndex = -1;
		emit dataSourceChanged();
		return;
	}

	QString mt(mimeType);
	if (mimeType.isEmpty())
		mt="kexi/table";
	int i = findItem(mt, name);
	if (i==-1) {
		if (mimeType.isEmpty())
			i = findItem("kexi/query", name);
		if (i==-1) {
			setCurrentIndex(0);
			return;
		}
	}
	setCurrentIndex(i);
	slotActivated(i);
}

void KexiDataSourceComboBox::slotNewItemStored(KexiPart::Item& item)
{
	QString name(item.name());
	//insert a new item, maintaining sort order and splitting to tables and queries
	if (item.mimeType()=="kexi/table") {
		int i = 1; /*skip empty row*/
#ifdef ADD_DEFINEQUERY_ROW
		i++; /*skip 'define query' row*/
#endif
		for (; i < d->firstQueryIndex() && name>=itemText(i); i++)
			;
		addItem(d->tableIcon, name, i);
		completionObject()->addItem(name);
		d->tablesCount++;
	}
	else if (item.mimeType()=="kexi/query") {
		int i;
		for (i=d->firstQueryIndex(); i<count() && name>=itemText(i); i++)
			;
		addItem(d->queryIcon, name, i);
		completionObject()->addItem(name);
	}
}

int KexiDataSourceComboBox::findItem(const QString& mimeType, const QString& name)
{
	int i, end;
	if (mimeType=="kexi/table") {
		i = 0;
#ifdef ADD_DEFINEQUERY_ROW
		i++; //skip 'define query'
#endif
		end = d->firstQueryIndex();
	}
	else if (mimeType=="kexi/query") {
		i = d->firstQueryIndex();
		end = count();
	}
	else
		return -1;

	QString nameString(name);

	for (; i<end; i++)
		if (itemText(i)==nameString)
			return i;
	
	return -1;
}

void KexiDataSourceComboBox::slotItemRemoved(const KexiPart::Item& item)
{
	const int i = findItem(item.mimeType(), item.name());
	if (i==-1)
		return;
	removeItem(i);
	completionObject()->removeItem(item.name());
	if (item.mimeType()=="kexi/table")
		d->tablesCount--;
#if 0 //disabled because even invalid data source can be set
	if (currentItem()==i) {
		if (i==(count()-1))
			setCurrentItem(i-1);
		else
			setCurrentItem(i);
	}
#endif
}

void KexiDataSourceComboBox::slotItemRenamed(const KexiPart::Item& item, const QString& oldName)
{
	const int i = findItem(item.mimeType(), QString(oldName));
	if (i==-1)
		return;
	setItemText(i, item.name());
	completionObject()->removeItem(oldName);
	completionObject()->addItem(item.name());
	setEditText(oldName); //still keep old name
}

void KexiDataSourceComboBox::slotActivated( int index )
{
	if (index >= d->firstTableIndex() && index < count() && d->prevIndex!=currentIndex()) {
		d->prevIndex = currentIndex();
		emit dataSourceChanged();
	}
}

QString KexiDataSourceComboBox::selectedMimeType() const
{
	if (selectedName().isEmpty())
		return "";
	const int index = currentIndex();
	if (index >= d->firstTableIndex() && index < (int)d->firstQueryIndex())
		return "kexi/table";
	else if (index >= (int)d->firstQueryIndex() && index < count())
		return "kexi/query";
	return "";
}

QString KexiDataSourceComboBox::selectedName() const
{
	if (isSelectionValid())
		return itemText(currentIndex());
	return currentText();
}

bool KexiDataSourceComboBox::isSelectionValid() const
{
	const int index = currentIndex();
	return index >= d->firstTableIndex() && index < count() && itemText(index)==currentText();
}

void KexiDataSourceComboBox::slotReturnPressed(const QString & text)
{
	//text is available: select item for this text:
	bool changed = false;
	if (text.isEmpty() && 0!=currentIndex()) {
		setCurrentIndex(0);
		changed = true;
	}
	else {
		const int index = findText( text, Qt::MatchExactly );
		if (index>=0 && index!=currentIndex()) {
			setCurrentIndex( index );
			changed = true;
		}
	}
	if (changed)
		emit dataSourceChanged();
}

void KexiDataSourceComboBox::focusOutEvent( QFocusEvent *e )
{
	KComboBox::focusOutEvent( e );
	slotReturnPressed(currentText());
}

#include "kexidatasourcecombobox.moc"
