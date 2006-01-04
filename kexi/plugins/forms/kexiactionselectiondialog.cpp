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

#include "kexiactionselectiondialog.h"

#include <widget/utils/klistviewitemtemplate.h>
#include <keximainwindow.h>
#include <kexipartitem.h>
#include <kexiproject.h>

#include <klistview.h>
#include <kaction.h>
#include <kiconloader.h>
#include <kdebug.h>

#include <qbitmap.h>
#include <qlabel.h>
#include <qheader.h>
#include <qvbox.h>

typedef KListViewItemTemplate<QString> ActionSelectorDialogListItem;

class ActionsListView : public KListView
{
public:
	ActionsListView(QWidget* parent, KexiActionSelectionDialog* dialog)
		: KListView(parent, "actionslistview")
	{
		setResizeMode(QListView::LastColumn);
		addColumn("");
		header()->hide();

		QPixmap noIcon( IconSize( KIcon::Small ), IconSize( KIcon::Small ) );
		QBitmap bmpNoIcon(noIcon.size());
		bmpNoIcon.fill(Qt::color0);
		noIcon.setMask(bmpNoIcon);

		QListViewItem *pitem = 0;
		KActionPtrList sharedActions( dialog->mainWin()->allActions() ); //sharedActions() );
		foreach (KActionPtrList::ConstIterator, it, sharedActions) {
			//! @todo set invisible pixmap box if actual pixmap is null
			//! @todo group actions
			//! @todo: store KAction* here?
			QString name = QString("kaction:%1").arg((*it)->name());
			pitem = new ActionSelectorDialogListItem(name, this, pitem, (*it)->text().replace("&", "") );
			pitem->setPixmap( 0, (*it)->iconSet( KIcon::Small, 16 ).pixmap( QIconSet::Small, QIconSet::Active ) );
			if (!pitem->pixmap(0) || pitem->pixmap(0)->isNull())
				pitem->setPixmap( 0, noIcon );
			if (!selectedItem() && dialog->currentActionName() == name) {
				setSelected(pitem, true);
				ensureItemVisible(pitem);
			}
		}
	}
	virtual ~ActionsListView() {}
};

class ScriptsListView : public KListView
{
public:
	ScriptsListView(QWidget* parent, KexiActionSelectionDialog* dialog)
		: KListView(parent, "scriptslistview")
	{
		setResizeMode(QListView::LastColumn);
		addColumn("");
		header()->hide();

		KexiPart::Info* info = Kexi::partManager().infoForMimeType("kexi/script");
		KexiProject* project = dialog->mainWin()->project();
		if(info && project) {
			KexiPart::ItemDict* itemdict = project->items(info);
			if(itemdict) {
				ActionSelectorDialogListItem* item = 0;
				for (KexiPart::ItemDictIterator it( *itemdict ); it.current(); ++it) {
					QString name = QString("script:%1").arg(it.current()->name());
					item = new ActionSelectorDialogListItem(name, this, item, it.current()->caption());
					if(!selectedItem() && dialog->currentActionName() == name)
						setSelected(item, true);
				}
			}
		}
	}
	virtual ~ScriptsListView() {}
};

class KexiActionSelectionDialog::KexiActionSelectionDialogPrivate
{
public:
	KexiMainWindow* mainWin;
	KListView* listview;
	QVBox *mainbox;
	QString currentActionName;
	KexiActionSelectionDialogPrivate() : listview(0) {}
};

KexiActionSelectionDialog::KexiActionSelectionDialog(KexiMainWindow* mainWin, QWidget *parent, 
	const QString& _currentActionName, const QCString& actionWidgetName)

	: KDialogBase(parent, "actionSelectorDialog", true, QString::null, KDialogBase::Ok | KDialogBase::Cancel )
	, d( new KexiActionSelectionDialogPrivate() )
{
	d->mainWin = mainWin;
	d->currentActionName = _currentActionName;

	QVBox* box = makeVBoxMainWidget();

	QLabel *lbl = new QLabel(i18n("Select Action to be executed after clicking \"%1\" button:").arg(actionWidgetName), box);
	lbl->setAlignment(Qt::AlignTop|Qt::AlignLeft|Qt::WordBreak);

	QComboBox* combobox = new QComboBox(box);
	combobox->insertItem( i18n("No Action") );
	combobox->insertItem( i18n("Actions") );
	
	// We use the scriptpart to determinate if the Kexi ScriptPart-plugin is
	// installed and if we like to show it in our list of actions.
	KexiPart::Info* scriptinfo = Kexi::partManager().infoForMimeType("kexi/script");
	if(scriptinfo) {
		combobox->insertItem( i18n("Scripts") );
	}
	
	d->mainbox = new QVBox(box);
	box->setStretchFactor(d->mainbox, 1);

	resize(400, 500);

	if (d->currentActionName.startsWith("script:")) {
		if(scriptinfo) {
			combobox->setCurrentItem(2);
			slotComboHighlighted(2);
		}
		
	}
	else if (d->currentActionName.startsWith("kaction:")) {
		combobox->setCurrentItem(1);
		slotComboHighlighted(1);
	}
	//else "No Action"

	connect(combobox, SIGNAL(highlighted(int)), this, SLOT(slotComboHighlighted(int)));
	connect(this, SIGNAL(finished()), SLOT(closeDialog()));
}

KexiActionSelectionDialog::~KexiActionSelectionDialog()
{
	delete d;
}

void KexiActionSelectionDialog::setActionView()
{
	delete d->listview;
	d->listview = new ActionsListView(d->mainbox, this);
	d->listview->show();
}

void KexiActionSelectionDialog::setScriptView()
{
	delete d->listview;
	d->listview = new ScriptsListView(d->mainbox, this);
	d->listview->show();
}

void KexiActionSelectionDialog::slotComboHighlighted(int index)
{
	switch(index) {
		case 1: setActionView(); break;
		case 2: setScriptView(); break;
		default: // "No Action"
			delete d->listview;
			d->listview = 0;
			break;
	}
}

KexiMainWindow* KexiActionSelectionDialog::mainWin()
{
	return d->mainWin;
}

QString KexiActionSelectionDialog::currentActionName() const
{
	return d->currentActionName;
}

void KexiActionSelectionDialog::slotOk()
{
	if(d->listview && d->listview->selectedItem()) {
		ActionSelectorDialogListItem *item = static_cast<ActionSelectorDialogListItem*>( d->listview->selectedItem() );
		if(item)
			d->currentActionName = item->data;
	}
	else {
		d->currentActionName = ""; // "No Action"
	}
	KDialogBase::slotOk();
}

void KexiActionSelectionDialog::closeDialog()
{
	// not needed cause KexiFormPart which uses us takes care of freeing.
	//delayedDestruct();
}

#include "kexiactionselectiondialog.moc"
