/* This file is part of the KDE project
   Copyright (C) 2003 Jarosław Staniek <staniek@kde.org>

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
#include <q3listview.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QPixmap>

#include <assert.h>

//! @internal
class KexiProjectSelectorWidget::Private
{
public:
    Private() {
        selectable = true;
    }
    QPixmap fileicon, dbicon;
    bool showProjectNameColumn : 1;
    bool showConnectionColumns : 1;
    bool selectable : 1;
};

/*================================================================*/

//! helper class
class ProjectDataLVItem : public Q3ListViewItem
{
public:
    ProjectDataLVItem(KexiProjectData *d,
                      const KexiDB::Driver::Info& info, KexiProjectSelectorWidget *selector)
            : Q3ListViewItem(selector->list)
            , data(d) {
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
    const QString iconname(KexiDB::defaultFileBasedDriverIcon());
    d->fileicon = KIconLoader::global()->loadMimeTypeIcon(iconname, KIconLoader::Desktop);
    setWindowIcon(KIcon(iconname));
    d->dbicon = SmallIcon("server-database");
// list->setHScrollBarMode( QScrollView::AlwaysOn );

    if (!d->showConnectionColumns) {
        list->removeColumn(2);
        list->removeColumn(2);
    }
    if (!d->showProjectNameColumn) {
        list->removeColumn(0);
    }
    setFocusProxy(list);

    //show projects
    setProjectSet(m_prj_set);
    connect(list, SIGNAL(doubleClicked(Q3ListViewItem*)),
            this, SLOT(slotItemExecuted(Q3ListViewItem*)));
    connect(list, SIGNAL(returnPressed(Q3ListViewItem*)),
            this, SLOT(slotItemExecuted(Q3ListViewItem*)));
    connect(list, SIGNAL(selectionChanged()),
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
    ProjectDataLVItem *item = static_cast<ProjectDataLVItem*>(list->selectedItem());
    if (item)
        return item->data;
    return 0;
}

void KexiProjectSelectorWidget::slotItemExecuted(Q3ListViewItem *item)
{
    if (!d->selectable)
        return;
    ProjectDataLVItem *ditem = static_cast<ProjectDataLVItem*>(item);
    if (ditem)
        emit projectExecuted(ditem->data);
}

void KexiProjectSelectorWidget::slotItemSelected()
{
    if (!d->selectable)
        return;
    ProjectDataLVItem *ditem = static_cast<ProjectDataLVItem*>(list->selectedItem());
    emit selectionChanged(ditem ? ditem->data : 0);
}

void KexiProjectSelectorWidget::setProjectSet(KexiProjectSet* prj_set)
{
    if (prj_set) {
        //old list
        list->clear();
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
            if (!d->selectable)
                item->setSelectable(false);
            if (info.fileBased)
                item->setPixmap(0, d->fileicon);
            else
                item->setPixmap(0, d->dbicon);
        } else {
            kWarning() << "KexiProjectSelector::KexiProjectSelector(): no driver found for '"
            << data->constConnectionData()->driverName << "'!";
        }
    }
    if (list->firstChild()) {
        list->setSelected(list->firstChild(), true);
    }
}

void KexiProjectSelectorWidget::setSelectable(bool set)
{
    if (d->selectable == set)
        return;
    d->selectable = set;
    //update items' state
    Q3ListViewItemIterator it(list);
    while (it.current()) {
        it.current()->setSelectable(d->selectable);
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

/*================================================================*/

KexiProjectSelectorDialog::KexiProjectSelectorDialog(QWidget *parent,
        KexiProjectSet* prj_set, bool showProjectNameColumn, bool showConnectionColumns)
        : KPageDialog(parent)
{
    setCaption(i18n("Open Recent Project"));
    init(prj_set, showProjectNameColumn, showConnectionColumns);
}

KexiProjectSelectorDialog::KexiProjectSelectorDialog(QWidget *parent,
        const KexiDB::ConnectionData& cdata,
        bool showProjectNameColumn, bool showConnectionColumns)
        : KPageDialog(parent)
{
    setCaption(i18n("Open Project"));
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
