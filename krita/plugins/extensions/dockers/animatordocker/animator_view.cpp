/*
 *  Main widget for animator plugin
 *  Copyright (C) 2011 Torio Mlshi <mlshi@lavabit.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <QAction>

#include <KLocale>

#include "kis_node_model.h"

#include "animator_view.h"
#include "animator_model.h"

#include <iostream>
// #include <QMenu>
#include <KMenu>
#include <KDialog>
#include <KLineEdit>

AnimatorView::AnimatorView()
{
//     setModel();
//     connect(this, SIGNAL(activated(QModelIndex)), amodel(), SLOT(activateLayer(QModelIndex)));
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(slotCustomContextMenuRequested(QPoint)));
    
    // Create menu
    m_itemContextMenu = new KMenu(this);
    
//     QAction* action_new = new QAction(i18n("Create"), this);
//     connect(action_new, SIGNAL(triggered(bool)), this, SLOT(createFrame()));
    
//     QAction* action_new_paint = 
    addAction(m_itemContextMenu->addAction(KIcon("document-new"), i18n("&Paint frame"), this, SLOT(slotNewPaint())));
//     addAction(itemContextMenu->addAction(KIcon("view-filter"), i18n("&Generated frame"), this, SLOT(slotNewGenerated())));
//     addAction(itemContextMenu->addAction(KIcon("view-filter"), i18n("&Filter frame"), this, SLOT(slotNewFilter())));
    addAction(m_itemContextMenu->addAction(KIcon("bookmark-new"), i18n("&Shape frame"), this, SLOT(slotNewShape())));
//     addAction(itemContextMenu->addAction(KIcon("edit-copy"), i18n("&Clone frame"), this, SLOT(slotNewClone())));
    addAction(m_itemContextMenu->addAction(KIcon("folder-new"), i18n("&Group frame"), this, SLOT(slotNewGroup())));
    
    m_itemContextMenu->addSeparator();
    
    addAction(m_itemContextMenu->addAction(KIcon("edit-copy"), i18n("Copy p&revious frame"), this, SLOT(slotCopyPrevious())));
    addAction(m_itemContextMenu->addAction(KIcon("edit-copy"), i18n("Copy &next frame"), this, SLOT(slotCopyNext())));
    
    m_itemContextMenu->addSeparator();

    addAction(m_itemContextMenu->addAction(KIcon("tools-wizard"), i18n("Interpolate"), this, SLOT(slotInterpolate())));

    m_itemContextMenu->addSeparator();
    
    addAction(m_itemContextMenu->addAction(KIcon("edit-delete"), i18n("Clear selected frames"), this, SLOT(slotClearFrames())));
    
    m_itemContextMenu->addSeparator();
    
    addAction(m_itemContextMenu->addAction(KIcon("edit-rename"), i18n("Rename layer"), this, SLOT(slotRenameLayer())));
    
//     connect(action_new_paint, SIGNAL(triggered(bool)), this, SLOT(createFrame()));
}

AnimatorView::~AnimatorView()
{
}

void AnimatorView::setModel(QAbstractItemModel* model)
{
    connect(this, SIGNAL(activated(QModelIndex)), model, SLOT(activateLayer(QModelIndex)));
    resizeColumnsToContents();
    connect(model, SIGNAL(framesNumberChanged(int)), this, SLOT(resizeColumnsToContents()));
    connect(model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)), this, SLOT(resizeColumnsToContents()));
    QTableView::setModel(model);
}

void AnimatorView::slotCustomContextMenuRequested(const QPoint& pos)
{
    QModelIndex index = indexAt(pos);
    
//     std::cout << "CONTEXT MENU REQUESTED" << std::endl;
    
    m_menuIndex = index;
    
    m_itemContextMenu->popup( mapToGlobal(pos) );
}

AnimatorModel* AnimatorView::amodel()
{
    return dynamic_cast<AnimatorModel*>(model());
}


void AnimatorView::slotNewPaint()
{
    amodel()->createFrame(m_menuIndex, "KisPaintLayer");
}

void AnimatorView::slotNewClone()
{
    // ??
    amodel()->createFrame(m_menuIndex, "KisCloneLayer");
}

void AnimatorView::slotNewFilter()
{
    amodel()->createFrame(m_menuIndex, "KisFilterLayer");
}

void AnimatorView::slotNewGenerated()
{
    amodel()->createFrame(m_menuIndex, "KisGeneratedLayer");
}

void AnimatorView::slotNewShape()
{
    amodel()->createFrame(m_menuIndex, "KisShapeLayer");
}

void AnimatorView::slotNewGroup()
{
    amodel()->createFrame(m_menuIndex, "KisGroupLayer");
}

void AnimatorView::slotCopyPrevious()
{
    amodel()->copyFramePrevious(m_menuIndex);
}

void AnimatorView::slotCopyNext()
{
    amodel()->copyFrameNext(m_menuIndex);
}

void AnimatorView::slotInterpolate()
{
    amodel()->clonePrevious(m_menuIndex);
}

void AnimatorView::slotClearFrames()
{
    if (selectionModel()->selection().isEmpty())
    {
        amodel()->clearFrame(m_menuIndex);
    } else
    {
        amodel()->clearFrames(selectionModel()->selectedIndexes());
    }
}

void AnimatorView::slotRenameLayer()
{
    renameLayer(m_menuIndex);
}

void AnimatorView::renameLayer(QModelIndex& index)
{
//     QString* name;
    m_rename_index = index;

    KDialog* renameDialog = new KDialog(this);
    renameDialog->setModal(true);
    renameDialog->setAttribute(Qt::WA_DeleteOnClose);
    renameDialog->setButtons(KDialog::Ok | KDialog::Cancel);
    renameDialog->setCaption(i18n("Rename layer"));
    
    QWidget* main_widget = new QWidget(renameDialog);
    QVBoxLayout* layout = new QVBoxLayout(main_widget);
    QLabel* label = new QLabel(i18n("Rename layer"), main_widget);
    KLineEdit* ledit = new KLineEdit(main_widget);
    ledit->setClearButtonShown(true);
    
    setRenameName(amodel()->getLayerName(index));
    ledit->setText(amodel()->getLayerName(index));
    ledit->setFocus();
    
    layout->addWidget(label);
    layout->addWidget(ledit);
    
    connect(ledit, SIGNAL(textChanged(QString)), this, SLOT(setRenameName(QString)));
    
    renameDialog->setMainWidget(main_widget);
    
    connect(renameDialog, SIGNAL(accepted()), this, SLOT(slotRenameDo()));
//     connect(renameDialog, SIGNAL(rejected()), this, SLOT(slotRenameDone()));
    
//     amodel()->renameLayer(m_menuIndex, *name);
    
    renameDialog->show();
}

void AnimatorView::setRenameName(QString s)
{
    m_rename_str = s;
}

void AnimatorView::slotRenameDo()
{
    amodel()->setLayerName(m_rename_index, m_rename_str);
}

// void AnimatorView::columnCountChanged(int old_count, int new_count)
// {
//     
//     for (int i = old_count; i < new_count; ++i)
//     {
//         setColumnWidth(i, 0);
//     }
//     QTableView::columnCountChanged(old_count, new_count);
// }

// void AnimatorView::tempCheck()
// {
//     std::cout << "Activated signal emmited" << std::endl;
// }
