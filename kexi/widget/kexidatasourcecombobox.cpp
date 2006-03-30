/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include <kdebug.h>
#include <kiconloader.h>
#include <klistbox.h>

#include <kexi.h>
#include <kexiproject.h>
#include <keximainwindow.h>
#include <kexipart.h>
#include <kexipartmanager.h>
#include <kexipartinfo.h>
#include <kexipartitem.h>

#include <kexidb/connection.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3CString>

#ifdef KEXI_SHOW_UNIMPLEMENTED
#define ADD_DEFINEQUERY_ROW
#endif

//! @internal
class KexiDataSourceComboBox::Private
{
	public:
		Private()
		 : tablesCount(0)
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
};

//------------------------

KexiDataSourceComboBox::KexiDataSourceComboBox(QWidget *parent, const char *name)
 : KComboBox(true/*rw*/, parent, name)
 , d(new Private())
{
	setInsertPolicy(NoInsert);
	setCompletionMode(KGlobalSettings::CompletionPopupAuto);
	setSizeLimit( 16 );
	connect(this, SIGNAL(activated(int)), this, SLOT(slotActivated(int)));

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

void KexiDataSourceComboBox::setProject(KexiProject *prj)
{
	if ((KexiProject*)d->prj == prj)
		return;

	if (d->prj) {
		disconnect(d->prj, 0, this, 0);
	}
	d->prj = prj;
	clear();
	d->tablesCount = 0;
	if (!d->prj)
		return;

	//needed for updating contents of the combo box
	connect(d->prj, SIGNAL(newItemStored(KexiPart::Item&)),
		this, SLOT(slotNewItemStored(KexiPart::Item&)));
	connect(d->prj, SIGNAL(itemRemoved(const KexiPart::Item&)),
		this, SLOT(slotItemRemoved(const KexiPart::Item&)));
	connect(d->prj, SIGNAL(itemRenamed(const KexiPart::Item&, const Q3CString&)),
		this, SLOT(slotItemRenamed(const KexiPart::Item&, const Q3CString&)));

	KexiDB::Connection *conn = d->prj->dbConnection();
	if (!conn)
		return;

	//special item: empty
	insertItem("");
#ifdef ADD_DEFINEQUERY_ROW
	//special item: define query
	insertItem(i18n("Define Query..."));
#endif

	KCompletion *comp = completionObject();

	//tables
	KexiPart::Info* partInfo = Kexi::partManager().infoForMimeType("kexi/table");
	if (!partInfo)
		return;
	KexiPart::ItemList list;
	prj->getSortedItems(list, partInfo);
	list.sort();
	d->tablesCount = 0;
	for (KexiPart::ItemListIterator it(list); it.current(); ++it, d->tablesCount++) {
		insertItem(d->tableIcon, it.current()->name()); //or caption()? 
		comp->addItem(it.current()->name());
	}

	//queries
	partInfo = Kexi::partManager().infoForMimeType("kexi/query");
	if (!partInfo)
		return;
	prj->getSortedItems(list, partInfo);
	list.sort();
	for (KexiPart::ItemListIterator it(list); it.current(); ++it) {
		insertItem(d->queryIcon, it.current()->name()); //or caption()? 
		comp->addItem(it.current()->name());
	}
//	setCurrentText("");
	setCurrentItem(0);
}

void KexiDataSourceComboBox::setDataSource(const Q3CString& mimeType, const Q3CString& name)
{
	if (name.isEmpty()) {
		clearEdit();
		setCurrentItem(0);
		emit dataSourceSelected();
		return;
	}

	Q3CString mt(mimeType);
	if (mimeType.isEmpty())
		mt="kexi/table";
	int i = findItem(mt, name);
	if (i==-1) {
		if (mimeType.isEmpty())
			i = findItem("kexi/query", name);
		if (i==-1)
			return;
	}
	setCurrentItem(i);
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
		for (; i < d->firstQueryIndex() && name>=text(i); i++)
			;
		insertItem(d->tableIcon, name, i);
		completionObject()->addItem(name);
		d->tablesCount++;
	}
	else if (item.mimeType()=="kexi/query") {
		int i;
		for (i=d->firstQueryIndex(); i<count() && name>=text(i); i++)
			;
		insertItem(d->queryIcon, name, i);
		completionObject()->addItem(name);
	}
}

int KexiDataSourceComboBox::findItem(const Q3CString& mimeType, const Q3CString& name)
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
		if (text(i)==nameString)
			return i;
	
	return -1;
}

void KexiDataSourceComboBox::slotItemRemoved(const KexiPart::Item& item)
{
	const int i = findItem(item.mimeType(), item.name().latin1());
	if (i==-1)
		return;
	removeItem(i);
	completionObject()->removeItem(item.name());
	if (item.mimeType()=="kexi/table")
		d->tablesCount--;
	if (currentItem()==i) {
		if (i==(count()-1))
			setCurrentItem(i-1);
		else
			setCurrentItem(i);
	}
/*	if (item.mimeType()=="kexi/table" || item.mime()=="kexi/query") {
		QString name(item.name());
		uint i, end;
		if (item.mimeType()=="kexi/table") {
			i = 1; //skip 'define query'
			end = 1+d->tablesCount;
		}
		else { //kexi/query
			i = 1+d->tablesCount;
			end = count();
		}
		for (; i<end; i++) {
			if (text(i)==name) {
				removeItem(i);
				completionObject()->removeItem(name);
				if (item.mimeType()=="kexi/table")
					d->tablesCount--;
				if (currentItem()==i) {
					if (i==(count()-1))
						setCurrentItem(i-1);
					else
						setCurrentItem(i);
				}
				break;
			}
		}
	}*/
}

void KexiDataSourceComboBox::slotItemRenamed(const KexiPart::Item& item, const Q3CString& oldName)
{
	const int i = findItem(item.mimeType(), oldName);
	if (i==-1)
		return;
	changeItem(item.name(), i);
	completionObject()->removeItem(QString(oldName));
	completionObject()->addItem(item.name());
/*	if (item.mimeType()=="kexi/table" || item.mime()=="kexi/query") {
		QString oldStrName(oldName);
		uint i, end;
		if (item.mimeType()=="kexi/table") {
			i = 1; //skip 'define query'
			end = 1+d->tablesCount;
		}
		else { //kexi/query
			i = 1+d->tablesCount;
			end = count();
		}
		for (; i<end; i++) {
			if (text(i)==oldStrName) {
				changeItem(item.name(), i);
				completionObject()->removeItem(oldStrName);
				completionObject()->addItem(item.name());
				break;
			}
		}
	}*/
}

void KexiDataSourceComboBox::slotActivated( int index )
{
	if (index >= d->firstTableIndex() && index < count())
		emit dataSourceSelected();
}

Q3CString KexiDataSourceComboBox::selectedMimeType() const
{
	const int index = currentItem();
	if (index >= d->firstTableIndex() && index < (int)d->firstQueryIndex())
		return "kexi/table";
	else if (index >= (int)d->firstQueryIndex() && index < count())
		return "kexi/query";
	return 0;
}

Q3CString KexiDataSourceComboBox::selectedName() const
{
	const int index = currentItem();
	if (index >= d->firstTableIndex() && index < count())
		return text(index).latin1();
	return 0;
}

#include "kexidatasourcecombobox.moc"
