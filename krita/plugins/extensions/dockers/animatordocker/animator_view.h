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


#ifndef ANIMATOR_WIDGET_H
#define ANIMATOR_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QLabel>
#include <KMenu>

#include "animator_model.h"

class AnimatorView : public QTableView
{
    Q_OBJECT
    
public:
    AnimatorView();
    virtual ~AnimatorView();
    
    virtual void setModel(QAbstractItemModel* model);

public:
    AnimatorModel* amodel();

    void renameLayer(QModelIndex& index);
    
public slots:
    void slotNewPaint();
    void slotNewShape();
    void slotNewGenerated();
    void slotNewFilter();
    void slotNewClone();                // not now
    void slotNewGroup();
    void slotCopyPrevious();
    void slotCopyNext();
    void slotInterpolate();
    
    void slotClearFrames();
    
    void slotRenameLayer();
    
    void slotCustomContextMenuRequested(const QPoint& pos);
    
    void setRenameName(QString s);
    
    void slotRenameDo();
//     void slotRenameDone();
    
//     void tempCheck();
    
protected slots:
//     void slotSetHeaderWidth();
//     void columnCountChanged(int old_count, int new_count);
    
    
protected:
    KMenu* m_itemContextMenu;
    QModelIndex m_menuIndex;
    
    QString m_rename_str;
    QModelIndex m_rename_index;
    
};

#endif // ANIMATOR_WIDGET_H
