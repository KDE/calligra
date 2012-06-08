/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jarosław Staniek <staniek@kde.org>

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

#include <kexipartitem.h>
#include <kexiproject.h>
#include <kexipartinfo.h>
#include <kexipart.h>
#include <kexiactioncategories.h>
#include <KexiMainWindowIface.h>

#include <kaction.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kstandardguiitem.h>
#include <kpushbutton.h>

#include <QBitmap>
#include <QLabel>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QStackedWidget>

#include <widget/utils/klistviewitemtemplate.h>
#include <widget/navigator/KexiProjectNavigator.h>
#include <widget/navigator/KexiProjectModel.h>
#include <kexiutils/utils.h>
#include <kexi_global.h>

class ActionSelectorDialogListItem : public QListWidgetItem
{
public:

    ActionSelectorDialogListItem(const QString& data, QListWidget *parent, QString label1)
            : QListWidgetItem(label1, parent)
            , fifoSorting(true) {
        m_sortKey.sprintf("%2.2d", parent->count());
	setData(Qt::UserRole, data);
    }

    virtual QString key(int column, bool ascending) const {
        return fifoSorting ? m_sortKey : text();
    }

    bool fifoSorting : 1;

protected:
    QString m_sortKey;
};

//---------------------------------------

ActionsListViewBase::ActionsListViewBase(QWidget* parent)
        : QListWidget(parent)
{
    setResizeMode(QListView::Adjust);
}

ActionsListViewBase::~ActionsListViewBase()
{
}

QListWidgetItem *ActionsListViewBase::itemForAction(const QString& actionName)
{
    for (int i = 0; i < count(); ++i) {
        ActionSelectorDialogListItem* itm = dynamic_cast<ActionSelectorDialogListItem*>(item(i));
        if (itm && itm->data(Qt::UserRole).toString() == actionName)
            return itm;
    }
    return 0;
}

void ActionsListViewBase::selectAction(const QString& actionName)
{
    QListWidgetItem *item = itemForAction(actionName);
    if (item) {
        item->setSelected(true);
    }
}

//---------------------------------------

KActionsListViewBase::KActionsListViewBase(QWidget* parent)
        : ActionsListViewBase(parent)
{
}

KActionsListViewBase::~KActionsListViewBase() {}

void KActionsListViewBase::init()
{
    const QPixmap noIcon(KexiUtils::emptyIcon(KIconLoader::Small));
    QList<QAction*> sharedActions(KexiMainWindowIface::global()->allActions());
    const Kexi::ActionCategories *acat = Kexi::actionCategories();
    foreach(QAction *action, sharedActions) {
//   kDebug() << (*it)->name() << " " << (*it)->text();
        //! @todo group actions
        //! @todo: store KAction* here?
        const int actionCategories = acat->actionCategories(action->objectName().toLatin1());
        if (actionCategories == -1) {
            kWarning() << "no category declared for action \""
                << action->objectName() << "\"! Fix this!";
            continue;
        }
        if (!isActionVisible(action->objectName().toLatin1(), actionCategories))
            continue;
        ActionSelectorDialogListItem *pitem = new ActionSelectorDialogListItem(
            action->objectName(), this,
            action->toolTip().isEmpty() ? action->text().replace("&", "") : action->toolTip());
        pitem->fifoSorting = false; //alpha sort
        pitem->setIcon(action->icon());
        if (pitem->icon().isNull())
            pitem->setIcon(noIcon);
    }
}

//---------------------------------------

