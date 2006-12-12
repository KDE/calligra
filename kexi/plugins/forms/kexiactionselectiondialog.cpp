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

#include <keximainwindow.h>
#include <kexipartitem.h>
#include <kexiproject.h>
#include <kexipartinfo.h>
#include <kexipart.h>

#include <klistview.h>
#include <kaction.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kstdguiitem.h>
#include <kpushbutton.h>

#include <qbitmap.h>
#include <qlabel.h>
#include <qheader.h>
#include <qvbox.h>
#include <qtooltip.h>
#include <qwidgetstack.h>

#include <widget/utils/klistviewitemtemplate.h>
#include <widget/kexibrowser.h>
#include <widget/kexibrowseritem.h>
#include <kexiutils/utils.h>

typedef KListViewItemTemplate<QString> ActionSelectorDialogListItemBase;

class ActionSelectorDialogListItem : public ActionSelectorDialogListItemBase
{
public:
	ActionSelectorDialogListItem(const QString& data, QListView *parent, QString label1)
	 : ActionSelectorDialogListItemBase(data, parent, label1)
	 , fifoSorting(true)
	{
		m_sortKey.sprintf("%2.2d", parent->childCount());
	}

	ActionSelectorDialogListItem(const QString& data, QListViewItem *parent, QString label1)
	 : ActionSelectorDialogListItemBase(data, parent, label1)
	 , fifoSorting(true)
	{
		m_sortKey.sprintf("%2.2d", parent->childCount());
	}

	virtual QString key( int column, bool ascending ) const
	{
		return fifoSorting ? m_sortKey : ActionSelectorDialogListItemBase::key(column, ascending);
	}

	bool fifoSorting : 1;

protected:
	QString m_sortKey;
};

class ActionsListViewBase : public KListView
{
public:
	ActionsListViewBase(QWidget* parent)
	 : KListView(parent)
	{
		setResizeMode(QListView::AllColumns);
		addColumn("");
		header()->hide();
		setColumnWidthMode(0, QListView::Maximum);
		setAllColumnsShowFocus(true);
		setTooltipColumn(0);
	}

	~ActionsListViewBase()
	{
	}

	//! \return item for action \a actionName
	virtual QListViewItem *itemForAction(const QString& actionName)
	{
		for (QListViewItemIterator it(this); it.current(); ++it) {
			ActionSelectorDialogListItem* item = dynamic_cast<ActionSelectorDialogListItem*>(it.current());
			if (item && item->data == actionName)
				return item;
		}
		return 0;
	}

	void selectAction(const QString& actionName)
	{
		QListViewItem *item = itemForAction(actionName);
		if (item) {
			setSelected(item, true);
			ensureItemVisible(firstChild());
			ensureItemVisible(selectedItem());
		}
	}
};

//! @internal Used to display KActions (in column 2)
class ActionsListView : public ActionsListViewBase
{
public:
	ActionsListView(QWidget* parent, KexiMainWindow* mainWin )
		: ActionsListViewBase(parent)
	{
		setSorting(0);
		const QPixmap noIcon( KexiUtils::emptyIcon(KIcon::Small) );
		KActionPtrList sharedActions( mainWin->allActions() ); //sharedActions() );
		foreach (KActionPtrList::ConstIterator, it, sharedActions) {
			//! @todo group actions
			//! @todo: store KAction* here?
			ActionSelectorDialogListItem *pitem = new ActionSelectorDialogListItem((*it)->name(), this, (*it)->text().replace("&", "") );
			pitem->fifoSorting = false; //alpha sort
			pitem->setPixmap( 0, (*it)->iconSet( KIcon::Small, 16 ).pixmap( QIconSet::Small, QIconSet::Active ) );
			if (!pitem->pixmap(0) || pitem->pixmap(0)->isNull())
				pitem->setPixmap( 0, noIcon );
		}
	}

	virtual ~ActionsListView() {}
};

