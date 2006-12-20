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
#include "kexiactionselectiondialog_p.h"

#include <keximainwindow.h>
#include <kexipartitem.h>
#include <kexiproject.h>
#include <kexipartinfo.h>
#include <kexipart.h>
#include <kexiactioncategories.h>

#include <klistview.h>
#include <kaction.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kstdguiitem.h>
#include <kpushbutton.h>

#include <qbitmap.h>
#include <qlabel.h>
#include <q3header.h>
#include <q3vbox.h>
#include <qtooltip.h>
#include <q3widgetstack.h>
//Added by qt3to4:
#include <Q3CString>
#include <Q3GridLayout>
#include <QPixmap>

#include <widget/utils/klistviewitemtemplate.h>
#include <widget/kexibrowser.h>
#include <widget/kexibrowseritem.h>
#include <kexiutils/utils.h>

typedef KListViewItemTemplate<QString> ActionSelectorDialogListItemBase;

class ActionSelectorDialogListItem : public ActionSelectorDialogListItemBase
{
public:
	ActionSelectorDialogListItem(const QString& data, Q3ListView *parent, QString label1)
	 : ActionSelectorDialogListItemBase(data, parent, label1)
	 , fifoSorting(true)
	{
		m_sortKey.sprintf("%2.2d", parent->childCount());
	}

	ActionSelectorDialogListItem(const QString& data, Q3ListViewItem *parent, QString label1)
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

//---------------------------------------

ActionsListViewBase::ActionsListViewBase(QWidget* parent)
 : K3ListView(parent)
{
	setResizeMode(Q3ListView::AllColumns);
	addColumn("");
	header()->hide();
	setColumnWidthMode(0, Q3ListView::Maximum);
	setAllColumnsShowFocus(true);
	setTooltipColumn(0);
}

ActionsListViewBase::~ActionsListViewBase()
{
}

Q3ListViewItem *ActionsListViewBase::itemForAction(const QString& actionName)
{
	for (Q3ListViewItemIterator it(this); it.current(); ++it) {
		ActionSelectorDialogListItem* item = dynamic_cast<ActionSelectorDialogListItem*>(it.current());
		if (item && item->data == actionName)
			return item;
}
	return 0;
}

void ActionsListViewBase::selectAction(const QString& actionName)
{
	Q3ListViewItem *item = itemForAction(actionName);
	if (item) {
		setSelected(item, true);
		ensureItemVisible(firstChild());
		ensureItemVisible(selectedItem());
}
}

//---------------------------------------

KActionsListViewBase::KActionsListViewBase(QWidget* parent, KexiMainWindow* mainWin)
	: ActionsListViewBase(parent)
	, m_mainWin(mainWin)
{
}

KActionsListViewBase::~KActionsListViewBase() {}

void KActionsListViewBase::init()
{
	setSorting(0);
	const QPixmap noIcon( KexiUtils::emptyIcon(KIcon::Small) );
	KActionPtrList sharedActions( m_mainWin->allActions() );
	const Kexi::ActionCategories *acat = Kexi::actionCategories();
	foreach (KActionPtrList::ConstIterator, it, sharedActions) {
//			kDebug() << (*it)->name() << " " << (*it)->text() << endl;
		//! @todo group actions
		//! @todo: store KAction* here?
		const int actionCategories = acat->actionCategories((*it)->name());
		if (actionCategories==-1) {
			kexipluginswarn << "KActionsListViewBase(): no category declared for action \"" 
				<< (*it)->name() << "\"! Fix this!" << endl;
			continue;
	}
		if (!isActionVisible((*it)->name(), actionCategories))
			continue;
		ActionSelectorDialogListItem *pitem = new ActionSelectorDialogListItem((*it)->name(), 
			this, (*it)->toolTip().isEmpty() ? (*it)->text().replace("&", "") : (*it)->toolTip() );
		pitem->fifoSorting = false; //alpha sort
		pitem->setPixmap( 0, (*it)->iconSet( KIcon::Small, 16 ).pixmap( QIcon::Small, QIcon::Active ) );
		if (!pitem->pixmap(0) || pitem->pixmap(0)->isNull())
			pitem->setPixmap( 0, noIcon );
}
}

//---------------------------------------

//! @internal Used to display KActions (in column 2)
class KActionsListView : public KActionsListViewBase
{
public:
	KActionsListView(QWidget* parent, KexiMainWindow* mainWin)
		: KActionsListViewBase(parent, mainWin)
	{
	}
	virtual ~KActionsListView() {}

