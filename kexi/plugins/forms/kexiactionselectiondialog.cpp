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
			if (!selectedItem() && dialog->currentActionName() == name)
				setSelected(pitem, true);
		}
		ensureItemVisible(selectedItem());
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

		QPixmap pm( SmallIcon("script") );
		KexiPart::Info* info = Kexi::partManager().infoForMimeType("kexi/script");
		KexiProject* project = dialog->mainWin()->project();
		if(info && project) {
			KexiPart::ItemDict* itemdict = project->items(info);
			if(itemdict) {
				ActionSelectorDialogListItem* item = 0;
				for (KexiPart::ItemDictIterator it( *itemdict ); it.current(); ++it) {
					QString name = QString("script:%1").arg(it.current()->name());
					item = new ActionSelectorDialogListItem(name, this, item, it.current()->caption());
					item->setPixmap(0, pm);
					if(!selectedItem() && dialog->currentActionName() == name)
						setSelected(item, true);
				}
				ensureItemVisible(selectedItem());
			}
		}
	}
	virtual ~ScriptsListView() {}
};

class KexiActionSelectionDialog::KexiActionSelectionDialogPrivate
{
public:
	KexiMainWindow* mainWin;
	KListView* kactionListView;
	KListView* scriptListView;
	QVBox *mainbox;
	QString currentActionName;
	KexiActionSelectionDialogPrivate() 
		: kactionListView(0), scriptListView(0) 
	{}
};

//-------------------------------------

//! actions IDs for the combo box
#define ACTION_TYPE_NO_ACTION_ID 0
#define ACTION_TYPE_KACTION_ID 1
#define ACTION_TYPE_SCRIPT_ID 2

//-------------------------------------

KexiActionSelectionDialog::KexiActionSelectionDialog(KexiMainWindow* mainWin, QWidget *parent, 
	const QString& _currentActionName, const QCString& actionWidgetName)

	: KDialogBase(parent, "actionSelectorDialog", true, i18n("Assigning Action to Command Button"), 
		KDialogBase::Ok | KDialogBase::Cancel )
	, d( new KexiActionSelectionDialogPrivate() )
{
	d->mainWin = mainWin;
	d->currentActionName = _currentActionName;

	QVBox* box = makeVBoxMainWidget();

	QLabel *lbl = new QLabel(i18n("Select Action to be executed after clicking \"%1\" button.")
		.arg(actionWidgetName), box);
	lbl->setAlignment(Qt::AlignTop|Qt::AlignLeft|Qt::WordBreak);

	QWidget *w = new QWidget(box);
	QHBoxLayout *lyr = new QHBoxLayout(w, 0, KDialogBase::spacingHint());
	QComboBox* combobox = new QComboBox(w);
	combobox->insertItem( i18n("No Action") );
	combobox->insertItem( i18n("Application") );
	lbl = new QLabel(combobox, i18n("Action type:").arg(actionWidgetName), w);
	lyr->addWidget(lbl);
	lyr->addWidget(combobox);
	lyr->addStretch(1);
	
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
	if (d->scriptListView)
		d->scriptListView->hide();
	if (!d->kactionListView) {
		d->kactionListView = new ActionsListView(d->mainbox, this);
	}
	d->kactionListView->show();
}

void KexiActionSelectionDialog::showScriptListView()
{
	if (d->kactionListView)
		d->kactionListView->hide();
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
			if (d->scriptListView)
				d->scriptListView->hide();
			break;
		case ACTION_TYPE_KACTION_ID:
			showKActionListView();
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
	QListViewItem *item = 
		(d->kactionListView && d->kactionListView->isVisible()) ? d->kactionListView->selectedItem() : 0;
	if (!item)
		item = (d->scriptListView && d->scriptListView->isVisible()) ? d->scriptListView->selectedItem() : 0;
	if (item) {
		d->currentActionName = dynamic_cast<ActionSelectorDialogListItem*>( item )->data;
	}
	else {
		d->currentActionName = QString::null; // "No Action"
	}
	KDialogBase::slotOk();
}

void KexiActionSelectionDialog::closeDialog()
{
	// not needed cause KexiFormPart which uses us takes care of freeing.
	//delayedDestruct();
}

#include "kexiactionselectiondialog.moc"
