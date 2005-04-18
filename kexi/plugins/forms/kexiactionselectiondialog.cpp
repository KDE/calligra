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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexiactionselectiondialog.h"

#include <widget/utils/klistviewitemtemplate.h>
#include <keximainwindow.h>

#include <klistview.h>
#include <kaction.h>
#include <kiconloader.h>

#include <qbitmap.h>
#include <qlabel.h>
#include <qheader.h>
#include <qvbox.h>

typedef KListViewItemTemplate<QCString> ActionSelectorDialogListItem;

KexiActionSelectionDialog::KexiActionSelectionDialog(KexiMainWindow* mainWin, QWidget *parent, 
	const QCString& _currentActionName, const QCString& actionWidgetName)
 : KDialogBase(parent, "actionSelectorDialog", true, QString::null, 
	KDialogBase::Ok | KDialogBase::Cancel )
{
	KActionPtrList sharedActions( mainWin->sharedActions() );

	QCString currentActionName( _currentActionName );

	//FOR FUTURE COMPATIBILITY:
	//check if this is kaction-type action
	if (!currentActionName.isEmpty()) {
		if (QString(currentActionName).startsWith("kaction:")) {
			currentActionName = currentActionName.mid(QCString("kaction:").length()); //cut prefix
		} else {
/*! @todo handle other types of action handlers here (eg. script event handler or function)! */
			currentActionName = ""; //for now we're pretending there was no action assigned
		}
	}

	QVBox *vbox = makeVBoxMainWidget();
	new QLabel(i18n("Select Action to be executed after clicking \"%1\" button:")
		.arg(actionWidgetName), vbox);
	m_listview = new KListView(vbox, "actionSelectorDialogLV");
	m_listview->setResizeMode(QListView::LastColumn);
	m_listview->addColumn("");
	m_listview->header()->hide();
	resize(400, 500);
	connect( m_listview, SIGNAL(executed(QListViewItem*)), this, SLOT(slotListViewExecuted(QListViewItem*)));
//! @todo QLabel *lblDescription = new QLabel(vbox);
//! @todo: add "update Button text and tooltip" check box
//! @todo double click accepts dialog
	QListViewItem *pitem = 0;

	QPixmap noIcon( QSize( IconSize( KIcon::Small ), IconSize( KIcon::Small )) );
	QBitmap bmpNoIcon(noIcon.size());
	bmpNoIcon.fill(Qt::color0);
	noIcon.setMask(bmpNoIcon);

	//add "no action" item (key == "")
	pitem = new ActionSelectorDialogListItem( QCString(), m_listview, pitem, i18n("<No Action>") );
	pitem->setPixmap( 0, noIcon );

	foreach (KActionPtrList::ConstIterator, it, sharedActions) {
		//! @todo set invisible pixmap box if actual pixmap is null
		//! @todo group actions
		//! @todo: store KAction* here?
		pitem = new ActionSelectorDialogListItem( (*it)->name(), m_listview, pitem, (*it)->text().replace("&", "") );
		pitem->setPixmap( 0, (*it)->iconSet( KIcon::Small, 16 ).pixmap( QIconSet::Small, QIconSet::Active ) );
		if (!pitem->pixmap(0) || pitem->pixmap(0)->isNull())
			pitem->setPixmap( 0, noIcon );
		if (!m_listview->selectedItem() && currentActionName == (*it)->name())
			m_listview->setSelected(pitem, true);
	}
	if (m_listview->selectedItem()) {
		m_listview->ensureItemVisible(m_listview->selectedItem());
	}
	else if (m_listview->firstChild()) {
		m_listview->setSelected(m_listview->firstChild(), true);
		m_listview->ensureItemVisible(m_listview->selectedItem());
	}
}

KexiActionSelectionDialog::~KexiActionSelectionDialog()
{
}

QCString KexiActionSelectionDialog::selectedActionName() const
{
	QCString actionName;
	if (QDialog::Accepted == result() && m_listview->selectedItem()) {
		//update property value
		ActionSelectorDialogListItem *item = static_cast<ActionSelectorDialogListItem*>(m_listview->selectedItem());
		actionName = item->data;
		if (!actionName.isEmpty())
			actionName = QCString("kaction:") + item->data;
	}
	return actionName;
}

void KexiActionSelectionDialog::slotListViewExecuted(QListViewItem*)
{
	accept();
}

#include "kexiactionselectiondialog.moc"
