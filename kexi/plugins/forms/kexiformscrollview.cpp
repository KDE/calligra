/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexiformscrollview.h"
#include "kexiformview.h"
#include "kexidbform.h"

#include <formeditor/form.h>
#include <formeditor/formmanager.h>
#include <formeditor/objecttree.h>

KexiFormScrollView::KexiFormScrollView(QWidget *parent, bool preview)
 : KexiScrollView(parent, preview)
 , KexiRecordNavigatorHandler()
 , KexiSharedActionClient()
 , KexiDataAwareObjectInterface()
 , KexiFormDataProvider()
{
	m_currentLocalSortColumn = -1; /* no column */
	m_localSortingOrder = -1; /* no sorting */
	m_previousItem = 0;
	m_navPanel = m_scrollViewNavPanel; //copy this pointer from KexiScrollView
	if (preview) {
		setRecordNavigatorVisible(true);
//tmp
//		recordNavigator()->setEditingIndicatorEnabled(true);
//		recordNavigator()->showEditingIndicator(true);
	}

	connect(this, SIGNAL(resizingStarted()), this, SLOT(slotResizingStarted()));
	//context menu
	m_popup = new KPopupMenu(this, "contextMenu");
//	m_provider = new KexiDataProvider();

	setFocusPolicy(NoFocus);
}

KexiFormScrollView::~KexiFormScrollView()
{
//	delete m_provider;
}

void
KexiFormScrollView::show()
{
	KexiScrollView::show();

	//now get resize mode settings for entire form
	if (m_preview) {
		KexiFormView* fv = dynamic_cast<KexiFormView*>(parent());
		int resizeMode = fv ? fv->resizeMode() : KexiFormView::ResizeAuto;
		if (resizeMode == KexiFormView::ResizeAuto)
			setResizePolicy(AutoOneFit);
	}
}

void
KexiFormScrollView::slotResizingStarted()
{
	if(m_form && m_form->manager())
		setSnapToGrid(m_form->manager()->snapWidgetsToGrid(), m_form->gridX(), m_form->gridY());
	else
		setSnapToGrid(false);
}

int KexiFormScrollView::rowsPerPage() const
{
	//! @todo
	return 10;
}

void KexiFormScrollView::selectCellInternal()
{
	//m_currentItem is already set by KexiDataAwareObjectInterface::setCursorPosition()
	if (m_currentItem) {
		if (m_currentItem!=m_previousItem) {
			fillDataItems(*m_currentItem);
			m_previousItem = m_currentItem;
		}
	}
	else
			m_previousItem = 0;
}

void KexiFormScrollView::ensureCellVisible(int row, int col/*=-1*/)
{
	//! @todo
//	if (m_currentItem)
		//fillDataItems(*m_currentItem);

//	if (m_form->tabStops()->first() && m_form->tabStops()->first()->widget())
//		m_form->tabStops()->first()->widget()->setFocus();
}

void KexiFormScrollView::moveToRecordRequested(uint r)
{
	//! @todo
}

void KexiFormScrollView::moveToLastRecordRequested()
{
	//! @todo
	selectLastRow();
}

void KexiFormScrollView::moveToPreviousRecordRequested()
{
	//! @todo
	selectPrevRow();
}

void KexiFormScrollView::moveToNextRecordRequested()
{
	//! @todo
	selectNextRow();
}

void KexiFormScrollView::moveToFirstRecordRequested()
{
	//! @todo
	selectFirstRow();
}

void KexiFormScrollView::addNewRecordRequested()
{
	//! @todo
}

void KexiFormScrollView::clearColumnsInternal(bool repaint)
{
	//! @todo
}

void KexiFormScrollView::addHeaderColumn(const QString& caption, const QString& description, int width)
{
	//! @todo
}

int KexiFormScrollView::currentLocalSortingOrder() const
{
	//! @todo
	return m_localSortingOrder;
}

int KexiFormScrollView::currentLocalSortColumn() const
{
	return m_currentLocalSortColumn;
}

void KexiFormScrollView::setLocalSortingOrder(int col, int order)
{
	//! @todo
	m_currentLocalSortColumn = col;
	m_localSortingOrder = order;
}

void KexiFormScrollView::sortColumnInternal(int col, int order)
{
	//! @todo
}

void KexiFormScrollView::updateGUIAfterSorting()
{
	//! @todo
}