//! @internal a list view displaying action categories user can select from (column 1)
class ActionCategoriesListView : public ActionsListViewBase
{
public:
	ActionCategoriesListView(QWidget* parent) //, KexiProject& project)
	 : ActionsListViewBase(parent)
	{
		QListViewItem *item = new ActionSelectorDialogListItem("noaction", this, i18n("No action") );
		const QPixmap noIcon( KexiUtils::emptyIcon(KIcon::Small) );
		item->setPixmap(0, noIcon);
		item = new ActionSelectorDialogListItem("kaction", this, i18n("Application actions") );
		item->setPixmap(0, SmallIcon("form_action"));

		KexiPart::PartInfoList *pl = Kexi::partManager().partInfoList();
		for (KexiPart::Info *info = pl->first(); info; info = pl->next()) {
			KexiPart::Part *part = Kexi::partManager().part(info);
			if (!info->isVisibleInNavigator() || !part)
				continue;
			item = new KexiBrowserItem(this, info);
			item->setText(0, part->instanceCaption());
		}
		adjustColumn(0);
		setMinimumWidth( columnWidth(0) + 6 );
	}

	~ActionCategoriesListView()
	{
	}

	//! \return item for action \a actionName, reimplemented to support KexiBrowserItem items
	virtual QListViewItem *itemForAction(const QString& actionName)
	{
		for (QListViewItemIterator it(this); it.current(); ++it) {
			//simple case
			ActionSelectorDialogListItem* item = dynamic_cast<ActionSelectorDialogListItem*>(it.current());
			if (item) {
				if (item->data == actionName)
					return it.current();
				continue;
			}
			KexiBrowserItem* bitem = dynamic_cast<KexiBrowserItem*>(it.current());
			if (bitem) {
				if (bitem->info()->objectName() == actionName)
					return it.current();
			}
		}
		return 0;
	}
};

//! @internal Used to display list of actions available to executing (column 3)
class ActionToExecuteListView : public ActionsListViewBase
{
	public:
		ActionToExecuteListView(QWidget* parent)
		 : ActionsListViewBase(parent)
		{
		}

		~ActionToExecuteListView()
		{
		}

		//! Updates actions
		void showActionsForMimeType(const QString& mimeType) {
			if (m_currentMimeType == mimeType)
				return;
			m_currentMimeType = mimeType;
			clear();
			KexiPart::Part *part = Kexi::partManager().partForMimeType( m_currentMimeType );
			if (!part)
				return;
			int supportedViewModes = part->supportedViewModes();
			ActionSelectorDialogListItem *item;
			const QPixmap noIcon( KexiUtils::emptyIcon(KIcon::Small) );
			if (supportedViewModes & Kexi::DataViewMode) {
				item = new ActionSelectorDialogListItem("open", this, i18n("Open in Data View"));
				item->setPixmap(0, SmallIcon("fileopen"));
			}
			if (part->info()->isExecuteSupported()) {
				item = new ActionSelectorDialogListItem("execute", this, i18n("Execute"));
				item->setPixmap(0, SmallIcon("player_play"));
			}
			if (part->info()->isPrintingSupported()) {
				ActionSelectorDialogListItem *printItem = new ActionSelectorDialogListItem(
					"print", this, i18n("Print"));
				printItem->setPixmap(0, SmallIcon("fileprint"));
				KAction *a = KStdAction::printPreview(0, 0, 0);
				item = new ActionSelectorDialogListItem("printPreview", printItem, 
					a->text().replace("&", ""));
				item->setPixmap(0, SmallIcon(a->icon()));
				delete a;
				item = new ActionSelectorDialogListItem("pageSetup", printItem, i18n("Show Page Setup"));
				item->setPixmap(0, noIcon);
				setOpen(printItem, true);
				printItem->setExpandable(false);
			}
			if (part->info()->isDataExportSupported()) {
				ActionSelectorDialogListItem *exportItem = new ActionSelectorDialogListItem(
					"exportToCSV", this, i18n("Note: use multiple rows if needed", "Export to File\nAs Data Table"));
				exportItem->setMultiLinesEnabled(true);
				exportItem->setPixmap(0, SmallIcon("table"));
				item = new ActionSelectorDialogListItem("copyToClipboardAsCSV", 
					exportItem, i18n("Note: use multiple rows if needed", "Copy to Clipboard\nAs Data Table"));
				item->setPixmap(0, SmallIcon("table"));
				item->setMultiLinesEnabled(true);
				setOpen(exportItem, true);
				exportItem->setExpandable(false);
			}
			if (supportedViewModes & Kexi::DesignViewMode) {
				item = new ActionSelectorDialogListItem("design", this, i18n("Open in Design View"));
				item->setPixmap(0, SmallIcon("edit"));
			}
			if (supportedViewModes & Kexi::TextViewMode) {
				item = new ActionSelectorDialogListItem("editText", this, i18n("Open in Text View"));
				item->setPixmap(0, noIcon);
			}
			updateWidth();
		}

