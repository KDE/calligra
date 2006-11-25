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

#include "kexiactionselectiondialog.h"

#include <widget/utils/klistviewitemtemplate.h>
#include <keximainwindow.h>
#include <kexipartitem.h>
#include <kexiproject.h>

#include <k3listview.h>
#include <kaction.h>
#include <kiconloader.h>
#include <kdebug.h>

#include <qbitmap.h>
#include <qlabel.h>
#include <q3header.h>
#include <q3vbox.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3HBoxLayout>
#include <Q3CString>

typedef KListViewItemTemplate<QString> ActionSelectorDialogListItem;

class ActionsListView : public K3ListView
{
public:
	ActionsListView(QWidget* parent, KexiActionSelectionDialog* dialog)
		: K3ListView(parent, "actionslistview")
	{
		setResizeMode(Q3ListView::LastColumn);
		addColumn("");
		header()->hide();

		QPixmap noIcon( IconSize( K3Icon::Small ), IconSize( K3Icon::Small ) );
		QBitmap bmpNoIcon(noIcon.size());
		bmpNoIcon.fill(Qt::color0);
		noIcon.setMask(bmpNoIcon);

		Q3ListViewItem *pitem = 0;
		KActionPtrList sharedActions( dialog->mainWin()->allActions() ); //sharedActions() );
		foreach (KActionPtrList::ConstIterator, it, sharedActions) {
			//! @todo set invisible pixmap box if actual pixmap is null
			//! @todo group actions
			//! @todo: store KAction* here?
			QString name = QString("kaction:%1").arg((*it)->name());
			pitem = new ActionSelectorDialogListItem(name, this, pitem, (*it)->text().replace("&", "") );
			pitem->setPixmap( 0, (*it)->iconSet( K3Icon::Small, 16 ).pixmap( QIcon::Small, QIcon::Active ) );
			if (!pitem->pixmap(0) || pitem->pixmap(0)->isNull())
				pitem->setPixmap( 0, noIcon );
			if (!selectedItem() && dialog->currentActionName() == name)
				setSelected(pitem, true);
		}
		ensureItemVisible(selectedItem());
	}
	virtual ~ActionsListView() {}
};

class KexiPartItemsListView : public K3ListView
{
public:
	KexiPartItemsListView(QWidget* parent, KexiActionSelectionDialog* dialog, const QString& partname)
		: K3ListView(parent)
	{
		setResizeMode(Q3ListView::LastColumn);
		addColumn("");
		header()->hide();

		QPixmap pm( SmallIcon(partname) );
		KexiPart::Info* info = Kexi::partManager().infoForMimeType( QString("kexi/%1").arg(partname) );
		KexiProject* project = dialog->mainWin()->project();
		if(info && project) {
			KexiPart::ItemDict* itemdict = project->items(info);
			if(itemdict) {
				ActionSelectorDialogListItem* item = 0;
				for (KexiPart::ItemDictIterator it( *itemdict ); it.current(); ++it) {
					QString name = QString("%1:%2").arg(partname).arg(it.current()->name());
					item = new ActionSelectorDialogListItem(name, this, item, it.current()->caption());
					item->setPixmap(0, pm);
					if(!selectedItem() && dialog->currentActionName() == name)
						setSelected(item, true);
				}
				ensureItemVisible(selectedItem());
			}
		}
	}
	virtual ~KexiPartItemsListView() {}
};

class MacrosListView : public KexiPartItemsListView
{
public:
	MacrosListView(QWidget* parent, KexiActionSelectionDialog* dialog)
		: KexiPartItemsListView(parent, dialog, "macro") {}
	virtual ~MacrosListView() {}
};

class ScriptsListView : public KexiPartItemsListView
{
public:
	ScriptsListView(QWidget* parent, KexiActionSelectionDialog* dialog)
		: KexiPartItemsListView(parent, dialog, "script") {}
	virtual ~ScriptsListView() {}
};

class KexiActionSelectionDialog::KexiActionSelectionDialogPrivate
{
public:
	KexiMainWindow* mainWin;
	ActionsListView* kactionListView;
	MacrosListView* macroListView;
	ScriptsListView* scriptListView;
	Q3VBox *mainbox;
	QString currentActionName;
	KexiActionSelectionDialogPrivate() 
		: kactionListView(0), macroListView(0), scriptListView(0) 
	{}
};

//-------------------------------------

//! actions IDs for the combo box
#define ACTION_TYPE_NO_ACTION_ID 0
#define ACTION_TYPE_KACTION_ID 1
#define ACTION_TYPE_MACRO_ID 2
#define ACTION_TYPE_SCRIPT_ID 3

//-------------------------------------

