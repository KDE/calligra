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
#ifndef KDGANTTVIEW_H
#define KDGANTTVIEW_H

#include <QWidget>
#include "kdganttglobal.h"

class QAbstractItemModel;
class QAbstractProxyModel;
class QAbstractItemView;
class QModelIndex;
class QItemSelectionModel;

namespace KDGantt {
    class ItemDelegate;
    class Constraint;
    class ConstraintModel;
    class AbstractGrid;
    class GraphicsView;
    class AbstractRowController;

    class KDGANTT_EXPORT View : public QWidget {
        Q_OBJECT
        KDGANTT_DECLARE_PRIVATE_BASE_POLYMORPHIC_QWIDGET(View)
        Q_PRIVATE_SLOT( d, void slotCollapsed(const QModelIndex&) )
        Q_PRIVATE_SLOT( d, void slotExpanded(const QModelIndex&) )
        Q_PRIVATE_SLOT( d, void slotVerticalScrollValueChanged( int ) )
        Q_PRIVATE_SLOT( d, void slotLeftWidgetVerticalRangeChanged( int, int ) )
        Q_PRIVATE_SLOT( d, void slotGfxViewVerticalRangeChanged( int, int ) )
    public:
        explicit View(QWidget* parent=0);
        virtual ~View();

        QAbstractItemModel* model() const;
        QItemSelectionModel* selectionModel() const;
        ItemDelegate* itemDelegate() const;
        ConstraintModel* constraintModel() const;
        AbstractGrid* grid() const;
        QModelIndex rootIndex() const;

        QModelIndex indexAt( const QPoint& pos ) const;

        void setLeftView( QAbstractItemView* );
        const QAbstractItemView* leftView() const;
        QAbstractItemView* leftView();

        void setRowController( AbstractRowController* );
        AbstractRowController* rowController();
        const AbstractRowController* rowController() const;

        const GraphicsView* graphicsView() const;
        GraphicsView* graphicsView();
        const QAbstractProxyModel* ganttProxyModel() const;
        QAbstractProxyModel* ganttProxyModel();

        void print( QPainter* painter, const QRectF& target = QRectF(), bool drawRowLabels=true);

    public Q_SLOTS:
        void setModel(QAbstractItemModel* model);
        void setRootIndex( const QModelIndex& idx );
        void setSelectionModel( QItemSelectionModel* smodel );
        void setItemDelegate( ItemDelegate* );
        void setConstraintModel( ConstraintModel* );
        void setGrid( AbstractGrid* );

    protected:
        /*reimp*/ void resizeEvent(QResizeEvent*);
    };
}

#endif /* KDGANTTVIEW_H */