void KexiFormScrollView::createEditor(int row, int col, const QString& addText, 
	bool removeOld)
{
	if (isReadOnly()) {
		kdDebug(44021) << "KexiFormScrollView::createEditor(): DATA IS READ ONLY!"<<endl;
		return;
	}
	if (column( col )->readOnly()) {
		kdDebug(44021) << "KexiFormScrollView::createEditor(): COL IS READ ONLY!"<<endl;
		return;
	}

	//! @todo
	const bool startRowEdit = !m_rowEditing; //remember if we're starting row edit

	if (!m_rowEditing) {
		//we're starting row editing session
		m_data->clearRowEditBuffer();
		
		m_rowEditing = true;
//		//indicate on the vheader that we are editing:
//		m_verticalHeader->setEditRow(m_curRow);
/*		if (isInsertingEnabled() && m_currentItem==m_insertItem) {
			//we should know that we are in state "new row editing"
			m_newRowEditing = true;
			//'insert' row editing: show another row after that:
			m_data->append( m_insertItem );
			//new empty insert item
			m_insertItem = new KexiTableItem(columns());
//			updateContents();
			m_verticalHeader->addLabel();
			d->verticalHeaderAlreadyAdded = true;
			updateWidgetContentsSize();
			//refr. current and next row
			updateContents(columnPos(0), rowPos(row), viewport()->width(), d->rowHeight*2);
//			updateContents(columnPos(0), rowPos(row+1), viewport()->width(), d->rowHeight);
//js: warning this breaks behaviour (cursor is skipping, etc.): qApp->processEvents(500);
			ensureVisible(columnPos(m_curCol), rowPos(row+1)+d->rowHeight-1, columnWidth(m_curCol), d->rowHeight);

			m_verticalHeader->setOffset(contentsY());
		}*/
	}	

	m_editor = editor(col); //m_dataItems.at(col);
	if (!m_editor)
		return;

	if (startRowEdit) {
		//! @todo hide indicator when editing is finished
		recordNavigator()->showEditingIndicator(true);

		emit rowEditStarted(m_curRow);
	}
}

KexiDataItemInterface *KexiFormScrollView::editor( int col, bool ignoreMissingEditor )
{
	if (!m_data || col<0 || col>=columns())
		return 0;

	return dynamic_cast<KexiFormDataItemInterface*>(dbFormWidget()->orderedDataAwareWidgets()->at( col ));
//	KexiFormDataItemInterface *item = m_dataItems.at(col);
	//return item;

/*
	KexiTableViewColumn *tvcol = m_data->column(col);
//	int t = tvcol->field->type();

	//find the editor for this column
	KexiDataItemInterface *editor = d->editors[ tvcol ];
	if (editor)
		return editor;

	//not found: create
//	editor = KexiCellEditorFactory::createEditor(*m_data->column(col)->field, this);
	editor = KexiCellEditorFactory::createEditor(*m_data->column(col), this);
	if (!editor) {//create error!
		if (!ignoreMissingEditor) {
			//js TODO: show error???
			cancelRowEdit();
		}
		return 0;
	}
	editor->hide();
	connect(editor,SIGNAL(editRequested()),this,SLOT(slotEditRequested()));
	connect(editor,SIGNAL(cancelRequested()),this,SLOT(cancelEditor()));
	connect(editor,SIGNAL(acceptRequested()),this,SLOT(acceptEditor()));

	editor->resize(columnWidth(col)-1, rowHeight()-1);
	editor->installEventFilter(this);
	if (editor->widget())
		editor->widget()->installEventFilter(this);
	//store
	d->editors.insert( tvcol, editor );
	return editor;*/
}

void KexiFormScrollView::editorShowFocus( int row, int col )
{
	//! @todo
//	if (m_currentItem)
//		m_provider->fillDataItems(*m_currentItem);
}

void KexiFormScrollView::updateCell(int row, int col)
{
	//! @todo
}

void KexiFormScrollView::updateRow(int row)
{
	//! @todo
}

void KexiFormScrollView::updateWidgetContents()
{
	//! @todo
}

void KexiFormScrollView::updateWidgetContentsSize()
{
	//! @todo
}

void KexiFormScrollView::updateWidgetScrollBars()
{
	//! @todo
}

