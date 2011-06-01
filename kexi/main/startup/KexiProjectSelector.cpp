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

#include "KexiProjectSelector.h"

#include <kexidb/drivermanager.h>
#include <kexidb/connectiondata.h>
#include <kexidb/utils.h>
#include <core/kexi.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <klocale.h>
#include <kdebug.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <QPixmap>
#include <QKeyEvent>

#include <assert.h>

//! @internal
class KexiProjectSelectorWidget::Private
{
public:
    Private() {
        selectable = true;
    }
    QPixmap fileicon, dbicon;
    bool showProjectNameColumn;
    bool showConnectionColumns;
    bool selectable;
};

/*================================================================*/

//! helper class
class ProjectDataLVItem : public QTreeWidgetItem
{
public:
    ProjectDataLVItem(KexiProjectData *d,
                      const KexiDB::Driver::Info& info, KexiProjectSelectorWidget *selector)
            : QTreeWidgetItem(selector->list())
            , data(d)
    {
        int colnum = 0;
        const KexiDB::ConnectionData *cdata = data->constConnectionData();
        if (selector->d->showProjectNameColumn)
            setText(colnum++, data->caption() + "  ");

        setText(colnum++, data->databaseName() + "  ");

        if (selector->d->showConnectionColumns) {
            QString drvname = info.caption.isEmpty() ? cdata->driverName : info.caption;
            if (info.fileBased) {
                setText(colnum++, i18n("File") + " (" + drvname + ")  ");
            } else {
                setText(colnum++, drvname + "  ");
            }

            QString conn;
            if (!cdata->caption.isEmpty())
                conn = cdata->caption + ": ";
            conn += cdata->serverInfoString();
            setText(3, conn + "  ");
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
        , m_prj_set(prj_set)
        , d(new Private())
{
    setupUi(this);
    setObjectName("KexiProjectSelectorWidget");
    d->showProjectNameColumn = showProjectNameColumn;
    d->showConnectionColumns = showConnectionColumns;
    list()->installEventFilter(this);

    const QString iconname(KexiDB::defaultFileBasedDriverIcon());
    d->fileicon = KIconLoader::global()->loadMimeTypeIcon(iconname, KIconLoader::Desktop);
    setWindowIcon(KIcon(iconname));
    d->dbicon = SmallIcon("server-database");
// list->setHScrollBarMode( QScrollView::AlwaysOn );

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
    setProjectSet(m_prj_set);
    connect(list(), SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotItemExecuted(QTreeWidgetItem*)));
    //connect(list(), SIGNAL(returnPressed(QTreeWidgetItem*)),
    //        this, SLOT(slotItemExecuted(QTreeWidgetItem*)));
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
    kDebug();
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
    m_prj_set = prj_set;
    if (!m_prj_set)
        return;
//TODO: what with project set's ownership?
    if (m_prj_set->error()) {
        kDebug() << "KexiProjectSelectorWidget::setProjectSet() : m_prj_set->error() !";
        return;
    }
    KexiDB::DriverManager manager;
    KexiProjectData::List prjlist = m_prj_set->list();
    foreach(KexiProjectData* data, prjlist) {
        KexiDB::Driver::Info info = manager.driverInfo(data->constConnectionData()->driverName);
        if (!info.name.isEmpty()) {
            ProjectDataLVItem *item = new ProjectDataLVItem(data, info, this);
            if (!d->selectable) {
                Qt::ItemFlags flags = item->flags();
                (flags |= Qt::ItemIsSelectable) ^= Qt::ItemIsSelectable;
                item->setFlags(flags);
            }
            if (info.fileBased)
                item->setIcon(0, d->fileicon);
            else
                item->setIcon(0, d->dbicon);
        }
        else {
            kWarning() << "KexiProjectSelector::KexiProjectSelector(): no driver found for '"
            << data->constConnectionData()->driverName << "'!";
        }
    }
    list()->setSortingEnabled(true);
    list()->sortItems(0, Qt::AscendingOrder);
    list()->resizeColumnToContents(0);
    if (list()->topLevelItemCount() > 0) {
        list()->topLevelItem(0)->setSelected(true);
    }
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

// void KexiProjectSelectorWidget::keyPressEvent(QKeyEvent* event)
// {
//     if (event->key() == Qt::Key_Enter && event->modifiers() == Qt::NoModifier) {
//         event->accept();
//         slotItemExecuted();
//         return;
//     }
//     QWidget::keyPressEvent(event);
// }

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
        KexiProjectSet* prj_set, bool showProjectNameColumn, bool showConnectionColumns)
        : KPageDialog(parent)
{
    setWindowTitle(i18n("Open Recent Project"));
    init(prj_set, showProjectNameColumn, showConnectionColumns);
}

KexiProjectSelectorDialog::KexiProjectSelectorDialog(QWidget *parent,
        const KexiDB::ConnectionData& cdata,
        bool showProjectNameColumn, bool showConnectionColumns)
        : KPageDialog(parent)
{
    setWindowTitle(i18n("Open Project"));
    KexiDB::ConnectionData _cdata(cdata);
    KexiProjectSet *prj_set = new KexiProjectSet(_cdata);
    init(prj_set, showProjectNameColumn, showConnectionColumns);
    setButtonGuiItem(Ok, KGuiItem(i18n("&Open"), "document-open",
                                  i18n("Open Database Connection")));

    m_sel->label()->setText(i18n("Select a project on <b>%1</b> database server to open:",
                                 _cdata.serverInfoString(false)));
}

KexiProjectSelectorDialog::~KexiProjectSelectorDialog()
{
}

void KexiProjectSelectorDialog::init(KexiProjectSet* prj_set, bool showProjectNameColumn,
                                     bool showConnectionColumns)
{
    setObjectName("KexiProjectSelectorDialog");
    setModal(true);
    setButtons(
#ifndef KEXI_NO_UNFINISHED
        //! @todo re-add Help when doc is available
        Help |
#endif
        Ok | Cancel
    );
    setDefaultButton(Ok);
    setFaceType(Plain);
    setSizeGripEnabled(true);

    m_sel = new KexiProjectSelectorWidget(this, prj_set,
                                          showProjectNameColumn, showConnectionColumns);
    setMainWidget(m_sel);
    setWindowIcon(m_sel->windowIcon());
    m_sel->setFocus();

    connect(m_sel, SIGNAL(projectExecuted(KexiProjectData*)),
            this, SLOT(slotProjectExecuted(KexiProjectData*)));
    connect(m_sel, SIGNAL(selectionChanged(KexiProjectData*)),
            this, SLOT(slotProjectSelectionChanged(KexiProjectData*)));
}

KexiProjectData* KexiProjectSelectorDialog::selectedProjectData() const
{
    return m_sel->selectedProjectData();
}

void KexiProjectSelectorDialog::slotProjectExecuted(KexiProjectData*)
{
    accept();
}

void KexiProjectSelectorDialog::slotProjectSelectionChanged(KexiProjectData* pdata)
{
    enableButtonOk(pdata);
}

void KexiProjectSelectorDialog::showEvent(QShowEvent * event)
{
    KPageDialog::showEvent(event);
    KPageDialog::centerOnScreen(this);
}

#include "KexiProjectSelector.moc"
