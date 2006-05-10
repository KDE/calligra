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
 
#include "kptpertview.h"

#include "kptview.h"
#include "kptpertcanvas.h"
#include "kptpart.h"
#include "kptproject.h"
#include "kptrelation.h"
#include "kptcontext.h"

#include <kdebug.h>

#include <qsplitter.h>
#include <q3vbox.h>
#include <QLayout>
#include <q3listview.h>
#include <q3header.h>
#include <q3popupmenu.h>
//Added by qt3to4:
#include <Q3GridLayout>

#include <kprinter.h>

namespace KPlato
{

PertView::PertView( View *view, QWidget *parent, QLayout *layout )
    : QWidget( parent, "Pert view" ),
    m_mainview( view ),
    m_node( 0 )
{
    init(layout);
}

PertView::~PertView()
{
}

void PertView::init(QLayout */*layout*/)
{
    //kDebug()<<k_funcinfo<<endl;
    Q3GridLayout *gl = new Q3GridLayout(this, 1, 1, -1, -1, "Pert QGridLayout");
    m_canvasview = new PertCanvas(this);
        gl->addWidget(m_canvasview, 0, 0);
    draw();
    connect(m_canvasview, SIGNAL(rightButtonPressed(Node *, const QPoint &)), this, SLOT(slotRMBPressed(Node *,const QPoint &)));
    connect(m_canvasview, SIGNAL(updateView(bool)), m_mainview, SLOT(slotUpdate(bool)));

    connect(m_canvasview, SIGNAL(addRelation(Node*, Node*)), SLOT(slotAddRelation(Node*, Node*)));
    connect(m_canvasview, SIGNAL(modifyRelation(Relation*)), SLOT(slotModifyRelation(Relation*)));
}    

void PertView::draw() 
{
    //kDebug()<<k_funcinfo<<endl;
    m_canvasview->draw(m_mainview->getPart()->getProject());
    m_canvasview->show();
}

void PertView::slotRMBPressed(Node *node, const QPoint & point)
{
    //kDebug()<<k_funcinfo<<" node: "<<node->name()<<endl;
    m_node = node;
    if (node && (node->type() == Node::Type_Task || node->type() == Node::Type_Milestone)) {
        Q3PopupMenu *menu = m_mainview->popupMenu("task_popup");
        if (menu)
        {
            /*int id =*/ menu->exec(point);
            //kDebug()<<k_funcinfo<<"id="<<id<<endl;
        }
        return;
    }
    if (node && node->type() == Node::Type_Summarytask) {
        Q3PopupMenu *menu = m_mainview->popupMenu("node_popup");
        if (menu)
        {
            /*int id =*/ menu->exec(point);
            //kDebug()<<k_funcinfo<<"id="<<id<<endl;
        }
        return;
    }
    //TODO: Other nodetypes
}

void PertView::slotAddRelation(Node* par, Node* child)
{
    kDebug()<<k_funcinfo<<endl;
    emit addRelation(par, child);
}

void PertView::slotModifyRelation(Relation *rel)
{
    kDebug()<<k_funcinfo<<endl;
    emit modifyRelation(rel);
}

void PertView::print(KPrinter &printer)
{
    Q_UNUSED(printer);
    kDebug()<<k_funcinfo<<endl;

}

Node *PertView::currentNode()
{
    return m_canvasview->selectedNode(); 
}

bool PertView::setContext(Context::Pertview &context)
{
    Q_UNUSED(context);
    //kDebug()<<k_funcinfo<<endl;
    return true;
}

void PertView::getContext(Context::Pertview &context) const
{
    Q_UNUSED(context);
    //kDebug()<<k_funcinfo<<endl;
}

}  //KPlato namespace

#include "kptpertview.moc"
