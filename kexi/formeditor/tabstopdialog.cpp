/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

#include <qcheckbox.h>
#include <qtooltip.h>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kpushbutton.h>

#include "form.h"
#include "objecttreeview.h"

#include "tabstopdialog.h"

using namespace KFormDesigner;

//////////////////////////////////////////////////////////////////////////////////
//////////  The Tab Stop Dialog to edit tab order  ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

TabStopDialog::TabStopDialog(QWidget *parent)
        : KDialog(parent)
{
    setObjectName("tabstop_dialog");
    setModal(true);
    setCaption(i18n("Edit Tab Order"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);

    QFrame *frame = new QFrame(this);
    setMainWidget(frame);
    QGridLayout *l = new QGridLayout(frame);
    m_treeview = new ObjectTreeView(frame, 
        ObjectTreeView::DisableSelection | ObjectTreeView::DisableContextMenu);
    m_treeview->setObjectName("tabstops_treeview");
    m_treeview->setItemsMovable(true);
    m_treeview->setDragEnabled(true);
    m_treeview->setDropVisualizer(true);
    m_treeview->setAcceptDrops(true);
    m_treeview->setFocus();
    l->addWidget(m_treeview, 0, 0);

    m_treeview->m_form = 0;
    connect(m_treeview, SIGNAL(currentChanged(Q3ListViewItem*)), this, SLOT(updateButtons(Q3ListViewItem*)));
    connect(m_treeview, SIGNAL(moved(Q3ListViewItem*, Q3ListViewItem*, Q3ListViewItem*)), this, SLOT(updateButtons(Q3ListViewItem*)));

    QVBoxLayout *vbox = new QVBoxLayout();
    l->addLayout(vbox, 0, 1);
    m_btnUp = new KPushButton(KIcon("arrow-up"), i18n("Move Up"), frame);
    m_btnUp->setToolTip(i18n("Move widget up"));
    vbox->addWidget(m_btnUp);
    connect(m_btnUp, SIGNAL(clicked()), this, SLOT(moveItemUp()));

    m_btnDown = new KPushButton(KIcon("arrow-down"), i18n("Move Down"), frame);
    m_btnDown->setToolTip(i18n("Move widget down"));
    vbox->addWidget(m_btnDown);
    connect(m_btnDown, SIGNAL(clicked()), this, SLOT(moveItemDown()));
    vbox->addStretch();

    m_check = new QCheckBox(i18n("Handle tab order automatically"), frame);
    m_check->setObjectName("tabstops_check");
    connect(m_check, SIGNAL(toggled(bool)), this, SLOT(slotRadioClicked(bool)));
    l->addWidget(m_check, 1, 0, 1, 2);

    updateGeometry();
    setInitialSize(QSize(500 + m_btnUp->width(), qMax(400, m_treeview->height())));
}

TabStopDialog::~TabStopDialog()
{
}

int TabStopDialog::exec(Form *form)
{
    m_treeview->clear();
    m_treeview->m_form = form;

    if (form->autoTabStops())
        form->autoAssignTabStops();
    form->updateTabStopsOrder();
    if (!form->tabStops()->isEmpty()) {
        ObjectTreeList::ConstIterator it(form->tabStops()->constBegin());
        it+=(form->tabStops()->count()-1);
        for (;it!=form->tabStops()->constEnd(); --it) {
            new ObjectTreeViewItem(m_treeview, *it);
        }
    }
    m_check->setChecked(form->autoTabStops());

    if (m_treeview->firstChild()) {
        m_treeview->setCurrentItem(m_treeview->firstChild());
        m_treeview->setSelected(m_treeview->firstChild(), true);
    }

    if (QDialog::Rejected == KDialog::exec())
        return QDialog::Rejected;

    //accepted
    form->setAutoTabStops(m_check->isChecked());
    if (form->autoTabStops()) {
        form->autoAssignTabStops();
        return QDialog::Accepted;
    }

    //add items to the order list
    form->tabStops()->clear();
    ObjectTreeViewItem *item = (ObjectTreeViewItem*)m_treeview->firstChild();
    while (item) {
        ObjectTreeItem *tree = item->objectTree();
        if (tree)
            form->tabStops()->append(tree);
        item = (ObjectTreeViewItem*)item->nextSibling();
    }
    return QDialog::Accepted;
}

void
TabStopDialog::moveItemUp()
{
    if (!m_treeview->selectedItem())
        return;
    Q3ListViewItem *before = m_treeview->selectedItem()->itemAbove();
    before->moveItem(m_treeview->selectedItem());
    updateButtons(m_treeview->selectedItem());
}

void
TabStopDialog::moveItemDown()
{
    Q3ListViewItem *item = m_treeview->selectedItem();
    if (!item)
        return;
    item->moveItem(item->nextSibling());
    updateButtons(item);
}

void
TabStopDialog::updateButtons(Q3ListViewItem *item)
{
    m_btnUp->setEnabled(item && (item->itemAbove() && m_treeview->isEnabled()
                                 /*&& (item->itemAbove()->parent() == item->parent()))*/));
    m_btnDown->setEnabled(item && item->nextSibling() && m_treeview->isEnabled());
}

void
TabStopDialog::slotRadioClicked(bool isOn)
{
    m_treeview->setEnabled(!isOn);
    updateButtons(m_treeview->selectedItem());
}

bool
TabStopDialog::autoTabStops() const
{
    return m_check->isChecked();
}

#include "tabstopdialog.moc"

