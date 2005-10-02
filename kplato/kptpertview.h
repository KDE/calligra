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

#include <qsplitter.h>

class QLayout;
class QListViewItem;

class KPrinter;

namespace KPlato
{

class KPTView;
class KPTPertCanvas;
class KPTNode;
class KPTRelation;
class KPTContext;

class KPTPertView : public QWidget
{
    Q_OBJECT
    
public:
 
    KPTPertView( KPTView *view, QWidget *parent, QLayout *layout );

    ~KPTPertView();
    
    void zoom(double /*zoom*/) {}

    void draw();
    KPTView *mainView() const { return m_mainview; } 

    void print(KPrinter &printer);

    KPTNode *currentNode();

    virtual bool setContext(KPTContext &context);
    virtual void getContext(KPTContext &context) const;

public slots:
    void slotRMBPressed(KPTNode *node, const QPoint & point);
    void slotAddRelation(KPTNode *par, KPTNode *child);
    void slotModifyRelation(KPTRelation *rel);

signals:
    void addRelation(KPTNode *par, KPTNode *child);
    void modifyRelation(KPTRelation *rel);

private:
    void init(QLayout *layout);
    KPTView *m_mainview;
    KPTPertCanvas *m_canvasview;
    KPTNode *m_node;
    int m_defaultFontSize;
};

}  //KPlato namespace

#endif
