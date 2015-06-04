/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2012 Adam Pigg <adam@piggz.co.uk>

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
#include <kexipartmanager.h>
#include <kexiactioncategories.h>
#include <KexiMainWindowIface.h>
#include <KexiIcon.h>

#include <kaction.h>
#include <kdebug.h>

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QStackedWidget>
#include <QDialogButtonBox>

#include <widget/navigator/KexiProjectNavigator.h>
#include <widget/navigator/KexiProjectModel.h>
#include <kexiutils/utils.h>
#include <kexi_global.h>

class ActionSelectorDialogTreeItem : public QTreeWidgetItem
{
public:
  enum ActionRole{
    ActionCategoryRole = Qt::UserRole + 1,
    ActionDataRole,
    ActionPartClassRole
  };
  
  ActionSelectorDialogTreeItem(QString label, QTreeWidget *parent)
  : QTreeWidgetItem(parent) {
      setText(0, label);
      
  }
  
  ActionSelectorDialogTreeItem(QString label, QTreeWidgetItem *parent)
          : QTreeWidgetItem(parent) {
              setText(0, label);
              
  }
  
  using QTreeWidgetItem::data;
  QVariant data(ActionRole role) {
      return QTreeWidgetItem::data(0, role);
  };
  
  using QTreeWidgetItem::setData;
  void setData(ActionRole role, QVariant value) {
      QTreeWidgetItem::setData(0, role, value);
  }
  
  QIcon icon() {
      return QTreeWidgetItem::icon(0);
  }
  
  void setIcon(const QIcon& icon) {
      QTreeWidgetItem::setIcon(0, icon);
  }
      
};

//---------------------------------------

ActionsListViewBase::ActionsListViewBase(QWidget* parent)
        : QTreeWidget(parent)
{
    setColumnCount(1);
    setHeaderHidden(true);
    setRootIsDecorated(false);
}

ActionsListViewBase::~ActionsListViewBase()
{
}

QTreeWidgetItem *ActionsListViewBase::itemForAction(const QString& actionName, QTreeWidgetItem* parent)
{   
    Q_UNUSED(parent);
    QTreeWidgetItemIterator it(this);
    while (*it) {
        ActionSelectorDialogTreeItem* itm = dynamic_cast<ActionSelectorDialogTreeItem*>(*it);
        if (itm && itm->data(ActionSelectorDialogTreeItem::ActionDataRole).toString() == actionName)
            return itm;
        ++it;
    }

    return 0;
}

void ActionsListViewBase::selectAction(const QString& actionName)
{
  //kDebug() << "Selecting action:" << actionName;
  QTreeWidgetItem *itm = itemForAction(actionName);
  if (itm) {
    setCurrentItem(itm);
    itm->setSelected(true);
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
        QString actionName;
        if (!action->toolTip().isEmpty()) {
            actionName = action->toolTip().remove("<html>").remove("</html>");
        } else {
            actionName = action->text().remove('&');
        }
        ActionSelectorDialogTreeItem *pitem = new ActionSelectorDialogTreeItem(
            actionName, this);

        pitem->setData(ActionSelectorDialogTreeItem::ActionCategoryRole, "kaction");
        pitem->setData(ActionSelectorDialogTreeItem::ActionDataRole, action->objectName());

        pitem->setIcon(action->icon());
        if (pitem->icon().isNull())
            pitem->setIcon(noIcon);
    }
    setSortingEnabled(true);
}

//---------------------------------------

