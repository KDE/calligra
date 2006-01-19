/* This file is part of the KDE project
   Copyright (C) 2003, 2004 Dag Andersen <danders@get2net.dk>

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
 
#ifndef KPTPERTVIEW_H
#define KPTPERTVIEW_H

#include "kptcontext.h"

#include <qsplitter.h>

class QLayout;
class QListViewItem;

class KPrinter;

namespace KPlato
{

class View;
class PertCanvas;
class Node;
class Relation;

class PertView : public QWidget
{
    Q_OBJECT
    
public:
 
    PertView( View *view, QWidget *parent, QLayout *layout );

    ~PertView();
    
    void setZoom(double /*zoom*/) {}

    void draw();
    View *mainView() const { return m_mainview; } 

    void print(KPrinter &printer);

    Node *currentNode();

    virtual bool setContext(Context::Pertview &context);
    virtual void getContext(Context::Pertview &context) const;

public slots:
    void slotRMBPressed(Node *node, const QPoint & point);
    void slotAddRelation(Node *par, Node *child);
    void slotModifyRelation(Relation *rel);

signals:
    void addRelation(Node *par, Node *child);
    void modifyRelation(Relation *rel);

private:
    void init(QLayout *layout);
    View *m_mainview;
    PertCanvas *m_canvasview;
    Node *m_node;
    int m_defaultFontSize;
};

}  //KPlato namespace

#endif