//! @internal Used to display KActions (in column 2)
class KActionsListView : public KActionsListViewBase
{
public:
    KActionsListView(QWidget* parent)
            : KActionsListViewBase(parent) {
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
    CurrentFormActionsListView(QWidget* parent)
            : KActionsListViewBase(parent) {
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
            : ActionsListViewBase(parent) {
        QListWidgetItem *itm = new ActionSelectorDialogListItem("noaction", this, i18n("No action"));
        const QPixmap noIcon(KexiUtils::emptyIcon(KIconLoader::Small));
        itm->setIcon(noIcon);
	
	itm = new ActionSelectorDialogListItem("currentForm", this, i18nc("Current form's actions", "Current"));
	itm->setIcon(SmallIcon("form"));
	
        itm = new ActionSelectorDialogListItem("kaction", this, i18n("Application actions"));
        itm->setIcon(SmallIcon("kexi"));

        KexiPart::PartInfoList *pl = Kexi::partManager().infoList();
        if (pl) {
            foreach(KexiPart::Info *info, *pl) {
                KexiPart::Part *part = Kexi::partManager().part(info);
                if (!info->isVisibleInNavigator() || !part)
                    continue;
                itm = new QListWidgetItem(this);
		itm->setData(Qt::UserRole, info->partClass());
                itm->setText(part->info()->instanceCaption());
		itm->setIcon(SmallIcon(part->info()->itemIcon()));
            }
        }

           
        
       
    }

    ~ActionCategoriesListView() {
    }

    //! \return item for action \a actionName, reimplemented to support KexiBrowserItem items
    virtual QListWidgetItem *itemForAction(const QString& actionName) {
        for (int i = 0; i < count(); ++i) {
            //simple case
            ActionSelectorDialogListItem* itm = dynamic_cast<ActionSelectorDialogListItem*>(item(i));
            if (itm) {
                if (itm->data(Qt::UserRole).toString() == actionName)
                    return itm;
                continue;
            }
            /*TODO?
            QListWidgetItem* bitm = item(i);
            if (bitm) {
                if (bitm->data().toString() == actionName)
                    return bitm;
            }*/
        }
        return 0;
    }
};

//! @internal Used to display list of actions available to executing (column 3)
class ActionToExecuteListView : public ActionsListViewBase
{
public:
    ActionToExecuteListView(QWidget* parent)
            : ActionsListViewBase(parent) {
    }

    ~ActionToExecuteListView() {
    }

    //! Updates actions
    void showActionsForPartClass(const QString& partClass) {
        if (m_currentPartClass == partClass)
            return;
        m_currentPartClass = partClass;
        clear();
        KexiPart::Part *part = Kexi::partManager().partForClass(m_currentPartClass);
        if (!part)
            return;
        Kexi::ViewModes supportedViewModes = part->info()->supportedViewModes();
        ActionSelectorDialogListItem *item;
        const QPixmap noIcon(KexiUtils::emptyIcon(KIconLoader::Small));
        if (supportedViewModes & Kexi::DataViewMode) {
            item = new ActionSelectorDialogListItem("open", this, i18n("Open in Data View"));
            item->setIcon(SmallIcon("document-open"));
        }
        if (part->info()->isExecuteSupported()) {
            item = new ActionSelectorDialogListItem("execute", this, i18n("Execute"));
            item->setIcon(SmallIcon("media-playback-start"));
        }
#ifndef KEXI_NO_QUICK_PRINTING
        if (part->info()->isPrintingSupported()) {
            ActionSelectorDialogListItem *printItem = new ActionSelectorDialogListItem(
                "print", this, i18n("Print"));
            printItem->setPixmap(0, SmallIcon("document-print"));
            KAction *a = KStandardAction::printPreview(0, 0, 0);
            item = new ActionSelectorDialogListItem("printPreview", printItem,
                                                    a->text().replace("&", "").replace("...", ""));
            item->setPixmap(0, a->icon().pixmap(16));
            delete a;
            item = new ActionSelectorDialogListItem(
                "pageSetup", printItem, i18n("Show Page Setup"));
            item->setPixmap(0, noIcon);
            setOpen(printItem, true);
            printItem->setExpandable(false);
        }
#endif
        if (part->info()->isDataExportSupported()) {
            ActionSelectorDialogListItem *exportItem = new ActionSelectorDialogListItem(
                "exportToCSV", this,
                i18nc("Note: use multiple rows if needed", "Export to File\nAs Data Table"));
            //TODO exportItem->setMultiLinesEnabled(true);
            exportItem->setIcon(SmallIcon("table"));
            item = new ActionSelectorDialogListItem("copyToClipboardAsCSV",
                                                    this,
                                                    i18nc("Note: use multiple rows if needed", "Copy to Clipboard\nAs Data Table"));
            item->setIcon(SmallIcon("table"));
        }
        item = new ActionSelectorDialogListItem("new", this, i18n("Create New Object"));
        item->setIcon(SmallIcon("document-new"));
        if (supportedViewModes & Kexi::DesignViewMode) {
            item = new ActionSelectorDialogListItem("design", this, i18n("Open in Design View"));
            item->setIcon(SmallIcon("document-properties"));
        }
        if (supportedViewModes & Kexi::TextViewMode) {
            item = new ActionSelectorDialogListItem("editText", this, i18n("Open in Text View"));
            item->setIcon(noIcon);
        }
        item = new ActionSelectorDialogListItem("close", this, i18n("Close View"));
        item->setIcon(SmallIcon("window-close"));
    }

    QString m_currentPartClass;
};

//-------------------------------------

//! @internal
class KexiActionSelectionDialog::Private
{
public:
    Private()
            : kactionPageWidget(0), kactionListView(0), objectsListView(0)
            , currentFormActionsPageWidget(0)
            , currentFormActionsListView(0)
            , secondAnd3rdColumnMainWidget(0)
            , hideActionToExecuteListView(false) {
    }

    void raiseWidget(QWidget *w) {
        secondAnd3rdColumnStack->setCurrentWidget(w);
        selectActionToBeExecutedLbl->setBuddy(w);
    }

    void updateSelectActionToBeExecutedMessage(const QString& actionType) {
        QString msg;
        if (actionType == "noaction")
            msg = QString();
        // hardcoded, but it's not that bad
        else if (actionType == "macro")
            msg = i18n(
                      "&Select macro to be executed after clicking \"%1\" button:", actionWidgetName);
        else if (actionType == "script")
            msg = i18n(
                      "&Select script to be executed after clicking \"%1\" button:", actionWidgetName);
        //default: table/query/form/report...
        else
            msg = i18n(
                      "&Select object to be opened after clicking \"%1\" button:", actionWidgetName);
        selectActionToBeExecutedLbl->setText(msg);
    }

    // changes 3rd column visibility
    void setActionToExecuteSectionVisible(bool visible, bool force = false) {
        if (!force && hideActionToExecuteListView != visible)
            return;
        hideActionToExecuteListView = !visible;
        actionToExecuteListView->hide();
        actionToExecuteLbl->hide();
        actionToExecuteListView->show();
        actionToExecuteLbl->show();
    }

    QString actionWidgetName;
    ActionCategoriesListView* actionCategoriesListView; //!< for column #1
    QWidget *kactionPageWidget;
    KActionsListView* kactionListView;  //!< for column #2
    KexiProjectNavigator* objectsListView; //!< for column #2
    QWidget *currentFormActionsPageWidget; //!< for column #2
    CurrentFormActionsListView* currentFormActionsListView; //!< for column #2
    QWidget *emptyWidget;
    QLabel* selectActionToBeExecutedLbl;
    ActionToExecuteListView* actionToExecuteListView;
    QLabel *actionToExecuteLbl;
    QWidget *secondAnd3rdColumnMainWidget;
    QGridLayout *glyr;
    QGridLayout *secondAnd3rdColumnGrLyr;
    QStackedWidget *secondAnd3rdColumnStack; //, *secondColumnStack;
    bool hideActionToExecuteListView;
};

//-------------------------------------

KexiActionSelectionDialog::KexiActionSelectionDialog(
    QWidget *parent, const KexiFormEventAction::ActionData& action,
    const QString& actionWidgetName)
        : KDialog(parent)
        , d(new Private())
{
    setModal(true);
    setObjectName("actionSelectorDialog");
    setWindowTitle(i18n("Assigning Action to Button"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    d->actionWidgetName = actionWidgetName;
    setButtonGuiItem(KDialog::Ok,
                     KGuiItem(i18nc("Assign action", "&Assign"), "dialog-ok", i18n("Assign action")));

    QWidget *mainWidget = new QWidget(this);
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
      [a]- QStackedWidget *secondAnd3rdColumnStack,
        - for displaying KActions, the stack contains d->kactionPageWidget QWidget
        - for displaying objects, the stack contains secondAnd3rdColumnMainWidget QWidget and
          QGridLayout *secondAnd3rdColumnGrLyr
       - kactionPageWidget contains only a QVBoxLayout and label+kactionListView
    */
//    d->glyr = new QGridLayout(mainWidget, 2, 2, KDialog::marginHint(), KDialog::spacingHint());
    d->glyr = new QGridLayout(mainWidget); // 2x2
    KexiUtils::setStandardMarginsAndSpacing(d->glyr);
    d->glyr->setRowStretch(1, 1);

    // 1st column: action types
    d->actionCategoriesListView = new ActionCategoriesListView(mainWidget);
    d->actionCategoriesListView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    d->glyr->addWidget(d->actionCategoriesListView, 1, 0);
    connect(d->actionCategoriesListView, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(slotActionCategorySelected(QListWidgetItem*)));

    QLabel *lbl = new QLabel(i18n("Action category:"), mainWidget);
    lbl->setBuddy(d->actionCategoriesListView);
    lbl->setMinimumHeight(lbl->fontMetrics().height()*2);
    lbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    lbl->setWordWrap(true);
    d->glyr->addWidget(lbl, 0, 0, Qt::AlignTop | Qt::AlignLeft);

    // widget stack for 2nd and 3rd column
    d->secondAnd3rdColumnStack = new QStackedWidget(mainWidget);
    d->secondAnd3rdColumnStack->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//    d->glyr->addMultiCellWidget(d->secondAnd3rdColumnStack, 0, 1, 1, 1);
    d->glyr->addWidget(d->secondAnd3rdColumnStack, 0, 1, 2, 1);

    d->secondAnd3rdColumnMainWidget = new QWidget(d->secondAnd3rdColumnStack);
    d->secondAnd3rdColumnMainWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//    d->secondAnd3rdColumnGrLyr = new QGridLayout(
//        d->secondAnd3rdColumnMainWidget, 2, 2, 0, KDialog::spacingHint());
    d->secondAnd3rdColumnGrLyr = new QGridLayout(d->secondAnd3rdColumnMainWidget);
    KDialog::resizeLayout(d->secondAnd3rdColumnGrLyr, 0, KDialog::spacingHint());
    d->secondAnd3rdColumnGrLyr->setRowStretch(1, 2);
    d->secondAnd3rdColumnStack->addWidget(d->secondAnd3rdColumnMainWidget);

    // 2nd column: list of actions/objects
    d->objectsListView = new KexiProjectNavigator(d->secondAnd3rdColumnMainWidget, KexiProjectNavigator::NoFeatures);
    d->secondAnd3rdColumnGrLyr->addWidget(d->objectsListView, 1, 0);
    connect(d->objectsListView, SIGNAL(selectionChanged(KexiPart::Item*)),
            this, SLOT(slotItemForOpeningOrExecutingSelected(KexiPart::Item*)));

    d->selectActionToBeExecutedLbl = new QLabel(d->secondAnd3rdColumnMainWidget);
    d->selectActionToBeExecutedLbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->selectActionToBeExecutedLbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    d->selectActionToBeExecutedLbl->setWordWrap(true);
    d->selectActionToBeExecutedLbl->setMinimumHeight(
        d->selectActionToBeExecutedLbl->fontMetrics().height()*2);
    d->secondAnd3rdColumnGrLyr->addWidget(
        d->selectActionToBeExecutedLbl, 0, 0, Qt::AlignTop | Qt::AlignLeft);

    d->emptyWidget = new QWidget(d->secondAnd3rdColumnStack);
    d->secondAnd3rdColumnStack->addWidget(d->emptyWidget);

    // 3rd column: actions to execute
    d->actionToExecuteListView = new ActionToExecuteListView(d->secondAnd3rdColumnMainWidget);
    d->actionToExecuteListView->installEventFilter(this); //to be able to disable painting
    d->actionToExecuteListView->viewport()->installEventFilter(this); //to be able to disable painting
    d->actionToExecuteListView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    
    connect(d->actionToExecuteListView, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(slotActionToExecuteItemExecuted(QListWidgetItem*)));
    connect(d->actionToExecuteListView, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(slotActionToExecuteItemSelected(QListWidgetItem*)));
    
    d->secondAnd3rdColumnGrLyr->addWidget(d->actionToExecuteListView, 1, 1);

    d->actionToExecuteLbl = new QLabel(
        i18n("Action to execute:"), d->secondAnd3rdColumnMainWidget);
    d->actionToExecuteLbl->setBuddy(d->actionToExecuteListView);
    d->actionToExecuteLbl->installEventFilter(this); //to be able to disable painting
    d->actionToExecuteLbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->actionToExecuteLbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    d->actionToExecuteLbl->setWordWrap(true);
    d->secondAnd3rdColumnGrLyr->addWidget(d->actionToExecuteLbl, 0, 1, Qt::AlignTop | Qt::AlignLeft);

    // temporary show all sections to avoid resizing the dialog in the future
    d->actionCategoriesListView->selectAction("table");
    d->setActionToExecuteSectionVisible(true);
    adjustSize();
    resize(qMax(700, width()), qMax(450, height()));

    bool ok;
    QString actionType, actionArg;
    KexiPart::Info* partInfo = action.decodeString(actionType, actionArg, ok);
    if (ok) {
        d->actionCategoriesListView->selectAction(actionType);
        if (actionType == "kaction") {
            d->kactionListView->selectAction(actionArg);
            d->kactionListView->setFocus();
        } else if (actionType == "currentForm") {
            d->currentFormActionsListView->selectAction(actionArg);
            d->currentFormActionsListView->setFocus();
        } else if (partInfo
                   && Kexi::partManager().part(partInfo)) // We use the Part Manager
            // to determine whether the Kexi-plugin is installed and whether we like to show
            // it in our list of actions.
        {
            KexiPart::Item *item = KexiMainWindowIface::global()->project()->item(
                                       partInfo, actionArg);
            if (d->objectsListView && item) {
                d->objectsListView->selectItem(*item);
                QString actionOption(action.option);
                if (actionOption.isEmpty())
                    actionOption = "open"; // for backward compatibility
                d->actionToExecuteListView->selectAction(actionOption);
                d->objectsListView->setFocus();
            }
        }
    } else {//invalid assignment or 'noaction'
        d->actionCategoriesListView->selectAction("noaction");
        d->actionCategoriesListView->setFocus();
    }
}

KexiActionSelectionDialog::~KexiActionSelectionDialog()
{
    delete d;
}

void KexiActionSelectionDialog::slotKActionItemExecuted(QListWidgetItem*)
{
    accept();
}

void KexiActionSelectionDialog::slotKActionItemSelected(QListWidgetItem*)
{
    d->setActionToExecuteSectionVisible(false);
    updateOKButtonStatus();
}

void KexiActionSelectionDialog::slotCurrentFormActionItemExecuted(QListWidgetItem*)
{
    accept();
}

void KexiActionSelectionDialog::slotCurrentFormActionItemSelected(QListWidgetItem*)
{
    d->setActionToExecuteSectionVisible(false);
    updateOKButtonStatus();
}

void KexiActionSelectionDialog::slotItemForOpeningOrExecutingSelected(KexiPart::Item* item)
{
    d->setActionToExecuteSectionVisible(item);
}

void KexiActionSelectionDialog::slotActionToExecuteItemExecuted(QListWidgetItem* item)
{
    if (!item)
        return;
    ActionSelectorDialogListItem *listItem = dynamic_cast<ActionSelectorDialogListItem*>(item);
    if (listItem && listItem->data(Qt::UserRole).isValid())
        accept();
}

void KexiActionSelectionDialog::slotActionToExecuteItemSelected(QListWidgetItem*)
{
  kDebug();
  updateOKButtonStatus();
}

void KexiActionSelectionDialog::slotActionCategorySelected(QListWidgetItem* item)
{
    ActionSelectorDialogListItem *simpleItem = dynamic_cast<ActionSelectorDialogListItem*>(item);
    // simple case: part-less item, e.g. kaction:
    if (simpleItem) {
        d->updateSelectActionToBeExecutedMessage(simpleItem->data(Qt::UserRole).toString());
        QString selectActionToBeExecutedMsg(
            I18N_NOOP("&Select action to be executed after clicking \"%1\" button:")); // msg for a label
        if (simpleItem->data(Qt::UserRole).toString() == "kaction") {
            if (!d->kactionPageWidget) {
                //create lbl+list view with a vlayout
                d->kactionPageWidget = new QWidget();
                d->kactionPageWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
                QVBoxLayout *vlyr = new QVBoxLayout(d->kactionPageWidget);
                KexiUtils::setMargins(vlyr, 0);
                vlyr->setSpacing(KDialog::spacingHint());
                d->kactionListView = new KActionsListView(d->kactionPageWidget);
                d->kactionListView->init();
                QLabel *lbl = new QLabel(
                    selectActionToBeExecutedMsg.arg(d->actionWidgetName), d->kactionPageWidget);
                lbl->setBuddy(d->kactionListView);
                lbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                lbl->setWordWrap(true);
                lbl->setMinimumHeight(lbl->fontMetrics().height()*2);
                vlyr->addWidget(lbl);
                vlyr->addWidget(d->kactionListView);
                d->secondAnd3rdColumnStack->addWidget(d->kactionPageWidget);
                connect(d->kactionListView, SIGNAL(itemActivated(QListWidgetItem*)),
                        this, SLOT(slotKActionItemExecuted(QListWidgetItem*)));
                connect(d->kactionListView, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                        this, SLOT(slotKActionItemSelected(QListWidgetItem*)));
            }
            d->setActionToExecuteSectionVisible(false);
            d->raiseWidget(d->kactionPageWidget);
            slotKActionItemSelected(d->kactionListView->currentItem()); //to refresh column #3
        } else if (simpleItem->data(Qt::UserRole).toString() == "currentForm") { //TODO move currentForm
            if (!d->currentFormActionsPageWidget) {
                //create lbl+list view with a vlayout
                d->currentFormActionsPageWidget = new QWidget();
                d->currentFormActionsPageWidget->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
                QVBoxLayout *vlyr = new QVBoxLayout(d->currentFormActionsPageWidget);
                KexiUtils::setMargins(vlyr, 0);
                vlyr->setSpacing(KDialog::spacingHint());
                d->currentFormActionsListView = new CurrentFormActionsListView(
                    d->currentFormActionsPageWidget);
                d->currentFormActionsListView->init();
                QLabel *lbl = new QLabel(
                    selectActionToBeExecutedMsg.arg(d->actionWidgetName),
                    d->currentFormActionsPageWidget);
                lbl->setBuddy(d->currentFormActionsListView);
                lbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                lbl->setWordWrap(true);
                lbl->setMinimumHeight(lbl->fontMetrics().height()*2);
                vlyr->addWidget(lbl);
                vlyr->addWidget(d->currentFormActionsListView);
                d->secondAnd3rdColumnStack->addWidget(d->currentFormActionsPageWidget);
                connect(d->currentFormActionsListView, SIGNAL(itemActivated(QListWidgetItem*)),
                        this, SLOT(slotCurrentFormActionItemExecuted(QListWidgetItem*)));
                connect(d->currentFormActionsListView, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                        this, SLOT(slotCurrentFormActionItemSelected(QListWidgetItem*)));
            }
            d->setActionToExecuteSectionVisible(false);
            d->raiseWidget(d->currentFormActionsPageWidget);
            slotCurrentFormActionItemSelected(d->currentFormActionsListView->currentItem()); //to refresh column #3
        } else if (simpleItem->data(Qt::UserRole).toString() == "noaction") {
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

    QString partClass = item->data(Qt::UserRole).toString();
    d->updateSelectActionToBeExecutedMessage(partClass);
    if (d->objectsListView->itemsPartClass() != item->data(Qt::UserRole).toString()) {
	d->objectsListView->setProject(
	    KexiMainWindowIface::global()->project(), partClass);
	d->actionToExecuteListView->showActionsForPartClass(partClass);
	d->setActionToExecuteSectionVisible(false);
    }
    if (d->secondAnd3rdColumnStack->currentWidget() != d->secondAnd3rdColumnMainWidget) {
	d->raiseWidget(d->secondAnd3rdColumnMainWidget);
	d->objectsListView->clearSelection();
	d->setActionToExecuteSectionVisible(false, true);
    } else
	d->raiseWidget(d->secondAnd3rdColumnMainWidget);

      
    d->actionCategoriesListView->update();
    updateOKButtonStatus();
}

KexiFormEventAction::ActionData KexiActionSelectionDialog::currentAction() const
{
    KexiFormEventAction::ActionData data;
    ActionSelectorDialogListItem *simpleItem = dynamic_cast<ActionSelectorDialogListItem*>(
                d->actionCategoriesListView->currentItem());
    // simple case: part-less item, e.g. kaction:
    if (simpleItem) {
        if (simpleItem->data(Qt::UserRole).toString() == "kaction") {
            if (d->kactionListView->currentItem()) {
                data.string = QString("kaction:")
                              + dynamic_cast<ActionSelectorDialogListItem*>(d->kactionListView->currentItem())->data(Qt::UserRole).toString();
                return data;
            }
        } else if (simpleItem->data(Qt::UserRole).toString() == "currentForm") {
            if (d->currentFormActionsListView->currentItem()) {
                data.string = QString("currentForm:")
                              + dynamic_cast<ActionSelectorDialogListItem*>(
                                  d->currentFormActionsListView->currentItem())->data(Qt::UserRole).toString();
                return data;
            }
        }
    }
    

    ActionSelectorDialogListItem *actionToExecute = dynamic_cast<ActionSelectorDialogListItem*>(d->actionToExecuteListView->currentItem());
    if (d->objectsListView && actionToExecute && !actionToExecute->data(Qt::UserRole).toString().isEmpty()) {
	KexiPart::Item* partItem = d->objectsListView->selectedPartItem();
	KexiPart::Info* partInfo = partItem ? Kexi::partManager().infoForClass(partItem->partClass()) : 0;
	if (partInfo) {
	    // opening or executing: table:name, query:name, form:name, macro:name, script:name, etc.
	    data.string = QString("%1:%2").arg(partInfo->objectName()).arg(partItem->name());
	    data.option = actionToExecute->data(Qt::UserRole).toString();
	    return data;
	}
    }

    return data; // No Action
}

void KexiActionSelectionDialog::updateOKButtonStatus()
{
    KPushButton *btn = button(Ok);
    ActionSelectorDialogListItem *simpleItem = dynamic_cast<ActionSelectorDialogListItem*>(d->actionCategoriesListView->currentItem());
    
    kDebug() << currentAction().string;
    btn->setEnabled((simpleItem && simpleItem->data(Qt::UserRole).toString() == "noaction") || !currentAction().isEmpty());
}

bool KexiActionSelectionDialog::eventFilter(QObject *o, QEvent *e)
{
    if (d->hideActionToExecuteListView)
        return true;
    return KDialog::eventFilter(o, e);
}

#include "kexiactionselectiondialog.moc"
#include "kexiactionselectiondialog_p.moc"
