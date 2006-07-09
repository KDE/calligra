/* This file is part of the KDE project
   Copyright (C) 2003 - 2004 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTPERTCANVAS_H
#define KPTPERTCANVAS_H

#include "kptnode.h"
#include "kptproject.h"

#include <qcanvas.h>
#include <qmemarray.h>
#include <qptrdict.h>

class QTimer;
class QPainter;
class QPoint;
class QSize;

namespace KPlato
{

class PertNodeItem;

class PertCanvas : public QCanvasView
{
    Q_OBJECT

public:
    PertCanvas( QWidget *parent );
    virtual ~PertCanvas();

    void draw(Project& project);
    void clear();
    QSize canvasSize();

    PertNodeItem *selectedItem();

    int verticalGap() { return m_verticalGap; }
    int horizontalGap() { return m_horizontalGap; }
    QSize itemSize() { return m_itemSize; }

    void setColumn(int row, int col) { m_rows.at(row)[col] = true; }

    void mapNode(PertNodeItem *item);
    void mapChildNode(PertNodeItem *parentItem, PertNodeItem *childItem, Relation::Type type);
    
    Node *selectedNode();

protected:
    void drawRelations();

    void createChildItems(PertNodeItem *node);
    PertNodeItem *createNodeItem(Node *node);

    void contentsMousePressEvent ( QMouseEvent * e );
    void contentsMouseReleaseEvent ( QMouseEvent * e );

signals:
    void rightButtonPressed(Node *node, const QPoint & point);
    void updateView(bool calculate);
    void addRelation(Node *par, Node *child);
    void modifyRelation(Relation *rel);


private:
    QCanvas *m_canvas;

    QTimer *m_scrollTimer;
    bool m_mousePressed;
    bool m_printing;

	int m_verticalGap;
	int m_horizontalGap;
	QSize m_itemSize;

    QPtrDict<PertNodeItem> m_nodes;
    QPtrList<Relation> m_relations;

    QPtrList<QMemArray<bool> > m_rows;

#ifndef NDEBUG
    void printDebug( int );
#endif

};

}  //KPlato namespace

#endif
