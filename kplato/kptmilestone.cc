/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kptmilestone.h"
#include "kptmilestonedialog.h"
#include "kptpertcanvas.h"
#include "kptcanvasitem.h"

#include <qdom.h>
#include <qbrush.h>
#include <kdebug.h>
#include <koRect.h> //DEBUGRECT

KPTMilestone::KPTMilestone(KPTNode *parent) : KPTNode(parent) {
}

KPTMilestone::~KPTMilestone() {
}

int KPTMilestone::type() const { return TYPE; }
int KPTMilestone::TYPE = KPTNode::Type_Milestone;

bool KPTMilestone::load(QDomElement &element) {
    kdDebug()<<k_funcinfo<<endl;
    m_name = element.attribute("name");
    m_leader = element.attribute("leader");
    m_description = element.attribute("description");

    earliestStart = KPTDuration(QDateTime::fromString(element.attribute("earlieststart")));
    latestFinish = KPTDuration(QDateTime::fromString(element.attribute("latestfinish")));

    return true;
}

void KPTMilestone::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("milestone");
    element.appendChild(me);

    me.setAttribute("name", m_name);
    me.setAttribute("leader", m_leader);
    me.setAttribute("description", m_description);

    me.setAttribute("earlieststart",earliestStart.dateTime().toString());
    me.setAttribute("latestfinish",latestFinish.dateTime().toString());
    
    // Only save parent relations    
    QPtrListIterator<KPTRelation> it(m_dependParentNodes);
    for ( ; it.current(); ++it ) {
        it.current()->save(me);
    }

}

KPTDuration *KPTMilestone::getStartTime() {
    return new KPTDuration(earliestStart);
}

KPTDuration *KPTMilestone::getFloat() {
    return 0L;
}


bool KPTMilestone::openDialog() {
    kdDebug()<<k_funcinfo<<endl;
    KPTMilestoneDialog *dialog = new KPTMilestoneDialog(*this);
    bool ret = dialog->exec();
    delete dialog;
    return ret;
}

void KPTMilestone::drawPert(KPTPertCanvas *view, KPTNode *parent) {
	if (!m_drawn) {
		if ( numChildren() > 0 ) {
			int col = view->summaryColumn();
			m_pertItem = new KPTPertMilestoneItem(view, *this, 0, col);
			m_pertItem->show();
			m_drawn = true;
			kdDebug()<<k_funcinfo<<" drawn milestone(?)("<<0<<","<<col<<"): "<<m_name<<endl;
			return;
		}
		if (!allParentsDrawn()) {
			return;
		}
		int col = getColumn();
		int row = view->row(getRow(), col);
		m_pertItem = new KPTPertMilestoneItem(view, *this, row, col);
		m_pertItem->show();
		m_drawn = true;
	}
	QPtrListIterator<KPTRelation> cit(m_dependChildNodes);
	for ( ; cit.current(); ++cit ) {
		cit.current()->child()->drawPert(view);
	}
}

#ifndef NDEBUG
void KPTMilestone::printDebug(bool children, QCString indent) {
    QCString i2 = indent + "---";
    kdDebug()<<indent<<" Milestone: "<<name()<<endl;
    KPTNode::printDebug(children, indent);
        
}
#endif