//! @internal Used to display KActions (in column 2)
class KActionsListView : public KActionsListViewBase
{
public:
    explicit KActionsListView(QWidget* parent)
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
    explicit CurrentFormActionsListView(QWidget* parent)
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
    explicit ActionCategoriesListView(QWidget* parent)
            : ActionsListViewBase(parent) {
        ActionSelectorDialogTreeItem *itm = new ActionSelectorDialogTreeItem(xi18n("No action"), this );
        itm->setData(ActionSelectorDialogTreeItem::ActionCategoryRole, "noaction");
        itm->setData(ActionSelectorDialogTreeItem::ActionDataRole, "noaction");
        const QPixmap noIcon(KexiUtils::emptyIcon(KIconLoader::Small));
        itm->setIcon(noIcon);

        itm = new ActionSelectorDialogTreeItem(xi18n("Application actions"), this );
        itm->setData(ActionSelectorDialogTreeItem::ActionCategoryRole, "kaction");
        itm->setData(ActionSelectorDialogTreeItem::ActionDataRole, "kaction");
        itm->setIcon(koIcon("calligrakexi"));

        KexiPart::PartInfoList *pl = Kexi::partManager().infoList();
        if (pl) {
            foreach(KexiPart::Info *info, *pl) {
                KexiPart::Part *part = Kexi::partManager().part(info);
                if (!info->isVisibleInNavigator() || !part)
                    continue;
                itm = new ActionSelectorDialogTreeItem(part->info()->instanceCaption(), this );
                itm->setData(ActionSelectorDialogTreeItem::ActionCategoryRole, "navObject");
                itm->setData(ActionSelectorDialogTreeItem::ActionDataRole, info->objectName());
                itm->setData(ActionSelectorDialogTreeItem::ActionPartClassRole, info->partClass());
                itm->setIcon(QIcon::fromTheme(part->info()->itemIconName()));
            }
        }

        QTreeWidgetItem *fitm = itemForAction("form");
        if (fitm) {
            itm = new ActionSelectorDialogTreeItem(xi18nc("Current form's actions", "Current"), fitm);
        } else {
            itm = new ActionSelectorDialogTreeItem(xi18nc("Current form's actions", "Current"), this);
        }

        itm->setData(ActionSelectorDialogTreeItem::ActionCategoryRole, "currentForm");
        itm->setData(ActionSelectorDialogTreeItem::ActionDataRole, "currentForm");
        itm->setIcon(koIcon("form"));

        expandAll();
        setSortingEnabled(false);
    }

    ~ActionCategoriesListView() {
    }

};

//! @internal Used to display list of actions available to executing (column 3)
class ActionToExecuteListView : public ActionsListViewBase
{
public:
    explicit ActionToExecuteListView(QWidget* parent)
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
        ActionSelectorDialogTreeItem *itm;
        const QPixmap noIcon(KexiUtils::emptyIcon(KIconLoader::Small));
        if (supportedViewModes & Kexi::DataViewMode) {
            itm = new ActionSelectorDialogTreeItem(xi18n("Open in Data View"), this);
            itm->setData(ActionSelectorDialogTreeItem::ActionDataRole , "open");
            itm->setIcon(koIcon("document-open"));
        }
        if (part->info()->isExecuteSupported()) {
            itm = new ActionSelectorDialogTreeItem(xi18n("Execute"), this);
            itm->setData(ActionSelectorDialogTreeItem::ActionDataRole , "execute");
            itm->setIcon(koIcon("media-playback-start"));
        }
#ifndef KEXI_NO_QUICK_PRINTING
        if (part->info()->isPrintingSupported()) {
            ActionSelectorDialogListItem *printItem = new ActionSelectorDialogListItem(
                "print", this, futureI18n("Print"));
            printItem->setPixmap(0, koIcon("document-print"));
            KAction *a = KStandardAction::printPreview(0, 0, 0);
            item = new ActionSelectorDialogListItem("printPreview", printItem,
                                                    a->text().remove('&').remove("..."));
            item->setPixmap(0, a->icon().pixmap(16));
            delete a;
            item = new ActionSelectorDialogListItem(
                "pageSetup", printItem, futureI18n("Show Page Setup"));
            item->setPixmap(0, noIcon);
            setOpen(printItem, true);
            printItem->setExpandable(false);
        }
#endif
        if (part->info()->isDataExportSupported()) {
            itm = new ActionSelectorDialogTreeItem(
                xi18nc("Note: use multiple rows if needed", "Export to File\nAs Data Table"), this);
            itm->setData(ActionSelectorDialogTreeItem::ActionDataRole , "exportToCSV");
            itm->setIcon(koIcon("table"));

            QTreeWidgetItem *eitem = itm;
            
            itm = new ActionSelectorDialogTreeItem(xi18nc("Note: use multiple rows if needed", "Copy to Clipboard\nAs Data Table"), eitem);
            itm->setData(ActionSelectorDialogTreeItem::ActionDataRole , "copyToClipboardAsCSV");
            itm->setIcon(koIcon("table"));
        }
        
        itm = new ActionSelectorDialogTreeItem(xi18n("Create New Object"), this);
        itm->setData(ActionSelectorDialogTreeItem::ActionDataRole , "new");
        itm->setIcon(koIcon("document-new"));