		void updateWidth()
		{
			adjustColumn(0);
			setMinimumWidth( columnWidth(0) );
		}

		QString m_currentMimeType;
};

//-------------------------------------

//! @internal
class KexiActionSelectionDialog::KexiActionSelectionDialogPrivate
{
public:
	KexiActionSelectionDialogPrivate() 
		: kactionListView(0), objectsListView(0)
	{
	}

	void raiseWidget(QWidget *w)
	{
		stack->raiseWidget( w );
		selectActionToBeExecutedLbl->setBuddy(w);
	}

	void updateSelectActionToBeExecutedMessage(const QString& actionType)
	{
		QString msg;
		if (actionType=="noaction")
			msg = QString::null;
		else if (actionType=="kaction")
			msg = i18n("&Select action to be executed after clicking \"%1\" button:").arg(actionWidgetName);
		// hardcoded, but it's not that bad
		else if (actionType=="macro")
			msg = i18n("&Select macro to be executed after clicking \"%1\" button:").arg(actionWidgetName);
		else if (actionType=="script")
			msg = i18n("&Select script to be executed after clicking \"%1\" button:").arg(actionWidgetName);
		//default: table/query/form/report...
		else
			msg = i18n("&Select object to be opened after clicking \"%1\" button:").arg(actionWidgetName);
		selectActionToBeExecutedLbl->setText(msg);
	}

	void setActionToExecuteSectionVisible(bool visible)
	{
		if (visible) {
			actionToExecuteListView->show();
			actionToExecuteLbl->show();
		}
		else {
			actionToExecuteListView->hide();
			actionToExecuteLbl->hide();
		}
	}

	KexiMainWindow* mainWin;
	QString actionWidgetName;
	ActionCategoriesListView* actionCategoriesListView; //!< for column #1
	ActionsListView* kactionListView;  //!< for column #2
	KexiBrowser* objectsListView; //!< for column #2
	QWidget *emptyWidget;
	QLabel* selectActionToBeExecutedLbl;
	ActionToExecuteListView* actionToExecuteListView;
	QLabel *actionToExecuteLbl;
	QWidgetStack *stack;
};

//-------------------------------------

