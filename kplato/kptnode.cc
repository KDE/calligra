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
#include "kptnode.h"

KPTNode::KPTNode() : m_nodes(), m_dependChildNodes(), m_dependParentNodes(), m_risk() {
        m_name="";
        m_risk.setAutoDelete(true);
/*
something like
QDate *nul = neq QDate(0,1,1);
m_startTime=new QDateTime(*log);
etc..
        m_startTime=new QTime(0), m_endTime=new QTime(0);
        m_optimisticDuration=new QTime(0);
        m_pessemisticDuration=new QTime(0);
        m_expectedDuration=new QTime(0);
*/
}

KPTNode::~KPTNode() {
}

void KPTNode::delChildNode( KPTNode *node, bool remove) {
}

void KPTNode::delChildNode( int number, bool remove) {
}

void KPTNode::addRisk( KPTRisk *risk ) {
}

void KPTNode::removeRisk( KPTRisk *risk ){
   // always auto remove
}

void KPTNode::removeRisk( int number ){
   // always auto remove
}

void KPTNode::insertRisk( unsigned int index, KPTRisk *risk ) {
}

void KPTNode::insertChildNode( unsigned int index, KPTNode *node) {
}

void KPTNode::addChildNode( KPTNode *node) {
}

QDateTime *KPTNode::getDelay() {
    return 0L;
}

