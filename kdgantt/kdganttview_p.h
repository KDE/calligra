/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Gantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Gantt licenses may use this file in
 ** accordance with the KD Gantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdgantt for
 **   information about KD Gantt Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#ifndef KDGANTTVIEW_P_H
#define KDGANTTVIEW_P_H

#include "kdganttview.h"
#include "kdganttgraphicsscene.h"
#include "kdgantttreeviewrowcontroller.h"
#include "kdganttconstraintmodel.h"
#include "kdganttconstraintproxy.h"

#include "kdganttgraphicsview.h"
#include "kdganttdatetimegrid.h"

#include "kdganttproxymodel.h"

#include <QSplitter>
#include <QTreeView>
#include <QGraphicsView>
#include <QPointer>

class QAbstractProxyModel;
class QGraphicsView;
class QGraphicsScene;
class QGraphicsRectItem;

namespace KDGantt {
    class GraphicsItem;
    class KDGanttTreeView;

    /* internal */

    /* internal */
    class KDGanttTreeView : public QTreeView {
    public:
        explicit KDGanttTreeView( QAbstractProxyModel* proxy, QWidget* parent=0 );
        virtual ~KDGanttTreeView();

        AbstractRowController* rowController() { return &m_controller; }
    private:
        TreeViewRowController m_controller;
    };

    class View::Private {
    public:
        explicit Private(View*);
        virtual ~Private();

        void init();

        GraphicsItem* createItem( ItemType type ) const;

        void updateScene();

        // slots
        void slotCollapsed(const QModelIndex&);
        void slotExpanded(const QModelIndex&);
        void slotVerticalScrollValueChanged( int );
        void slotLeftWidgetVerticalRangeChanged( int, int );
        void slotGfxViewVerticalRangeChanged( int, int );

        View* q;

        QSplitter splitter;

        /* TODO: Refine/subclass */
        //KDGanttTreeView treeview;
        QPointer<QAbstractItemView> leftWidget;
        AbstractRowController* rowController;
        GraphicsView gfxview;
        //KDGanttHeaderWidget headerwidget;

        QPointer<QAbstractItemModel> model;
        ProxyModel ganttProxyModel;
        //KDGanttTreeViewRowController rowController;
        ConstraintModel mappedConstraintModel;
        ConstraintProxy constraintProxy;
    };

}
#endif /* KDGANTTVIEW_P_H */