	virtual bool isActionVisible(const char* actionName, int actionCategories) const {
		Q_UNUSED(actionName);
		return actionCategories & Kexi::GlobalActionCategory;
	}
};

//! @internal Used to display KActions (in column 2)
class CurrentFormActionsListView : public KActionsListViewBase
{
public:
	CurrentFormActionsListView(QWidget* parent, KexiMainWindow* mainWin)
		: KActionsListViewBase(parent, mainWin)
	{
	}
	virtual ~CurrentFormActionsListView() {}

	virtual bool isActionVisible(const char* actionName, int actionCategories) const {
		return actionCategories & Kexi::WindowActionCategory
				&& Kexi::actionCategories()->actionSupportsObjectType(actionName, KexiPart::FormObjectType);
	}
};

//! @internal a list view displaying action categories user can select from (column 1)
class ActionCategoriesListView : public ActionsListViewBase
{
public:
	ActionCategoriesListView(QWidget* parent) //, KexiProject& project)
	 : ActionsListViewBase(parent)
	{
		Q3ListViewItem *item = new ActionSelectorDialogListItem("noaction", this, i18n("No action") );
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
		Q3ListViewItem *formItem = itemForAction("form");
		if (formItem) {
			item = new ActionSelectorDialogListItem("currentForm", formItem, 
				i18n("Current form's actions", "Current"));
		}
		adjustColumn(0);
		setMinimumWidth( columnWidth(0) + 6 );
	}

	~ActionCategoriesListView()
	{
	}

