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

#include <qbitmap.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <Q3Header>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QStackedWidget>

#include <widget/utils/klistviewitemtemplate.h>
#include <widget/KexiProjectListView.h>
#include <widget/KexiProjectListViewItem.h>
#include <kexiutils/utils.h>
#include <kexi_global.h>

typedef KListViewItemTemplate<QString> ActionSelectorDialogListItemBase;

class ActionSelectorDialogListItem : public ActionSelectorDialogListItemBase
{
public:
    ActionSelectorDialogListItem(const QString& data, Q3ListView *parent, QString label1)
            : ActionSelectorDialogListItemBase(data, parent, label1)
            , fifoSorting(true) {
        m_sortKey.sprintf("%2.2d", parent->childCount());
    }

    ActionSelectorDialogListItem(const QString& data, Q3ListViewItem *parent, QString label1)
            : ActionSelectorDialogListItemBase(data, parent, label1)
            , fifoSorting(true) {
        m_sortKey.sprintf("%2.2d", parent->childCount());
    }

    virtual QString key(int column, bool ascending) const {
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

KActionsListViewBase::KActionsListViewBase(QWidget* parent)
        : ActionsListViewBase(parent)
{
}

KActionsListViewBase::~KActionsListViewBase() {}

void KActionsListViewBase::init()
{
    setSorting(0);
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
        pitem->setPixmap(0, action->icon().pixmap(16));
        if (!pitem->pixmap(0) || pitem->pixmap(0)->isNull())
            pitem->setPixmap(0, noIcon);
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
        Q3ListViewItem *item = new ActionSelectorDialogListItem(
            "noaction", this, i18n("No action"));
        const QPixmap noIcon(KexiUtils::emptyIcon(KIconLoader::Small));
        item->setPixmap(0, noIcon);
        item = new ActionSelectorDialogListItem("kaction", this, i18n("Application actions"));
        item->setPixmap(0, SmallIcon("form_action"));

        KexiPart::PartInfoList *pl = Kexi::partManager().partInfoList();
        foreach(KexiPart::Info *info, *pl) {
            KexiPart::Part *part = Kexi::partManager().part(info);
            if (!info->isVisibleInNavigator() || !part)
                continue;
            item = new KexiProjectListViewItem(this, info);
            item->setText(0, part->instanceCaption());
        }
        Q3ListViewItem *formItem = itemForAction("form");
        if (formItem) {
            item = new ActionSelectorDialogListItem("currentForm", formItem,
                                                    i18nc("Current form's actions", "Current"));
        }
        adjustColumn(0);
        setMinimumWidth(columnWidth(0) + 6);
    }

    ~ActionCategoriesListView() {
    }

    //! \return item for action \a actionName, reimplemented to support KexiBrowserItem items
    virtual Q3ListViewItem *itemForAction(const QString& actionName) {
        for (Q3ListViewItemIterator it(this); it.current(); ++it) {
            //simple case
            ActionSelectorDialogListItem* item
            = dynamic_cast<ActionSelectorDialogListItem*>(it.current());
            if (item) {
                if (item->data == actionName)
                    return it.current();
                continue;
            }
            KexiProjectListViewItem* bitem = dynamic_cast<KexiProjectListViewItem*>(it.current());
            if (bitem) {
                if (bitem->partInfo()->objectName() == actionName)
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
        int supportedViewModes = part->supportedViewModes();
        ActionSelectorDialogListItem *item;
        const QPixmap noIcon(KexiUtils::emptyIcon(KIconLoader::Small));
        if (supportedViewModes & Kexi::DataViewMode) {
            item = new ActionSelectorDialogListItem("open", this, i18n("Open in Data View"));
            item->setPixmap(0, SmallIcon("document-open"));
        }
        if (part->info()->isExecuteSupported()) {
            item = new ActionSelectorDialogListItem("execute", this, i18n("Execute"));
            item->setPixmap(0, SmallIcon("media-playback-start"));
        }
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
        if (part->info()->isDataExportSupported()) {
            ActionSelectorDialogListItem *exportItem = new ActionSelectorDialogListItem(
                "exportToCSV", this,
                i18nc("Note: use multiple rows if needed", "Export to File\nAs Data Table"));
            exportItem->setMultiLinesEnabled(true);
            exportItem->setPixmap(0, SmallIcon("table"));
            item = new ActionSelectorDialogListItem("copyToClipboardAsCSV",
                                                    exportItem,
                                                    i18nc("Note: use multiple rows if needed", "Copy to Clipboard\nAs Data Table"));
            item->setPixmap(0, SmallIcon("table"));
            item->setMultiLinesEnabled(true);
            setOpen(exportItem, true);
            exportItem->setExpandable(false);
        }
        item = new ActionSelectorDialogListItem("new", this, i18n("Create New Object"));
        item->setPixmap(0, SmallIcon("document-new"));
        if (supportedViewModes & Kexi::DesignViewMode) {
            item = new ActionSelectorDialogListItem("design", this, i18n("Open in Design View"));
            item->setPixmap(0, SmallIcon("document-properties"));
        }
        if (supportedViewModes & Kexi::TextViewMode) {
            item = new ActionSelectorDialogListItem("editText", this, i18n("Open in Text View"));
            item->setPixmap(0, noIcon);
        }
        item = new ActionSelectorDialogListItem("close", this, i18n("Close View"));
        item->setPixmap(0, SmallIcon("window-close"));
        updateWidth();
    }

    void updateWidth() {
        adjustColumn(0);
        setMinimumWidth(columnWidth(0));
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
    KexiProjectListView* objectsListView; //!< for column #2
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
    setWindowTitle(i18n("Assigning Action to Command Button"));
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
    connect(d->actionCategoriesListView, SIGNAL(selectionChanged(Q3ListViewItem*)),
            this, SLOT(slotActionCategorySelected(Q3ListViewItem*)));

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
    d->objectsListView = new KexiProjectListView(d->secondAnd3rdColumnMainWidget, KexiProjectListView::NoFeatures);
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
    connect(d->actionToExecuteListView, SIGNAL(executed(Q3ListViewItem*)),
            this, SLOT(slotActionToExecuteItemExecuted(Q3ListViewItem*)));
    connect(d->actionToExecuteListView, SIGNAL(selectionChanged(Q3ListViewItem*)),
            this, SLOT(slotActionToExecuteItemSelected(Q3ListViewItem*)));
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
    d->actionToExecuteListView->updateWidth();

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
    ActionSelectorDialogListItemBase *listItem
    = dynamic_cast<ActionSelectorDialogListItemBase*>(item);
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
            I18N_NOOP("&Select action to be executed after clicking \"%1\" button:")); // msg for a label
        if (simpleItem->data == "kaction") {
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
                connect(d->kactionListView, SIGNAL(executed(Q3ListViewItem*)),
                        this, SLOT(slotKActionItemExecuted(Q3ListViewItem*)));
                connect(d->kactionListView, SIGNAL(selectionChanged(Q3ListViewItem*)),
                        this, SLOT(slotKActionItemSelected(Q3ListViewItem*)));
            }
            d->setActionToExecuteSectionVisible(false);
            d->raiseWidget(d->kactionPageWidget);
            slotKActionItemSelected(d->kactionListView->selectedItem()); //to refresh column #3
        } else if (simpleItem->data == "currentForm") {
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
                connect(d->currentFormActionsListView, SIGNAL(executed(Q3ListViewItem*)),
                        this, SLOT(slotCurrentFormActionItemExecuted(Q3ListViewItem*)));
                connect(d->currentFormActionsListView, SIGNAL(selectionChanged(Q3ListViewItem*)),
                        this, SLOT(slotCurrentFormActionItemSelected(Q3ListViewItem*)));
            }
            d->setActionToExecuteSectionVisible(false);
            d->raiseWidget(d->currentFormActionsPageWidget);
            slotCurrentFormActionItemSelected(d->currentFormActionsListView->selectedItem()); //to refresh column #3
        } else if (simpleItem->data == "noaction") {
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
    KexiProjectListViewItem* browserItem = dynamic_cast<KexiProjectListViewItem*>(item);
    if (browserItem) {
        d->updateSelectActionToBeExecutedMessage(browserItem->partInfo()->objectName());
        if (d->objectsListView->itemsPartClass() != browserItem->partInfo()->partClass()) {
            d->objectsListView->setProject(
                KexiMainWindowIface::global()->project(), browserItem->partInfo()->partClass());
            d->actionToExecuteListView->showActionsForPartClass(browserItem->partInfo()->partClass());
            d->setActionToExecuteSectionVisible(false);
        }
        if (d->secondAnd3rdColumnStack->currentWidget() != d->secondAnd3rdColumnMainWidget) {
            d->raiseWidget(d->secondAnd3rdColumnMainWidget);
            d->objectsListView->clearSelection();
            d->setActionToExecuteSectionVisible(false, true);
        } else
            d->raiseWidget(d->secondAnd3rdColumnMainWidget);
    }
    d->actionCategoriesListView->update();
    updateOKButtonStatus();
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
                              + dynamic_cast<ActionSelectorDialogListItem*>(d->kactionListView->selectedItem())->data;
                return data;
            }
        } else if (simpleItem->data == "currentForm") {
            if (d->currentFormActionsListView->selectedItem()) {
                data.string = QString("currentForm:")
                              + dynamic_cast<ActionSelectorDialogListItem*>(
                                  d->currentFormActionsListView->selectedItem())->data;
                return data;
            }
        }
    }
    KexiProjectListViewItem* browserItem = dynamic_cast<KexiProjectListViewItem*>(d->actionCategoriesListView->selectedItem());
    if (browserItem) {
        ActionSelectorDialogListItem *actionToExecute = dynamic_cast<ActionSelectorDialogListItem*>(
                    d->actionToExecuteListView->selectedItem());
        if (d->objectsListView && actionToExecute && !actionToExecute->data.isEmpty()) {
            KexiPart::Item* partItem = d->objectsListView->selectedPartItem();
            KexiPart::Info* partInfo = partItem ? Kexi::partManager().infoForClass(partItem->partClass()) : 0;
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
    KPushButton *btn = button(Ok);
    ActionSelectorDialogListItem *simpleItem = dynamic_cast<ActionSelectorDialogListItem*>(
                d->actionCategoriesListView->selectedItem());
    btn->setEnabled(
        (simpleItem && simpleItem->data == "noaction") || !currentAction().isEmpty());
}

bool KexiActionSelectionDialog::eventFilter(QObject *o, QEvent *e)
{
    if (d->hideActionToExecuteListView)
        return true;
    return KDialog::eventFilter(o, e);
}

#include "kexiactionselectiondialog.moc"
#include "kexiactionselectiondialog_p.moc"
