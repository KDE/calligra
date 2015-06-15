/* This file is part of the KDE project
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KexiProjectSelectorWidget.h"
#include <core/kexi.h>
#include <KexiIcon.h>

#include <KDbDriverManager>
#include <KDbDriverMetaData>
#include <KDbConnectionData>
#include <KDbUtils>

#include <KLocalizedString>
#include <KGuiItem>

#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>

#include <assert.h>

//! @internal
class KexiProjectSelectorWidget::Private
{
public:
    explicit Private(KexiProjectSet *prj_set_)
         : prj_set(prj_set_)
    {
        selectable = true;
    }

    QIcon fileicon, dbicon;
    bool showProjectNameColumn;
    bool showConnectionColumns;
    bool selectable;

    KexiProjectSet *prj_set;
};

class KexiProjectSelectorDialog::Private
{
public:
  Private() {}
  KexiProjectSelectorWidget* sel;
};

/*================================================================*/

//! helper class
class ProjectDataLVItem : public QTreeWidgetItem
{
public:
    ProjectDataLVItem(KexiProjectData *d,
                      const KDbDriverMetaData &driverMetaData,
                      KexiProjectSelectorWidget *selector)
            : QTreeWidgetItem(selector->list())
            , data(d)
    {
        int colnum = 0;
        const KDbConnectionData cdata = *data->connectionData();
        if (selector->d->showProjectNameColumn)
            setText(colnum++, data->caption() + "  ");

        setText(colnum++, data->databaseName() + "  ");

        if (selector->d->showConnectionColumns) {
            QString driverName = driverMetaData.name();
            if (driverName.isEmpty()) {
                KDbDriverManager manager;
                const KDbDriverMetaData *driverMetaDataFromCdata = manager.driverMetaData(cdata.driverId());
                if (driverMetaDataFromCdata) {
                    driverName = driverMetaDataFromCdata->name();
                }
            }
            if (driverMetaData.isFileBased()) {
                setText(colnum++, xi18n("File (%1)", driverName) + "  ");
            } else {
                setText(colnum++, driverName + "  ");
            }
            QString conn;
            if (cdata.caption().isEmpty()) {
                conn = cdata.toUserVisibleString();
            }
            else {
                conn = xi18nc("caption: server_info", "%1: %2",
                              cdata.caption(), cdata.toUserVisibleString());
            }
            setText(colnum++, conn + "  ");
        }
    }
    ~ProjectDataLVItem() {}

    KexiProjectData *data;
};

/*================================================================*/

/*!
 *  Constructs a KexiProjectSelector which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
KexiProjectSelectorWidget::KexiProjectSelectorWidget(QWidget* parent,
        KexiProjectSet* prj_set, bool showProjectNameColumn, bool showConnectionColumns)
        : QWidget(parent)
        , d(new Private(prj_set))
{
    setupUi(this);
    setObjectName("KexiProjectSelectorWidget");
    d->showProjectNameColumn = showProjectNameColumn;
    d->showConnectionColumns = showConnectionColumns;
    list()->installEventFilter(this);

    d->fileicon = Kexi::defaultFileBasedDriverIcon();
    setWindowIcon(d->fileicon);
    d->dbicon = koIcon("server-database");

    QTreeWidgetItem *headerItem = list()->headerItem();
    QTreeWidgetItem *newHeaderItem = new QTreeWidgetItem;
    int newHeaderItemIndex = 0;
    if (d->showProjectNameColumn) {
        newHeaderItem->setText(newHeaderItemIndex++, headerItem->text(0)); // project name
    }
    newHeaderItem->setText(newHeaderItemIndex++, headerItem->text(1)); // database
    if (d->showConnectionColumns) {
        newHeaderItem->setText(newHeaderItemIndex++, headerItem->text(2)); // type
        newHeaderItem->setText(newHeaderItemIndex++, headerItem->text(3)); // connection
    }
    list()->setHeaderItem(newHeaderItem);
    setFocusProxy(list());

    //show projects
    setProjectSet(d->prj_set);
    connect(list(), SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotItemExecuted(QTreeWidgetItem*)));
    connect(list(), SIGNAL(itemSelectionChanged()),
            this, SLOT(slotItemSelected()));
}

/*!
 *  Destroys the object and frees any allocated resources
 */
KexiProjectSelectorWidget::~KexiProjectSelectorWidget()
{
    delete d;
}

KexiProjectData* KexiProjectSelectorWidget::selectedProjectData() const
{
    QList<QTreeWidgetItem *> items = list()->selectedItems();
    if (items.isEmpty())
        return 0;
    ProjectDataLVItem *item = static_cast<ProjectDataLVItem*>(items.first());
    if (item)
        return item->data;
    return 0;
}

void KexiProjectSelectorWidget::slotItemExecuted(QTreeWidgetItem *item)
{
    if (!d->selectable)
        return;
    ProjectDataLVItem *ditem = static_cast<ProjectDataLVItem*>(item);
    if (ditem)
        emit projectExecuted(ditem->data);
}

void KexiProjectSelectorWidget::slotItemExecuted()
{
    qDebug();
    if (!d->selectable)
        return;
    QList<QTreeWidgetItem *> items = list()->selectedItems();
    if (items.isEmpty())
        return;
    slotItemExecuted(items.first());
}

void KexiProjectSelectorWidget::slotItemSelected()
{
    if (!d->selectable)
        return;
    QList<QTreeWidgetItem *> items = list()->selectedItems();
    ProjectDataLVItem *ditem = static_cast<ProjectDataLVItem*>(items.isEmpty() ? 0 : items.first());
    emit selectionChanged(ditem ? ditem->data : 0);
}