	//! \return item for action \a actionName, reimplemented to support KexiBrowserItem items
	virtual Q3ListViewItem *itemForAction(const QString& actionName)
	{
		for (Q3ListViewItemIterator it(this); it.current(); ++it) {
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
				KAction *a = KStandardAction::printPreview(0, 0, 0);
				item = new ActionSelectorDialogListItem("printPreview", printItem, 
					a->text().replace("&", "").replace("...", ""));
				item->setPixmap(0, SmallIcon(a->icon()));
				delete a;
				item = new ActionSelectorDialogListItem("pageSetup", printItem, i18n("Show Page Setup"));
				item->setPixmap(0, noIcon);
				setOpen(printItem, true);
				printItem->setExpandable(false);
			}
			if (part->info()->isDataExportSupported()) {
				ActionSelectorDialogListItem *exportItem = new ActionSelectorDialogListItem(
					"exportToCSV", this, 
					i18n("Note: use multiple rows if needed", "Export to File\nAs Data Table"));
				exportItem->setMultiLinesEnabled(true);
				exportItem->setPixmap(0, SmallIcon("table"));
				item = new ActionSelectorDialogListItem("copyToClipboardAsCSV", 
					exportItem, 
					i18n("Note: use multiple rows if needed", "Copy to Clipboard\nAs Data Table"));
				item->setPixmap(0, SmallIcon("table"));
				item->setMultiLinesEnabled(true);
				setOpen(exportItem, true);
				exportItem->setExpandable(false);
			}
			item = new ActionSelectorDialogListItem("new", this, i18n("Create New Object"));
			item->setPixmap(0, SmallIcon("filenew"));
			if (supportedViewModes & Kexi::DesignViewMode) {
				item = new ActionSelectorDialogListItem("design", this, i18n("Open in Design View"));
				item->setPixmap(0, SmallIcon("edit"));
			}
			if (supportedViewModes & Kexi::TextViewMode) {
				item = new ActionSelectorDialogListItem("editText", this, i18n("Open in Text View"));
				item->setPixmap(0, noIcon);
			}
			item = new ActionSelectorDialogListItem("close", this, i18n("Close View"));
			item->setPixmap(0, SmallIcon("fileclose"));
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
		: kactionPageWidget(0), kactionListView(0), objectsListView(0)
		, currentFormActionsPageWidget(0)
		, currentFormActionsListView(0)
		, secondAnd3rdColumnMainWidget(0)
		, hideActionToExecuteListView(false)
	{
	}

	void raiseWidget(QWidget *w)
	{
		secondAnd3rdColumnStack->raiseWidget( w );
		selectActionToBeExecutedLbl->setBuddy(w);
	}

	void updateSelectActionToBeExecutedMessage(const QString& actionType)
	{
		QString msg;
		if (actionType=="noaction")
			msg = QString::null;
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

	// changes 3rd column visibility
	void setActionToExecuteSectionVisible(bool visible, bool force = false)
	{
		if (!force && hideActionToExecuteListView != visible)
			return;
		hideActionToExecuteListView = !visible;
		actionToExecuteListView->hide();
		actionToExecuteLbl->hide();
		actionToExecuteListView->show();
		actionToExecuteLbl->show();
	}

	KexiMainWindow* mainWin;
	QString actionWidgetName;
	ActionCategoriesListView* actionCategoriesListView; //!< for column #1
	QWidget *kactionPageWidget;
	KActionsListView* kactionListView;  //!< for column #2
	KexiBrowser* objectsListView; //!< for column #2
	QWidget *currentFormActionsPageWidget; //!< for column #2
	CurrentFormActionsListView* currentFormActionsListView; //!< for column #2
	QWidget *emptyWidget;
	QLabel* selectActionToBeExecutedLbl;
	ActionToExecuteListView* actionToExecuteListView;
	QLabel *actionToExecuteLbl;
	QWidget *secondAnd3rdColumnMainWidget;
	QWidgetStack *secondAnd3rdColumnStack, *secondColumnStack;
	bool hideActionToExecuteListView;
};

//-------------------------------------

KexiActionSelectionDialog::KexiActionSelectionDialog(KexiMainWindow* mainWin, QWidget *parent, 
	const KexiFormEventAction::ActionData& action, const Q3CString& actionWidgetName)
	: KDialogBase(parent, "actionSelectorDialog", true, i18n("Assigning Action to Command Button"), 
		KDialogBase::Ok | KDialogBase::Cancel )
	, d( new KexiActionSelectionDialogPrivate() )
{
	d->mainWin = mainWin;
	d->actionWidgetName = actionWidgetName;
	setButtonOK( KGuiItem(i18n("Assign action", "&Assign"), "button_ok", i18n("Assign action")) );

	QWidget *mainWidget = new QWidget( this );
	mainWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setMainWidget(mainWidget);

/*    lbl 1
   +------------+ +-------------------------------+
   |            | |              [a]              |
   | 1st column | | +----------- + +------------+ |
   |            | | | 2nd column | | 3rd column | |
   |            | | +            + +            + |
   |            | | +------------+ +------------+ |
   +------------+ +-------------------------------+
   \______________________________________________/
                        glyr
  [a]- QWidgetStack *secondAnd3rdColumnStack, 
    - for displaying KActions, the stack contains d->kactionPageWidget QWidget
    - for displaying objects, the stack contains secondAnd3rdColumnMainWidget QWidget and Q3GridLayout *secondAnd3rdColumnGrLyr
	 - kactionPageWidget contains only a Q3VBoxLayout and label+kactionListView
*/
	Q3GridLayout *glyr = new Q3GridLayout(mainWidget, 2, 2, KDialog::marginHint(), KDialog::spacingHint());
	glyr->setRowStretch(1, 1);

	// 1st column: action types
	d->actionCategoriesListView = new ActionCategoriesListView(mainWidget);
	d->actionCategoriesListView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	glyr->addWidget(d->actionCategoriesListView, 1, 0);
	connect( d->actionCategoriesListView, SIGNAL(selectionChanged(Q3ListViewItem*)), 
		this, SLOT(slotActionCategorySelected(Q3ListViewItem*)));

	QLabel *lbl = new QLabel(d->actionCategoriesListView, i18n("Action category:"), mainWidget);
	lbl->setMinimumHeight(lbl->fontMetrics().height()*2);
	lbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	lbl->setAlignment(Qt::AlignTop|Qt::AlignLeft|Qt::TextWordWrap);
	glyr->addWidget(lbl, 0, 0, Qt::AlignTop|Qt::AlignLeft);

	// widget stack for 2nd and 3rd column
	d->secondAnd3rdColumnStack = new QWidgetStack(mainWidget);
	d->secondAnd3rdColumnStack->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	glyr->addMultiCellWidget(d->secondAnd3rdColumnStack, 0, 1, 1, 1);//, Qt::AlignTop|Qt::AlignLeft);

	d->secondAnd3rdColumnMainWidget = new QWidget(d->secondAnd3rdColumnStack);
	d->secondAnd3rdColumnMainWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	Q3GridLayout *secondAnd3rdColumnGrLyr = new Q3GridLayout(d->secondAnd3rdColumnMainWidget, 2, 2, 0, KDialog::spacingHint());
	secondAnd3rdColumnGrLyr->setRowStretch(1, 2);
	d->secondAnd3rdColumnStack->addWidget(d->secondAnd3rdColumnMainWidget);

	// 2nd column: list of actions/objects
	d->objectsListView = new KexiBrowser(d->secondAnd3rdColumnMainWidget, d->mainWin, 0/*features*/);
	secondAnd3rdColumnGrLyr->addWidget(d->objectsListView, 1, 0);
	connect(d->objectsListView, SIGNAL(selectionChanged(KexiPart::Item*)),
		this, SLOT(slotItemForOpeningOrExecutingSelected(KexiPart::Item*)));

	d->selectActionToBeExecutedLbl = new QLabel(d->secondAnd3rdColumnMainWidget);
	d->selectActionToBeExecutedLbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	d->selectActionToBeExecutedLbl->setAlignment(Qt::AlignTop|Qt::AlignLeft|Qt::TextWordWrap);
	d->selectActionToBeExecutedLbl->setMinimumHeight(d->selectActionToBeExecutedLbl->fontMetrics().height()*2);
	secondAnd3rdColumnGrLyr->addWidget(d->selectActionToBeExecutedLbl, 0, 0, Qt::AlignTop|Qt::AlignLeft);

	d->emptyWidget = new QWidget(d->secondAnd3rdColumnStack);
	d->secondAnd3rdColumnStack->addWidget(d->emptyWidget);

	// 3rd column: actions to execute
	d->actionToExecuteListView = new ActionToExecuteListView(d->secondAnd3rdColumnMainWidget);
	d->actionToExecuteListView->installEventFilter(this); //to be able to disable painting
	d->actionToExecuteListView->viewport()->installEventFilter(this); //to be able to disable painting
	d->actionToExecuteListView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	connect(d->actionToExecuteListView, SIGNAL(executed(Q3ListViewItem*)),
		this, SLOT(slotActionToExecuteItemExecuted(Q3ListViewItem*)));
	connect(d->actionToExecuteListView, SIGNAL(selectionChanged(Q3ListViewItem*)),
		this, SLOT(slotActionToExecuteItemSelected(Q3ListViewItem*)));
	secondAnd3rdColumnGrLyr->addWidget(d->actionToExecuteListView, 1, 1);

	d->actionToExecuteLbl = new QLabel(d->actionToExecuteListView, 
		i18n("Action to execute:"), d->secondAnd3rdColumnMainWidget);
	d->actionToExecuteLbl->installEventFilter(this); //to be able to disable painting
	d->actionToExecuteLbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	d->actionToExecuteLbl->setAlignment(Qt::AlignTop|Qt::AlignLeft|Qt::TextWordWrap);
	secondAnd3rdColumnGrLyr->addWidget(d->actionToExecuteLbl, 0, 1, Qt::AlignTop|Qt::AlignLeft);

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
		else if (actionType=="currentForm") {
			d->currentFormActionsListView->selectAction(actionArg);
			d->currentFormActionsListView->setFocus();
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

void KexiActionSelectionDialog::slotKActionItemExecuted(Q3ListViewItem*)
{
	accept();
}

void KexiActionSelectionDialog::slotKActionItemSelected(Q3ListViewItem*)
{
	d->setActionToExecuteSectionVisible(false);
	updateOKButtonStatus();
}

void KexiActionSelectionDialog::slotCurrentFormActionItemExecuted(Q3ListViewItem*)
{
	accept();
}

void KexiActionSelectionDialog::slotCurrentFormActionItemSelected(Q3ListViewItem*)
{
	d->setActionToExecuteSectionVisible(false);
	updateOKButtonStatus();
}

void KexiActionSelectionDialog::slotItemForOpeningOrExecutingSelected(KexiPart::Item* item)
{
	d->setActionToExecuteSectionVisible(item);
}

void KexiActionSelectionDialog::slotActionToExecuteItemExecuted(Q3ListViewItem* item)
{
	if (!item)
		return;
	ActionSelectorDialogListItemBase *listItem = dynamic_cast<ActionSelectorDialogListItemBase*>(item);
	if (listItem && !listItem->data.isEmpty())
		accept();
}

void KexiActionSelectionDialog::slotActionToExecuteItemSelected(Q3ListViewItem*)
{
	updateOKButtonStatus();
}

void KexiActionSelectionDialog::slotActionCategorySelected(Q3ListViewItem* item)
{
	ActionSelectorDialogListItem *simpleItem = dynamic_cast<ActionSelectorDialogListItem*>(item);
	// simple case: part-less item, e.g. kaction:
	if (simpleItem) {
		d->updateSelectActionToBeExecutedMessage(simpleItem->data);
		QString selectActionToBeExecutedMsg(
			i18n("&Select action to be executed after clicking \"%1\" button:")); // msg for a label
		if (simpleItem->data == "kaction") {
			if (!d->kactionPageWidget) {
				//create lbl+list view with a vlayout
				d->kactionPageWidget = new QWidget();
				d->kactionPageWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
				Q3VBoxLayout *vlyr = new Q3VBoxLayout(d->kactionPageWidget, 0, KDialog::spacingHint());
				d->kactionListView = new KActionsListView(d->kactionPageWidget, d->mainWin);
				d->kactionListView->init();
				QLabel *lbl = new QLabel(d->kactionListView, selectActionToBeExecutedMsg.arg(d->actionWidgetName),
					d->kactionPageWidget);
				lbl->setAlignment(Qt::AlignTop|Qt::AlignLeft|Qt::WordBreak);
				lbl->setMinimumHeight(lbl->fontMetrics().height()*2);
				vlyr->addWidget(lbl);
				vlyr->addWidget(d->kactionListView);
				d->secondAnd3rdColumnStack->addWidget(d->kactionPageWidget);
				connect(d->kactionListView, SIGNAL(executed(Q3ListViewItem*)),
					this, SLOT(slotKActionItemExecuted(Q3ListViewItem*)));
				connect( d->kactionListView, SIGNAL(selectionChanged(Q3ListViewItem*)), 
					this, SLOT(slotKActionItemSelected(Q3ListViewItem*)));
			}
			d->setActionToExecuteSectionVisible(false);
			d->raiseWidget(d->kactionPageWidget);
			slotKActionItemSelected(d->kactionListView->selectedItem()); //to refresh column #3
		}
		else if (simpleItem->data == "currentForm") {
			if (!d->currentFormActionsPageWidget) {
				//create lbl+list view with a vlayout
				d->currentFormActionsPageWidget = new QWidget();
				d->currentFormActionsPageWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
				Q3VBoxLayout *vlyr = new Q3VBoxLayout(d->currentFormActionsPageWidget, 0, KDialog::spacingHint());
				d->currentFormActionsListView = new CurrentFormActionsListView(
					d->currentFormActionsPageWidget, d->mainWin);
				d->currentFormActionsListView->init();
				QLabel *lbl = new QLabel(d->currentFormActionsListView, 
					selectActionToBeExecutedMsg.arg(d->actionWidgetName), d->currentFormActionsPageWidget);
				lbl->setAlignment(Qt::AlignTop|Qt::AlignLeft|Qt::WordBreak);
				lbl->setMinimumHeight(lbl->fontMetrics().height()*2);
				vlyr->addWidget(lbl);
				vlyr->addWidget(d->currentFormActionsListView);
				d->secondAnd3rdColumnStack->addWidget(d->currentFormActionsPageWidget);
				connect(d->currentFormActionsListView, SIGNAL(executed(Q3ListViewItem*)),
					this, SLOT(slotCurrentFormActionItemExecuted(Q3ListViewItem*)));
				connect( d->currentFormActionsListView, SIGNAL(selectionChanged(Q3ListViewItem*)), 
					this, SLOT(slotCurrentFormActionItemSelected(Q3ListViewItem*)));
			}
			d->setActionToExecuteSectionVisible(false);
			d->raiseWidget(d->currentFormActionsPageWidget);
			slotCurrentFormActionItemSelected(d->currentFormActionsListView->selectedItem()); //to refresh column #3
		}
		else if (simpleItem->data == "noaction") {
			d->raiseWidget(d->emptyWidget);
			d->objectsListView->clearSelection();
			//hide column #3
			d->setActionToExecuteSectionVisible(false);
		}
		d->actionCategoriesListView->update();
		updateOKButtonStatus();
		return;
	}
	// other case
	KexiBrowserItem* browserItem = dynamic_cast<KexiBrowserItem*>(item);
	if (browserItem) {
		d->updateSelectActionToBeExecutedMessage(browserItem->info()->objectName());
		if (d->objectsListView->itemsMimeType().latin1()!=browserItem->info()->mimeType()) {
			d->objectsListView->setProject(d->mainWin->project(), browserItem->info()->mimeType());
			d->actionToExecuteListView->showActionsForMimeType( browserItem->info()->mimeType() );
			d->setActionToExecuteSectionVisible(false);
		}
		if (d->secondAnd3rdColumnStack->visibleWidget()!=d->secondAnd3rdColumnMainWidget) {
			d->raiseWidget( d->secondAnd3rdColumnMainWidget );
			d->objectsListView->clearSelection();
			d->setActionToExecuteSectionVisible(false, true);
		}
		else
			d->raiseWidget( d->secondAnd3rdColumnMainWidget );
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
		else if (simpleItem->data == "currentForm") {
			if (d->currentFormActionsListView->selectedItem()) {
				data.string = QString("currentForm:")
					+ dynamic_cast<ActionSelectorDialogListItem*>( 
						d->currentFormActionsListView->selectedItem() )->data;
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

bool KexiActionSelectionDialog::eventFilter(QObject *o, QEvent *e)
{
	if (d->hideActionToExecuteListView)
		return true;
	return KDialogBase::eventFilter(o, e);
}

#include "kexiactionselectiondialog.moc"
#include "kexiactionselectiondialog_p.moc"