KexiActionSelectionDialog::KexiActionSelectionDialog(KexiMainWindow* mainWin, QWidget *parent, 
	const KexiFormEventAction::ActionData& action, const QCString& actionWidgetName)
	: KDialogBase(parent, "actionSelectorDialog", true, i18n("Assigning Action to Command Button"), 
		KDialogBase::Ok | KDialogBase::Cancel )
	, d( new KexiActionSelectionDialogPrivate() )
{
	d->mainWin = mainWin;
	d->actionWidgetName = actionWidgetName;
	setButtonOK( KGuiItem(i18n("Assign action", "&Assign"), "button_ok", i18n("Assign action")) );

	QWidget *main = new QWidget( this );
	main->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setMainWidget(main);

	QGridLayout *glyr = new QGridLayout(main, 2, 3, KDialog::marginHint(), KDialog::spacingHint());
	glyr->setRowStretch(1, 1);

	// 1st column: action types
	d->actionCategoriesListView = new ActionCategoriesListView(main);
	d->actionCategoriesListView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	glyr->addWidget(d->actionCategoriesListView, 1, 0);
	connect( d->actionCategoriesListView, SIGNAL(selectionChanged(QListViewItem*)), 
		this, SLOT(slotActionCategorySelected(QListViewItem*)));

	QLabel *lbl = new QLabel(d->actionCategoriesListView, i18n("Action category:").arg(actionWidgetName), main);
	lbl->setMinimumHeight(lbl->fontMetrics().height()*2);
	lbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	lbl->setAlignment(Qt::AlignTop|Qt::AlignLeft|Qt::WordBreak);
	glyr->addWidget(lbl, 0, 0, Qt::AlignTop|Qt::AlignLeft);

	// 2nd column: list of actions/objects
	d->stack = new QWidgetStack(main);
	d->stack->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	glyr->addWidget(d->stack, 1, 1);

	d->selectActionToBeExecutedLbl = new QLabel(main);
	d->selectActionToBeExecutedLbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	d->selectActionToBeExecutedLbl->setAlignment(Qt::AlignTop|Qt::AlignLeft|Qt::WordBreak);
	lbl->setMinimumHeight(lbl->minimumHeight());
	glyr->addWidget(d->selectActionToBeExecutedLbl, 0, 1, Qt::AlignTop|Qt::AlignLeft);

	d->emptyWidget = new QWidget(d->stack);
	d->stack->addWidget(d->emptyWidget);

	// 3rd column: actions to execute
	d->actionToExecuteListView = new ActionToExecuteListView(main);
	d->actionToExecuteListView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	connect(d->actionToExecuteListView, SIGNAL(executed(QListViewItem*)),
		this, SLOT(slotActionToExecuteItemExecuted(QListViewItem*)));
	connect(d->actionToExecuteListView, SIGNAL(selectionChanged(QListViewItem*)),
		this, SLOT(slotActionToExecuteItemSelected(QListViewItem*)));
	glyr->addWidget(d->actionToExecuteListView, 1, 2);

	d->actionToExecuteLbl = new QLabel(d->actionToExecuteListView, i18n("Action to execute:").arg(actionWidgetName), main);
	d->actionToExecuteLbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	d->actionToExecuteLbl->setAlignment(Qt::AlignTop|Qt::AlignLeft|Qt::WordBreak);
	glyr->addWidget(d->actionToExecuteLbl, 0, 2, Qt::AlignTop|Qt::AlignLeft);

	// temporary show all sections to avoid resizing the dialog in the future
	d->actionCategoriesListView->selectAction("table");
	d->setActionToExecuteSectionVisible(true);
	adjustSize();
	d->actionToExecuteListView->updateWidth();

	bool ok;
	QString actionType, actionArg;
	KexiPart::Info* partInfo = action.decodeString(actionType, actionArg, ok);
	if (ok) {
		d->actionCategoriesListView->selectAction(actionType);
		if (actionType=="kaction") {
			d->kactionListView->selectAction(actionArg);
			d->kactionListView->setFocus();
		}
		else if (partInfo
			&& Kexi::partManager().part(partInfo)) // We use the Part Manager
			// to determine whether the Kexi-plugin is installed and whether we like to show 
			// it in our list of actions.
		{
			KexiPart::Item *item = d->mainWin->project()->item(partInfo, actionArg);
			if (d->objectsListView && item) {
				d->objectsListView->selectItem(*item);
				QString actionOption( action.option );
				if (actionOption.isEmpty())
					actionOption = "open"; // for backward compatibility
				d->actionToExecuteListView->selectAction(actionOption);
				d->objectsListView->setFocus();
			}
		}
	}
	else {//invalid assignment or 'noaction'
		d->actionCategoriesListView->selectAction("noaction");
		d->actionCategoriesListView->setFocus();
	}
}

KexiActionSelectionDialog::~KexiActionSelectionDialog()
{
	delete d;
}

void KexiActionSelectionDialog::slotKActionItemExecuted(QListViewItem*)
{
	accept();
}

void KexiActionSelectionDialog::slotKActionItemSelected(QListViewItem*)
{
	d->setActionToExecuteSectionVisible(false);
	updateOKButtonStatus();
}

void KexiActionSelectionDialog::slotItemForOpeningOrExecutingSelected(KexiPart::Item* item)
{
	d->setActionToExecuteSectionVisible(item);
}

void KexiActionSelectionDialog::slotActionToExecuteItemExecuted(QListViewItem* item)
{
	if (!item)
		return;
	ActionSelectorDialogListItemBase *listItem = dynamic_cast<ActionSelectorDialogListItemBase*>(item);
	if (listItem && !listItem->data.isEmpty())
		accept();
}

void KexiActionSelectionDialog::slotActionToExecuteItemSelected(QListViewItem*)
{
	updateOKButtonStatus();
}