        if (supportedViewModes & Kexi::DesignViewMode) {
            itm = new ActionSelectorDialogTreeItem(xi18n("Open in Design View"), this);
            itm->setData(ActionSelectorDialogTreeItem::ActionDataRole , "design");
            itm->setIcon(koIcon("document-properties"));
        }
        if (supportedViewModes & Kexi::TextViewMode) {
            itm = new ActionSelectorDialogTreeItem(xi18n("Open in Text View"), this);
            itm->setData(ActionSelectorDialogTreeItem::ActionDataRole , "editText");
            itm->setIcon(noIcon);
        }
        
        itm = new ActionSelectorDialogTreeItem( xi18n("Close View"), this);
        itm->setData(ActionSelectorDialogTreeItem::ActionDataRole , "close");
        itm->setIcon(koIcon("window-close"));

        expandAll();
        setSortingEnabled(true);
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
    }

    QString selectActionToBeExecutedMessage(const QString& actionType) const {
        QString msg;
        if (actionType == "noaction")
            return QString();
        if (actionType == "kaction" || actionType == "currentForm")
            return xi18n("&Select action to be executed after clicking <interface>%1</interface> button:",
                        actionWidgetName);
        // hardcoded, but it's not that bad
        if (actionType == "org.kexi-project.macro")
            return xi18n("&Select macro to be executed after clicking <interface>%1</interface> button:",
                        actionWidgetName);
        if (actionType == "org.kexi-project.script")
            return xi18n("&Select script to be executed after clicking <interface>%1</interface> button:",
                        actionWidgetName);
        //default: org.kexi-project.table/query/form/report...
        return xi18n("&Select object to be opened after clicking <interface>%1</interface> button:",
                    actionWidgetName);
    }

    // changes 3rd column visibility
    void setActionToExecuteSectionVisible(bool visible) {
        actionToExecuteListView->setVisible(visible);
        actionToExecuteLbl->setVisible(visible);
    }

    QString actionWidgetName;
    ActionCategoriesListView* actionCategoriesListView; //!< for column #1
    QWidget *kactionPageWidget;
    KActionsListView* kactionListView;  //!< for column #2
    KexiProjectNavigator* objectsListView; //!< for column #2
    QWidget *currentFormActionsPageWidget; //!< for column #2
    CurrentFormActionsListView* currentFormActionsListView; //!< for column #2
    QWidget *emptyWidget;
    QLabel *selectActionToBeExecutedLabel;
    QLabel *kactionPageLabel;
    QLabel *currentFormActionsPageLabel;
    ActionToExecuteListView* actionToExecuteListView;
    QLabel *actionToExecuteLbl;
    QWidget *secondAnd3rdColumnMainWidget;
    QGridLayout *glyr;
    QGridLayout *secondAnd3rdColumnGrLyr;
    QStackedWidget *secondAnd3rdColumnStack; //, *secondColumnStack;
    bool hideActionToExecuteListView;
};

//-------------------------------------

static QLabel *createSelectActionLabel(QWidget *parent, QWidget *buddy)
{
    QLabel *lbl = new QLabel(parent);
    lbl->setBuddy(buddy);
    lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    lbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    lbl->setWordWrap(true);
    lbl->setMinimumHeight(lbl->fontMetrics().height()*2);
    return lbl;
}

//-------------------------------------

