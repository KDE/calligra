/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org

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

#include "kptduration.h"

KPTDuration::KPTDuration() {
    zero = QDateTime(QDate(0,1,1));
    m_theTime = QDateTime(QDate(0,1,1));
}

KPTDuration::KPTDuration(const KPTDuration &d) {
    zero = QDateTime(QDate(0,1,1));
    set(d.m_theTime);
}

KPTDuration::KPTDuration(int h, int m, int s, int ms) {
    zero = QDateTime(QDate(0,1,1));
    m_theTime = QDateTime(QDate(0,1,1),QTime(h,m,s,ms));
}

KPTDuration::~KPTDuration() {
}

void KPTDuration::add(KPTDuration time) {
    set(m_theTime.addDays(zero.daysTo(time.m_theTime)));
    set(m_theTime.addSecs(zero.secsTo(time.m_theTime)));
}

void KPTDuration::substract(KPTDuration time) {
    set(m_theTime.addDays(time.m_theTime.daysTo(zero)));
    set(m_theTime.addSecs(time.m_theTime.secsTo(zero)));
}

void const KPTDuration::set(KPTDuration newTime) {
    set(newTime.m_theTime);
}

void const KPTDuration::set(QDateTime newTime) {
    m_theTime.setDate(newTime.date());
    m_theTime.setTime(newTime.time());
}

