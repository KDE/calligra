/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005,2010 Jarosław Staniek <staniek@kde.org>

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
#include "WidgetTreeWidget.h"

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
    setWindowTitle(i18n("Edit Tab Order"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);

    QFrame *frame = new QFrame(this);
    setMainWidget(frame);
    QGridLayout *l = new QGridLayout(frame);
    m_widgetTree = new WidgetTreeWidget(frame,
        WidgetTreeWidget::DisableSelection | WidgetTreeWidget::DisableContextMenu);
    m_widgetTree->setObjectName("tabstops:widgetTree");
    m_widgetTree->setDragEnabled(true);
    m_widgetTree->setDropIndicatorShown(true);
    //2.0 m_widgetTree->setItemsMovable(true);
    //2.0 m_widgetTree->setDropVisualizer(true);
    m_widgetTree->setDragDropMode(QAbstractItemView::InternalMove);
    m_widgetTree->setAcceptDrops(true);
    //m_widgetTree->setFocus();
    l->addWidget(m_widgetTree, 0, 0);

    m_widgetTree->m_form = 0;
    connect(m_widgetTree, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
#warning TODO connect(m_widgetTree, SIGNAL(moved(Q3ListViewItem*, Q3ListViewItem*, Q3ListViewItem*)), this, SLOT(updateButtons(Q3ListViewItem*)));

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
    setInitialSize(QSize(500 + m_btnUp->width(), qMax(400, m_widgetTree->height())));
}

TabStopDialog::~TabStopDialog()
{
}

int TabStopDialog::exec(Form *form)
{
    m_widgetTree->clear();
    m_widgetTree->m_form = form;

    if (form->autoTabStops())
        form->autoAssignTabStops();
    form->updateTabStopsOrder();
    if (!form->tabStops()->isEmpty()) {
        ObjectTreeList::ConstIterator it(form->tabStops()->constBegin());
        it+=(form->tabStops()->count()-1);
        for (;it!=form->tabStops()->constEnd(); --it) {
            new WidgetTreeWidgetItem(m_widgetTree, *it);
        }
    }
    m_check->setChecked(form->autoTabStops());

    if (m_widgetTree->invisibleRootItem()->childCount() > 0) {
        QTreeWidgetItem *firstItem = m_widgetTree->invisibleRootItem()->child(0);
        m_widgetTree->setCurrentItem(firstItem);
        firstItem->setSelected(true);
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
    QTreeWidgetItemIterator it(m_widgetTree);
    while (*it) {
        ObjectTreeItem *tree = static_cast<WidgetTreeWidgetItem*>(*it)->data();
        if (tree)
            form->tabStops()->append(tree);
    }
    return QDialog::Accepted;
}

void TabStopDialog::moveItemUp()
{
    QTreeWidgetItem *selected = m_widgetTree->selectedItem();
    if (!selected)
        return;
    // we assume there is flat list
    QTreeWidgetItem *root = m_widgetTree->invisibleRootItem();
    const int selectedIndex = root->indexOfChild(selected);
    if (selectedIndex < 1)
        return; // no place to move
    root->takeChild(selectedIndex);
    root->insertChild(selectedIndex - 1, selected);
    updateButtons(selected);
}

void TabStopDialog::moveItemDown()
{
    QTreeWidgetItem *selected = m_widgetTree->selectedItem();
    if (!selected)
        return;
    // we assume there is flat list
    QTreeWidgetItem *root = m_widgetTree->invisibleRootItem();
    const int selectedIndex = root->indexOfChild(selected);
    if (selectedIndex >= (root->childCount() - 1))
        return; // no place to move
    root->takeChild(selectedIndex);
    root->insertChild(selectedIndex + 1, selected);
    updateButtons(selected);
}

void TabStopDialog::updateButtons(QTreeWidgetItem *item)
{
    QTreeWidgetItem *root = m_widgetTree->invisibleRootItem();
    m_btnUp->setEnabled(item && (root->indexOfChild(item) > 0 && m_widgetTree->isEnabled()
                                 /*&& (item->itemAbove()->parent() == item->parent()))*/));
    m_btnDown->setEnabled(item && root->indexOfChild(item) < (root->childCount() - 1) && m_widgetTree->isEnabled());
}

void TabStopDialog::slotSelectionChanged()
{
    updateButtons(m_widgetTree->selectedItem());
}

void TabStopDialog::slotRadioClicked(bool isOn)
{
    m_widgetTree->setEnabled(!isOn);
    updateButtons(m_widgetTree->selectedItem());
}

bool TabStopDialog::autoTabStops() const
{
    return m_check->isChecked();
}

#include "tabstopdialog.moc"