KexiActionSelectionDialog::KexiActionSelectionDialog(
    QWidget *parent, const KexiFormEventAction::ActionData& action,
    const QString& actionWidgetName)
        : QDialog(parent)
        , d(new Private())
{
    setModal(true);
    setObjectName("actionSelectorDialog");
    setWindowTitle(xi18nc("@title:window", "Assigning Action to Button"));

    // layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    QWidget *mainWidget = new QWidget(this);
    mainWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    mainLayout->addWidget(mainWidget);

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
    d->glyr = new QGridLayout(mainWidget); // 2x2
    KexiUtils::setStandardMarginsAndSpacing(d->glyr);
    d->glyr->setRowStretch(1, 1);

    // 1st column: action types
    d->actionCategoriesListView = new ActionCategoriesListView(mainWidget);
    d->actionCategoriesListView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    d->glyr->addWidget(d->actionCategoriesListView, 1, 0);
    connect(d->actionCategoriesListView, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(slotActionCategorySelected(QTreeWidgetItem*)));

    QLabel *lbl = new QLabel(xi18n("Action category:"), mainWidget);
    lbl->setBuddy(d->actionCategoriesListView);
    lbl->setMinimumHeight(lbl->fontMetrics().height()*2);
    lbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    lbl->setWordWrap(true);

    d->glyr->addWidget(lbl, 0, 0, Qt::AlignTop | Qt::AlignLeft);

    // widget stack for 2nd and 3rd column
    d->secondAnd3rdColumnStack = new QStackedWidget(mainWidget);
    d->secondAnd3rdColumnStack->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    d->glyr->addWidget(d->secondAnd3rdColumnStack, 0, 1, 2, 1);

    d->secondAnd3rdColumnMainWidget = new QWidget(d->secondAnd3rdColumnStack);
    d->secondAnd3rdColumnMainWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    d->secondAnd3rdColumnGrLyr = new QGridLayout(d->secondAnd3rdColumnMainWidget);
    QDialog::resizeLayout(d->secondAnd3rdColumnGrLyr, 0, KexiUtils::spacingHint());
    d->secondAnd3rdColumnGrLyr->setRowStretch(1, 2);
    d->secondAnd3rdColumnStack->addWidget(d->secondAnd3rdColumnMainWidget);

    // 2nd column: list of actions/objects
    d->objectsListView = new KexiProjectNavigator(d->secondAnd3rdColumnMainWidget,
                                                  KexiProjectNavigator::Borders);
    d->secondAnd3rdColumnGrLyr->addWidget(d->objectsListView, 1, 0);
    d->secondAnd3rdColumnGrLyr->setColumnStretch(0,1);
    d->secondAnd3rdColumnGrLyr->setColumnStretch(1,1);
    connect(d->objectsListView, SIGNAL(selectionChanged(KexiPart::Item*)),
            this, SLOT(slotItemForOpeningOrExecutingSelected(KexiPart::Item*)));

    d->selectActionToBeExecutedLabel = createSelectActionLabel(d->secondAnd3rdColumnMainWidget, 0);
    d->secondAnd3rdColumnGrLyr->addWidget(
        d->selectActionToBeExecutedLabel, 0, 0, Qt::AlignTop | Qt::AlignLeft);

    d->emptyWidget = new QWidget(d->secondAnd3rdColumnStack);
    d->secondAnd3rdColumnStack->addWidget(d->emptyWidget);

    // 3rd column: actions to execute
    d->actionToExecuteListView = new ActionToExecuteListView(d->secondAnd3rdColumnMainWidget);
    d->actionToExecuteListView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    
    connect(d->actionToExecuteListView, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotActionToExecuteItemExecuted(QTreeWidgetItem*)));
    connect(d->actionToExecuteListView, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(slotActionToExecuteItemSelected(QTreeWidgetItem*)));
    
    d->secondAnd3rdColumnGrLyr->addWidget(d->actionToExecuteListView, 1, 1);

    d->actionToExecuteLbl = createSelectActionLabel(d->secondAnd3rdColumnMainWidget,
                                                    d->actionToExecuteListView);
    d->actionToExecuteLbl->setText(xi18n("Action to execute:"));
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
                slotItemForOpeningOrExecutingSelected(item);

                QString actionOption(action.option);
                if (actionOption.isEmpty()) {
                    actionOption = "open"; // for backward compatibility
                }
                d->actionToExecuteListView->selectAction(actionOption);
                d->objectsListView->setFocus();
            }
        }
    } else {//invalid assignment or 'noaction'
        d->actionCategoriesListView->selectAction("noaction");
        d->actionCategoriesListView->setFocus();
    }

    // buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    d->actionWidgetName = actionWidgetName;
    buttonBox->button(QDialogButtonBox::Ok)->setText(xi18nc("Assign action", "&Assign"));
    //buttonBox->button(QDialogButtonBox::Ok)->setIcon(koIconName("dialog-ok"));
    buttonBox->button(QDialogButtonBox::Ok)->setToolTip(xi18n("Assign action"));
    mainLayout->addWidget(buttonBox);
}

KexiActionSelectionDialog::~KexiActionSelectionDialog()
{
    delete d;
}

void KexiActionSelectionDialog::slotKActionItemExecuted(QTreeWidgetItem*)
{
    accept();
}

void KexiActionSelectionDialog::slotKActionItemSelected(QTreeWidgetItem*)
{
    d->setActionToExecuteSectionVisible(false);
    updateOKButtonStatus();
}

void KexiActionSelectionDialog::slotCurrentFormActionItemExecuted(QTreeWidgetItem*)
{
    accept();
}

void KexiActionSelectionDialog::slotCurrentFormActionItemSelected(QTreeWidgetItem*)
{
    d->setActionToExecuteSectionVisible(false);
    updateOKButtonStatus();
}

