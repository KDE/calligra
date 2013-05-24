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

#include <QCheckBox>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>

#include <kdebug.h>
#include <klocale.h>
#include <kpushbutton.h>

#include <KoIcon.h>

#include "form.h"
#include "WidgetTreeWidget.h"

#include "tabstopdialog.h"

using namespace KFormDesigner;

//////////////////////////////////////////////////////////////////////////////////
//////////  The Tab Stop Dialog to edit tab order  ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

class TabStopDialog::Private
{
public:
    Private();
    ~Private();

    WidgetTreeWidget *widgetTree;
    KPushButton *btnUp, *btnDown;
    QCheckBox *check;
};

TabStopDialog::Private::Private()
{

}

TabStopDialog::Private::~Private()
{

}

TabStopDialog::TabStopDialog(QWidget *parent)
    : KDialog(parent), d(new Private())
{
    setObjectName("tabstop_dialog");
    setModal(true);
    setWindowTitle(i18n("Edit Tab Order"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);

    QFrame *frame = new QFrame(this);
    setMainWidget(frame);
    QGridLayout *l = new QGridLayout(frame);
    d->widgetTree = new WidgetTreeWidget(frame,
        WidgetTreeWidget::DisableSelection | WidgetTreeWidget::DisableContextMenu);
    d->widgetTree->setObjectName("tabstops:widgetTree");
    d->widgetTree->setDragEnabled(true);
    d->widgetTree->setDropIndicatorShown(true);
    d->widgetTree->setDragDropMode(QAbstractItemView::InternalMove);
    d->widgetTree->setAcceptDrops(true);
    l->addWidget(d->widgetTree, 0, 0);

    d->widgetTree->setForm(0);
    connect(d->widgetTree, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
#ifdef __GNUC__
#warning TODO connect(d->widgetTree, SIGNAL(moved(Q3ListViewItem*,Q3ListViewItem*,Q3ListViewItem*)), this, SLOT(updateButtons(Q3ListViewItem*)));
#else
#pragma WARNING( TODO connect(d->widgetTree, SIGNAL(moved(Q3ListViewItem*,Q3ListViewItem*,Q3ListViewItem*)), this, SLOT(updateButtons(Q3ListViewItem*))); )
#endif

    QVBoxLayout *vbox = new QVBoxLayout();
    l->addLayout(vbox, 0, 1);
    d->btnUp = new KPushButton(koIcon("arrow-up"), i18n("Move Up"), frame);
    d->btnUp->setToolTip(i18n("Move widget up"));
    vbox->addWidget(d->btnUp);
    connect(d->btnUp, SIGNAL(clicked()), this, SLOT(moveItemUp()));

    d->btnDown = new KPushButton(koIcon("arrow-down"), i18n("Move Down"), frame);
    d->btnDown->setToolTip(i18n("Move widget down"));
    vbox->addWidget(d->btnDown);
    connect(d->btnDown, SIGNAL(clicked()), this, SLOT(moveItemDown()));
    vbox->addStretch();

    d->check = new QCheckBox(i18n("Handle tab order automatically"), frame);
    d->check->setObjectName("tabstops_check");
    connect(d->check, SIGNAL(toggled(bool)), this, SLOT(slotRadioClicked(bool)));
    l->addWidget(d->check, 1, 0, 1, 2);

    updateGeometry();
    setInitialSize(QSize(500 + d->btnUp->width(), qMax(400, d->widgetTree->height())));
}

TabStopDialog::~TabStopDialog()
{
    delete d;
}

int TabStopDialog::exec(Form *form)
{
    d->widgetTree->clear();
    d->widgetTree->setForm(form);

    if (form->autoTabStops())
        form->autoAssignTabStops();
    form->updateTabStopsOrder();
    if (!form->tabStops()->isEmpty()) {
        ObjectTreeList::ConstIterator it(form->tabStops()->constBegin());
        it+=(form->tabStops()->count()-1);
        for (;it!=form->tabStops()->constEnd(); --it) {
            new WidgetTreeWidgetItem(d->widgetTree, *it);
        }
    }
    d->check->setChecked(form->autoTabStops());

    if (d->widgetTree->invisibleRootItem()->childCount() > 0) {
        QTreeWidgetItem *firstItem = d->widgetTree->invisibleRootItem()->child(0);
        d->widgetTree->setCurrentItem(firstItem);
        firstItem->setSelected(true);
    }

    if (QDialog::Rejected == KDialog::exec())
        return QDialog::Rejected;

    //accepted
    form->setAutoTabStops(d->check->isChecked());
    if (form->autoTabStops()) {
        form->autoAssignTabStops();
        return QDialog::Accepted;
    }

    //add items to the order list
    form->tabStops()->clear();
    QTreeWidgetItemIterator it(d->widgetTree);
    while (*it) {
        ObjectTreeItem *tree = static_cast<WidgetTreeWidgetItem*>(*it)->data();
        if (tree)
            form->tabStops()->append(tree);
    }
    return QDialog::Accepted;
}

void TabStopDialog::moveItemUp()
{
    QTreeWidgetItem *selected = d->widgetTree->selectedItem();
    if (!selected)
        return;
    // we assume there is flat list
    QTreeWidgetItem *root = d->widgetTree->invisibleRootItem();
    const int selectedIndex = root->indexOfChild(selected);
    if (selectedIndex < 1)
        return; // no place to move
    root->takeChild(selectedIndex);
    root->insertChild(selectedIndex - 1, selected);
    updateButtons(selected);
}

void TabStopDialog::moveItemDown()
{
    QTreeWidgetItem *selected = d->widgetTree->selectedItem();
    if (!selected)
        return;
    // we assume there is flat list
    QTreeWidgetItem *root = d->widgetTree->invisibleRootItem();
    const int selectedIndex = root->indexOfChild(selected);
    if (selectedIndex >= (root->childCount() - 1))
        return; // no place to move
    root->takeChild(selectedIndex);
    root->insertChild(selectedIndex + 1, selected);
    updateButtons(selected);
}

void TabStopDialog::updateButtons(QTreeWidgetItem *item)
{
    QTreeWidgetItem *root = d->widgetTree->invisibleRootItem();
    d->btnUp->setEnabled(item && (root->indexOfChild(item) > 0 && d->widgetTree->isEnabled()
                                 /*&& (item->itemAbove()->parent() == item->parent()))*/));
    d->btnDown->setEnabled(item && root->indexOfChild(item) < (root->childCount() - 1) && d->widgetTree->isEnabled());
}

void TabStopDialog::slotSelectionChanged()
{
    updateButtons(d->widgetTree->selectedItem());
}

void TabStopDialog::slotRadioClicked(bool isOn)
{
    d->widgetTree->setEnabled(!isOn);
    updateButtons(d->widgetTree->selectedItem());
}

bool TabStopDialog::autoTabStops() const
{
    return d->check->isChecked();
}

#include "tabstopdialog.moc"