KexiActionSelectionDialog::KexiActionSelectionDialog(KexiMainWindow* mainWin, QWidget *parent, 
	const QString& _currentActionName, const Q3CString& actionWidgetName)

	: KDialogBase(parent, "actionSelectorDialog", true, i18n("Assigning Action to Command Button"), 
		KDialogBase::Ok | KDialogBase::Cancel )
	, d( new KexiActionSelectionDialogPrivate() )
{
	d->mainWin = mainWin;
	d->currentActionName = _currentActionName;

	KVBox* box = makeVBoxMainWidget();

	QLabel *lbl = new QLabel(i18n("Select Action to be executed after clicking \"%1\" button.")
		.arg(actionWidgetName), box);
	lbl->setAlignment(Qt::AlignTop|Qt::AlignLeft|Qt::TextWordWrap);

	QWidget *w = new QWidget(box);
	Q3HBoxLayout *lyr = new Q3HBoxLayout(w, 0, KDialogBase::spacingHint());
	QComboBox* combobox = new QComboBox(w);
	combobox->insertItem( i18n("No Action") );
	combobox->insertItem( i18n("Application") );
	lbl = new QLabel(combobox, i18n("Action type:").arg(actionWidgetName), w);
	lyr->addWidget(lbl);
	lyr->addWidget(combobox);
	lyr->addStretch(1);

	d->mainbox = new Q3VBox(box);
	box->setStretchFactor(d->mainbox, 1);
	resize(400, 500);

	// We use the PartManager to determinate if the Kexi-plugin is
	// installed and if we like to show it in our list of actions.
	KexiPart::Info* macroinfo = Kexi::partManager().infoForMimeType("kexi/macro");
	if(macroinfo) {
		combobox->insertItem( i18n("Macros") );
	}

	KexiPart::Info* scriptinfo = Kexi::partManager().infoForMimeType("kexi/script");
	if(scriptinfo) {
		combobox->insertItem( i18n("Scripts") );
	}
	
	if (d->currentActionName.startsWith("macro:")) {
		if(macroinfo) {
			combobox->setCurrentItem(ACTION_TYPE_MACRO_ID);
			slotActionTypeSelected(ACTION_TYPE_MACRO_ID);
		}
		
	}
	else if (d->currentActionName.startsWith("script:")) {
		if(scriptinfo) {
			combobox->setCurrentItem(ACTION_TYPE_SCRIPT_ID);
			slotActionTypeSelected(ACTION_TYPE_SCRIPT_ID);
		}
		
	}
	else { // default == kaction //if (d->currentActionName.startsWith("kaction:")) {
		combobox->setCurrentItem(ACTION_TYPE_KACTION_ID);
		slotActionTypeSelected(ACTION_TYPE_KACTION_ID);
	}
	connect(combobox, SIGNAL(activated(int)), this, SLOT(slotActionTypeSelected(int)));
	connect(this, SIGNAL(finished()), SLOT(closeDialog()));
}

KexiActionSelectionDialog::~KexiActionSelectionDialog()
{
	delete d;
}

void KexiActionSelectionDialog::showKActionListView()
{
	if (d->macroListView)
		d->macroListView->hide();
	if (d->scriptListView)
		d->scriptListView->hide();
	if (!d->kactionListView) {
		d->kactionListView = new ActionsListView(d->mainbox, this);
	}
	d->kactionListView->show();
}

void KexiActionSelectionDialog::showMacroListView()
{
	if (d->kactionListView)
		d->kactionListView->hide();
	if (d->scriptListView)
		d->scriptListView->hide();
	if (!d->macroListView) {
		d->macroListView = new MacrosListView(d->mainbox, this);
	}
	d->macroListView->show();
}

void KexiActionSelectionDialog::showScriptListView()
{
	if (d->kactionListView)
		d->kactionListView->hide();
	if (d->macroListView)
		d->macroListView->hide();
	if (!d->scriptListView) {
		d->scriptListView = new ScriptsListView(d->mainbox, this);
	}
	d->scriptListView->show();
}

void KexiActionSelectionDialog::slotActionTypeSelected(int index)
{
	switch(index) {
		case ACTION_TYPE_NO_ACTION_ID:
			if (d->kactionListView)
				d->kactionListView->hide();
			if (d->macroListView)
				d->macroListView->hide();
			if (d->scriptListView)
				d->scriptListView->hide();
			break;
		case ACTION_TYPE_KACTION_ID:
			showKActionListView();
			break;
		case ACTION_TYPE_MACRO_ID:
			showMacroListView();
			break;
		case ACTION_TYPE_SCRIPT_ID:
			showScriptListView();
			break;
		default:;
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
	Q3ListViewItem *item = 
		(d->kactionListView && d->kactionListView->isVisible()) ? d->kactionListView->selectedItem() : 0;
	if (!item)
		item = (d->macroListView && d->macroListView->isVisible()) ? d->macroListView->selectedItem() : 0;
	if (!item)
		item = (d->scriptListView && d->scriptListView->isVisible()) ? d->scriptListView->selectedItem() : 0;
	if (item) {
		d->currentActionName = dynamic_cast<ActionSelectorDialogListItem*>( item )->data;
	}
	else {
		d->currentActionName.clear(); // "No Action"
	}
	KDialogBase::slotOk();
}

void KexiActionSelectionDialog::closeDialog()
{
	// not needed cause KexiFormPart which uses us takes care of freeing.
	//delayedDestruct();
}

#include "kexiactionselectiondialog.moc"
