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

#include <formeditor/form.h>
#include <formeditor/formmanager.h>

KexiFormScrollView::KexiFormScrollView(QWidget *parent, bool preview)
 : KexiScrollView(parent, preview)
 , KexiRecordNavigatorHandler()
 , KexiSharedActionClient()
 , KexiDataAwareObjectInterface()
{
	m_currentLocalSortColumn = -1; /* no column */
	m_localSortingOrder = -1; /* no sorting */
	m_navPanel = m_scrollViewNavPanel; //copy this pointer from KexiScrollView
	if(preview)
		setRecordNavigatorVisible(true);
	connect(this, SIGNAL(resizingStarted()), this, SLOT(slotResizingStarted()));
	//context menu
	m_popup = new KPopupMenu(this, "contextMenu");
}

KexiFormScrollView::~KexiFormScrollView()
{
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

void KexiFormScrollView::connectCellSelectedSignal(const QObject* receiver, const char* intIntMember)
{
	connect(this, SIGNAL(cellSelected(int,int)), receiver, intIntMember);
}

void KexiFormScrollView::connectRowEditStartedSignal(const QObject* receiver, const char* intMember)
{
}

void KexiFormScrollView::connectRowEditTerminatedSignal(const QObject* receiver, const char* voidMember)
{
}

void KexiFormScrollView::connectReloadActionsSignal(const QObject* receiver, const char* voidMember)
{
}

void KexiFormScrollView::connectDataSetSignal(const QObject* receiver, const char* kexiTableViewDataMember)
{
}

void KexiFormScrollView::connectToReloadDataSlot(const QObject* sender, const char* voidSignal)
{
}

int KexiFormScrollView::rowsPerPage() const
{
	return 10;
}

void KexiFormScrollView::ensureCellVisible(int row, int col/*=-1*/)
{
	//! @todo
}

void KexiFormScrollView::moveToRecordRequested(uint r)
{
	//! @todo
}

void KexiFormScrollView::moveToLastRecordRequested()
{
	//! @todo
}

void KexiFormScrollView::moveToPreviousRecordRequested()
{
	//! @todo
}

void KexiFormScrollView::moveToNextRecordRequested()
{
	//! @todo
}

void KexiFormScrollView::moveToFirstRecordRequested()
{
	//! @todo
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

void KexiFormScrollView::reloadActions()
{
	//! @todo
}

void KexiFormScrollView::createEditor(int row, int col, const QString& addText, 
	bool removeOld)
{
	//! @todo
}

KexiTableEdit *KexiFormScrollView::editor( int col, bool ignoreMissingEditor )
{
	//! @todo
	return 0;
}

void KexiFormScrollView::editorShowFocus( int row, int col )
{
	//! @todo
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

#include "kexiformscrollview.moc"