void KexiActionSelectionDialog::slotItemForOpeningOrExecutingSelected(KexiPart::Item* item)
{
    d->setActionToExecuteSectionVisible(item);
}

void KexiActionSelectionDialog::slotActionToExecuteItemExecuted(QTreeWidgetItem* item)
{
    if (!item)
        return;
    ActionSelectorDialogTreeItem *listItem = dynamic_cast<ActionSelectorDialogTreeItem*>(item);
    if (listItem && listItem->data(ActionSelectorDialogTreeItem::ActionDataRole).isValid())
        accept();
}

void KexiActionSelectionDialog::slotActionToExecuteItemSelected(QTreeWidgetItem*)
{
  //kDebug();
  updateOKButtonStatus();
}

void KexiActionSelectionDialog::slotActionCategorySelected(QTreeWidgetItem* item)
{
    ActionSelectorDialogTreeItem *categoryItm = dynamic_cast<ActionSelectorDialogTreeItem*>(item);
    // simple case: part-less item, e.g. kaction:
    if (categoryItm) {
        if (categoryItm->data(ActionSelectorDialogTreeItem::ActionCategoryRole).toString() == "kaction") {
            if (!d->kactionPageWidget) {
                //create lbl+list view with a vlayout
                d->kactionPageWidget = new QWidget();
                d->kactionPageWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
                QVBoxLayout *vlyr = new QVBoxLayout(d->kactionPageWidget);
                KexiUtils::setMargins(vlyr, 0);
                vlyr->setSpacing(KexiUtils::spacingHint());
                d->kactionListView = new KActionsListView(d->kactionPageWidget);
                d->kactionListView->init();
                d->kactionPageLabel  = createSelectActionLabel(d->kactionPageWidget, d->kactionListView);
                vlyr->addWidget(d->kactionPageLabel);
                vlyr->addWidget(d->kactionListView);
                d->secondAnd3rdColumnStack->addWidget(d->kactionPageWidget);
                connect(d->kactionListView, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
                        this, SLOT(slotKActionItemExecuted(QTreeWidgetItem*)));
                connect(d->kactionListView, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
                        this, SLOT(slotKActionItemSelected(QTreeWidgetItem*)));
            }
            d->kactionPageLabel->setText(
                d->selectActionToBeExecutedMessage(categoryItm->data(ActionSelectorDialogTreeItem::ActionDataRole).toString()));
            d->setActionToExecuteSectionVisible(false);
            d->raiseWidget(d->kactionPageWidget);
            slotKActionItemSelected(d->kactionListView->currentItem()); //to refresh column #3
        } else if (categoryItm->data(ActionSelectorDialogTreeItem::ActionCategoryRole).toString() == "currentForm") {
            if (!d->currentFormActionsPageWidget) {
                //create lbl+list view with a vlayout
                d->currentFormActionsPageWidget = new QWidget();
                d->currentFormActionsPageWidget->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
                QVBoxLayout *vlyr = new QVBoxLayout(d->currentFormActionsPageWidget);
                KexiUtils::setMargins(vlyr, 0);
                vlyr->setSpacing(KexiUtils::spacingHint());
                d->currentFormActionsListView = new CurrentFormActionsListView(
                    d->currentFormActionsPageWidget);
                d->currentFormActionsListView->init();
                d->currentFormActionsPageLabel = createSelectActionLabel(
                            d->currentFormActionsPageWidget, d->currentFormActionsListView);
                vlyr->addWidget(d->currentFormActionsPageLabel);
                vlyr->addWidget(d->currentFormActionsListView);
                d->secondAnd3rdColumnStack->addWidget(d->currentFormActionsPageWidget);
                connect(d->currentFormActionsListView, SIGNAL(itemActivated(QTreeWidgetItem*)),
                        this, SLOT(slotCurrentFormActionItemExecuted(QTreeWidgetItem*)));
                connect(d->currentFormActionsListView, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
                        this, SLOT(slotCurrentFormActionItemSelected(QTreeWidgetItem*)));
            }
            d->currentFormActionsPageLabel->setText(
                d->selectActionToBeExecutedMessage(categoryItm->data(ActionSelectorDialogTreeItem::ActionDataRole).toString()));
            d->setActionToExecuteSectionVisible(false);
            d->raiseWidget(d->currentFormActionsPageWidget);
            slotCurrentFormActionItemSelected(d->currentFormActionsListView->currentItem()); //to refresh column #3
        } else if (categoryItm->data(ActionSelectorDialogTreeItem::ActionCategoryRole).toString() == "noaction") {
            d->raiseWidget(d->emptyWidget);
            d->objectsListView->clearSelection();
            //hide column #3
            d->setActionToExecuteSectionVisible(false);
        } else if (categoryItm->data(ActionSelectorDialogTreeItem::ActionCategoryRole).toString() == "navObject") {
            QString partClass = categoryItm->data(ActionSelectorDialogTreeItem::ActionPartClassRole).toString();
            d->selectActionToBeExecutedLabel->setText(d->selectActionToBeExecutedMessage(partClass));
            if (d->objectsListView->itemsPartClass() != partClass) {
                QString errorString;
                d->objectsListView->setProject(KexiMainWindowIface::global()->project(), partClass, &errorString, false);
                d->actionToExecuteListView->showActionsForPartClass(partClass);
                d->setActionToExecuteSectionVisible(false);
            }
            if (d->secondAnd3rdColumnStack->currentWidget() != d->secondAnd3rdColumnMainWidget) {
                d->raiseWidget(d->secondAnd3rdColumnMainWidget);
                d->objectsListView->clearSelection();
                d->setActionToExecuteSectionVisible(false);
            } else {
                d->raiseWidget(d->secondAnd3rdColumnMainWidget);
            }
            d->selectActionToBeExecutedLabel->setBuddy(d->secondAnd3rdColumnMainWidget);
        }

        d->actionCategoriesListView->update();
        updateOKButtonStatus();
        return;
    }
    d->actionCategoriesListView->update();
    d->actionToExecuteListView->update();
    updateOKButtonStatus();
}