void KexiActionSelectionDialog::slotActionCategorySelected(QListViewItem* item)
{
	ActionSelectorDialogListItem *simpleItem = dynamic_cast<ActionSelectorDialogListItem*>(item);
	// simple case: part-less item, e.g. kaction:
	if (simpleItem) {
		d->updateSelectActionToBeExecutedMessage(simpleItem->data);
		if (simpleItem->data == "kaction") {
			if (!d->kactionListView) {
				d->kactionListView = new ActionsListView(d->stack, d->mainWin);
				d->stack->addWidget(d->kactionListView);
				connect(d->kactionListView, SIGNAL(executed(QListViewItem*)),
					this, SLOT(slotKActionItemExecuted(QListViewItem*)));
				connect( d->kactionListView, SIGNAL(selectionChanged(QListViewItem*)), 
					this, SLOT(slotKActionItemSelected(QListViewItem*)));
			}
			d->raiseWidget(d->kactionListView);
			slotKActionItemSelected(d->kactionListView->selectedItem()); //to refresh column #3
			d->setActionToExecuteSectionVisible(false);
		}
		else if (simpleItem->data == "noaction") {
			d->raiseWidget(d->emptyWidget);
			//refresh column #3
			d->setActionToExecuteSectionVisible(false);
		}
		d->actionCategoriesListView->update();
		updateOKButtonStatus();
		return;
	}

	KexiBrowserItem* browserItem = dynamic_cast<KexiBrowserItem*>(item);
	if (browserItem) {
		d->updateSelectActionToBeExecutedMessage(browserItem->info()->objectName());
		if (!d->objectsListView) {
			d->objectsListView = new KexiBrowser(d->stack, d->mainWin, 0/*features*/);
			d->stack->addWidget(d->objectsListView);
			connect(d->objectsListView, SIGNAL(selectionChanged(KexiPart::Item*)),
				this, SLOT(slotItemForOpeningOrExecutingSelected(KexiPart::Item*)));
		}
		if (d->objectsListView->itemsMimeType().latin1()!=browserItem->info()->mimeType()) {
			d->objectsListView->setProject(d->mainWin->project(), browserItem->info()->mimeType());
			d->actionToExecuteListView->showActionsForMimeType( browserItem->info()->mimeType() );
			d->setActionToExecuteSectionVisible(false);
		}
		d->raiseWidget( d->objectsListView );
	}
	d->actionCategoriesListView->update();
	updateOKButtonStatus();
}

KexiMainWindow* KexiActionSelectionDialog::mainWin() const
{
	return d->mainWin;
}

KexiFormEventAction::ActionData KexiActionSelectionDialog::currentAction() const
{
	KexiFormEventAction::ActionData data;
	ActionSelectorDialogListItem *simpleItem = dynamic_cast<ActionSelectorDialogListItem*>(
		d->actionCategoriesListView->selectedItem());
	// simple case: part-less item, e.g. kaction:
	if (simpleItem) {
		if (simpleItem->data == "kaction") {
			if (d->kactionListView->selectedItem()) {
				data.string = QString("kaction:")
					+ dynamic_cast<ActionSelectorDialogListItem*>( d->kactionListView->selectedItem() )->data;
				return data;
			}
		}
	}
	KexiBrowserItem* browserItem = dynamic_cast<KexiBrowserItem*>( d->actionCategoriesListView->selectedItem() );
	if (browserItem) {
		ActionSelectorDialogListItem *actionToExecute = dynamic_cast<ActionSelectorDialogListItem*>(
			d->actionToExecuteListView->selectedItem());
		if (d->objectsListView && actionToExecute && !actionToExecute->data.isEmpty()) {
			KexiPart::Item* partItem = d->objectsListView->selectedPartItem();
			KexiPart::Info* partInfo = partItem ? Kexi::partManager().infoForMimeType( partItem->mimeType() ) : 0;
			if (partInfo) {
				// opening or executing: table:name, query:name, form:name, macro:name, script:name, etc.
				data.string = QString("%1:%2").arg(partInfo->objectName()).arg(partItem->name());
				data.option = actionToExecute->data;
				return data;
			}
		}
	}
	return data; // No Action
}

void KexiActionSelectionDialog::updateOKButtonStatus()
{
	QPushButton *btn = actionButton(Ok);
	ActionSelectorDialogListItem *simpleItem = dynamic_cast<ActionSelectorDialogListItem*>(
		d->actionCategoriesListView->selectedItem());
	btn->setEnabled( (simpleItem && simpleItem->data == "noaction") || !currentAction().isEmpty() );
}

#include "kexiactionselectiondialog.moc"