void KexiProjectSelectorWidget::setProjectSet(KexiProjectSet* prj_set)
{
    if (prj_set) {
        //old list
        list()->clear();
    }
    d->prj_set = prj_set;
    if (!d->prj_set)
        return;
//! @todo what with project set's ownership?
    if (d->prj_set->result().isError()) {
        qDebug() << "d->prj_set->error()" << d->prj_set->result();
        return;
    }
    KDbDriverManager manager;
    KexiProjectData::List prjlist = d->prj_set->list();
    foreach(KexiProjectData* data, prjlist) {
        const KDbDriverMetaData *driverMetaData = manager.driverMetaData(data->connectionData()->driverId());
        if (driverMetaData) {
            ProjectDataLVItem *item = new ProjectDataLVItem(data, *driverMetaData, this);
            if (!d->selectable) {
                Qt::ItemFlags flags = item->flags();
                (flags |= Qt::ItemIsSelectable) ^= Qt::ItemIsSelectable;
                item->setFlags(flags);
            }
            if (driverMetaData->isFileBased())
                item->setIcon(0, d->fileicon);
            else
                item->setIcon(0, d->dbicon);
        }
        else {
            qWarning() << "no driver found for" << data->connectionData()->driverId();
        }
    }
    list()->setSortingEnabled(true);
    list()->sortItems(0, Qt::AscendingOrder);
    list()->resizeColumnToContents(0);
    if (list()->topLevelItemCount() > 0) {
        list()->topLevelItem(0)->setSelected(true);
    }
}

KexiProjectSet *KexiProjectSelectorWidget::projectSet() {
    return d->prj_set;
}

void KexiProjectSelectorWidget::setSelectable(bool set)
{
    if (d->selectable == set)
        return;
    d->selectable = set;
    //update items' state
    QTreeWidgetItemIterator it(list());
    while (*it) {
        Qt::ItemFlags flags = (*it)->flags();
        flags |= Qt::ItemIsSelectable;
        if (!d->selectable)
            flags ^= Qt::ItemIsSelectable;
        (*it)->setFlags(flags);
    }
}

bool KexiProjectSelectorWidget::isSelectable() const
{
    return d->selectable;
}

QLabel *KexiProjectSelectorWidget::label() const
{
    return Ui_KexiProjectSelector::label;
}

QTreeWidget* KexiProjectSelectorWidget::list() const
{
    return Ui_KexiProjectSelector::list;
}

bool KexiProjectSelectorWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if ((ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return)
            && ke->modifiers() == Qt::NoModifier)
        {
            slotItemExecuted();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

/*================================================================*/

KexiProjectSelectorDialog::KexiProjectSelectorDialog(QWidget *parent,
        const KDbConnectionData& cdata,
        bool showProjectNameColumn, bool showConnectionColumns)
        : KPageDialog(parent)
        , d(new Private)
{
    setWindowTitle(xi18nc("@title:window", "Open Project"));
    KDbConnectionData _cdata(cdata);
    KexiProjectSet *prj_set = new KexiProjectSet(&_cdata);
    init(prj_set, showProjectNameColumn, showConnectionColumns);
    KGuiItem okItem(xi18n("&Open"), koIconName("document-open"),
                    xi18n("Open Database Connection"));
    KGuiItem::assign(button(QDialogButtonBox::Ok), okItem);

    d->sel->label()->setText(xi18n("Select a project on <resource>%1</resource> database server to open:",
                                   _cdata.toUserVisibleString(KDbConnectionData::NoUserVisibleStringOption)));
}

KexiProjectSelectorDialog::~KexiProjectSelectorDialog()
{
    delete d;
}

void KexiProjectSelectorDialog::init(KexiProjectSet* prj_set, bool showProjectNameColumn,
                                     bool showConnectionColumns)
{
    setObjectName("KexiProjectSelectorDialog");
    setModal(true);
    setStandardButtons(
#ifdef KEXI_SHOW_UNFINISHED
        //! @todo re-add Help when doc is available
        QDialogButtonBox::Help |
#endif
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel
    );
    button(QDialogButtonBox::Ok)->setDefault(true);
    setFaceType(Plain);
    setSizeGripEnabled(true);

    d->sel = new KexiProjectSelectorWidget(this, prj_set,
                                          showProjectNameColumn, showConnectionColumns);
    //! @todo KEXI3 test this
    addPage(d->sel, QString());
    setWindowIcon(d->sel->windowIcon());
    d->sel->setFocus();

    connect(d->sel, SIGNAL(projectExecuted(KexiProjectData*)),
            this, SLOT(slotProjectExecuted(KexiProjectData*)));
    connect(d->sel, SIGNAL(selectionChanged(KexiProjectData*)),
            this, SLOT(slotProjectSelectionChanged(KexiProjectData*)));
}

KexiProjectData* KexiProjectSelectorDialog::selectedProjectData() const
{
    return d->sel->selectedProjectData();
}

void KexiProjectSelectorDialog::slotProjectExecuted(KexiProjectData*)
{
    accept();
}

void KexiProjectSelectorDialog::slotProjectSelectionChanged(KexiProjectData* pdata)
{
    button(QDialogButtonBox::Ok)->setEnabled(pdata);
}

void KexiProjectSelectorDialog::showEvent(QShowEvent * event)
{
    KPageDialog::showEvent(event);
    //! @todo KPageDialog::centerOnScreen(this);
}

KexiProjectSet* KexiProjectSelectorDialog::projectSet() const
{
    return d->sel->projectSet();
}