KexiFormEventAction::ActionData KexiActionSelectionDialog::currentAction() const
{
    KexiFormEventAction::ActionData data;
    ActionSelectorDialogTreeItem *categoryItm = dynamic_cast<ActionSelectorDialogTreeItem*>(d->actionCategoriesListView->currentItem());

    if (categoryItm) {
        QString actionCategory = categoryItm->data(ActionSelectorDialogTreeItem::ActionCategoryRole).toString();

        if (actionCategory == "kaction") {
            if (d->kactionListView->currentItem()) {
                data.string = QString("kaction:")
                + dynamic_cast<ActionSelectorDialogTreeItem*>(d->kactionListView->currentItem())->data(ActionSelectorDialogTreeItem::ActionDataRole).toString();
                return data;
            }
        } else if (actionCategory == "currentForm") {
            if (d->currentFormActionsListView->currentItem()) {
                data.string = QString("currentForm:")
                              + dynamic_cast<ActionSelectorDialogTreeItem*>(
                                  d->currentFormActionsListView->currentItem())->data(ActionSelectorDialogTreeItem::ActionDataRole).toString();
                return data;
            }
        } else if (actionCategory == "noaction") {
          return data;
        } else if (actionCategory == "navObject") {
            ActionSelectorDialogTreeItem *actionToExecute = dynamic_cast<ActionSelectorDialogTreeItem*>(d->actionToExecuteListView->currentItem());
            if (d->objectsListView && actionToExecute && !actionToExecute->data(ActionSelectorDialogTreeItem::ActionDataRole).toString().isEmpty()) {
                KexiPart::Item* partItem = d->objectsListView->selectedPartItem();
                KexiPart::Info* partInfo = partItem ? Kexi::partManager().infoForClass(partItem->partClass()) : 0;
                if (partInfo) {
                    // opening or executing: table:name, query:name, form:name, macro:name, script:name, etc.
                    data.string = QString("%1:%2").arg(partInfo->objectName()).arg(partItem->name());
                    data.option = actionToExecute->data(ActionSelectorDialogTreeItem::ActionDataRole).toString();
                    return data;
                }
            }
        } else {
            kWarning() << "No current category item";
        }
    }
    
    return data; // No Action
}

void KexiActionSelectionDialog::updateOKButtonStatus()
{
    ActionSelectorDialogTreeItem *itm = dynamic_cast<ActionSelectorDialogTreeItem*>(d->actionCategoriesListView->currentItem());

    //kDebug() << "Current Action:" << currentAction().string << ":" << currentAction().option;
    QPushButton *btn = button(QDialogButtonBox::Ok);
    btn->setEnabled((itm && itm->data(ActionSelectorDialogTreeItem::ActionCategoryRole).toString() == "noaction") || !currentAction().isEmpty());
}
