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
#include "kptrelation.h"
#include "defs.h"

KPTRelation::KPTRelation(KPTNode *parent, KPTNode *child, TimingType tt, TimingRelation tr, KPTDuration lag) {
    m_parent=parent;
    m_child=child;
    m_timingType=tt;
    m_timingRelation=tr;
    m_lag=lag;
}

KPTRelation::KPTRelation(KPTNode *parent, KPTNode *child, TimingType tt, TimingRelation tr) {
    m_parent=parent;
    m_child=child;
    m_timingType=tt;
    m_timingRelation=tr;
    m_lag=KPTDuration();
}

KPTRelation::~KPTRelation() {
}

void KPTRelation::timingType(TimingType tt) {
}
void KPTRelation::timingRelation(TimingRelation tr) {
}