void KexiFormScrollView::slotRowRepaintRequested(KexiTableItem& item)
{
	//! @todo
}

void KexiFormScrollView::slotAboutToDeleteRow(KexiTableItem& item, 
	KexiDB::ResultInfo* result, bool repaint)
{
	//! @todo
}

void KexiFormScrollView::slotRowDeleted()
{
	//! @todo
}

void KexiFormScrollView::slotRowInserted(KexiTableItem *item, bool repaint)
{
	//! @todo
}

void KexiFormScrollView::slotRowInserted(KexiTableItem *item, uint row, bool repaint)
{
	//! @todo
}

void KexiFormScrollView::slotRowsDeleted( const QValueList<int> & )
{
	//! @todo
}

KexiDBForm* KexiFormScrollView::dbFormWidget() const
{
	return dynamic_cast<KexiDBForm*>(m_widget);
}

int KexiFormScrollView::columns() const
{
	return dbFormWidget()->orderedDataAwareWidgets()->count(); //m_dataItems.count();
}

uint KexiFormScrollView::fieldNumberForColumn(int col)
{
	KexiFormDataItemInterface *item = dynamic_cast<KexiFormDataItemInterface*>(dbFormWidget()->orderedDataAwareWidgets()->at( col ));
	if (!item)
		return -1;
	KexiFormDataItemInterfaceToIntMap::ConstIterator it(m_fieldNumbersForDataItems.find( item ));
	return it!=m_fieldNumbersForDataItems.constEnd() ? it.data() : -1;
}

bool KexiFormScrollView::columnEditable(int col)
{
	kdDebug() << "KexiFormScrollView::columnEditable(" << col << ")" << endl;
	foreach_list (QPtrListIterator<KexiFormDataItemInterface>, it, m_dataItems) {
		kdDebug() << (dynamic_cast<QWidget*>(it.current()) ? dynamic_cast<QWidget*>(it.current())->name() : "" ) 
			<< it.current()->dataSource() << " " << endl;
	}
	kdDebug() << "-- focus widgets --" << endl;
	foreach_list (QPtrListIterator<QWidget>, it, *dbFormWidget()->orderedFocusWidgets()) {
		kdDebug() << it.current()->name() << endl;
	}
	kdDebug() << "-- data-aware widgets --" << endl;
	foreach_list (QPtrListIterator<QWidget>, it, *dbFormWidget()->orderedDataAwareWidgets()) {
		kdDebug() << it.current()->name() << endl;
	}

	//int index = dbFormWidget()->indexForDataItem( item );
//	KexiFormDataItemInterface *item1 = dynamic_cast<KexiFormDataItemInterface*>(dbFormWidget()->orderedFocusWidgets()->at( col ));
	KexiFormDataItemInterface *item = dynamic_cast<KexiFormDataItemInterface*>(dbFormWidget()->orderedDataAwareWidgets()->at( col ));

	if (!item || item->isReadOnly())
		return false;

	KexiFormDataItemInterfaceToIntMap::ConstIterator it(m_fieldNumbersForDataItems.find( item ));
	return KexiDataAwareObjectInterface::columnEditable( it!=m_fieldNumbersForDataItems.constEnd() ? it.data() : -1 );

//	KexiFormDataItemInterface *item = m_dataItems.at(col);
//	return item && !item->isReadOnly() && KexiDataAwareObjectInterface::columnEditable(col);
}

void KexiFormScrollView::valueChanged(KexiDataItemInterface* item)
{
	if (!item)
		return;
	//only signal start editing when no row editing was started already
	if (dbFormWidget()->editedItem!=item) {
		dbFormWidget()->editedItem = dynamic_cast<KexiFormDataItemInterface*>(item);
		startEditCurrentCell();
	}
	fillDuplicatedDataItems(dynamic_cast<KexiFormDataItemInterface*>(item), item->value());
}

void KexiFormScrollView::initDataContents()
{
	KexiDataAwareObjectInterface::initDataContents();

	recordNavigator()->setEditingIndicatorEnabled( !isReadOnly() );
	recordNavigator()->showEditingIndicator(false);
}

KexiTableViewColumn* KexiFormScrollView::column(int col)
{
	const uint id = fieldNumberForColumn(col);
	return (id >= 0) ? m_data->column( id ) : 0;
}

#include "kexiformscrollview.moc"
