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

KPTResource::KPTResource() : m_appointments(), m_workingHours() {
}

KPTResource::~KPTResource() {
}

KPTRisk *KPTResource::risk() {
}

void KPTResource::setRisk(KPTRisk *risk) {
}

void KPTResource::setName(QString) {
}

QString &KPTResource::name() {
}

void KPTResource::setAvailableFrom(QTime af) {
}

QTime KPTResource::availableFrom() {
}

void KPTResource::setAvailableUntil(QTime au) {
}

QTime KPTResource::availableUntil() {
}

void KPTResource::addWorkingHour(QTime from, QTime until) {
}

QTime KPTResource::getFirstAvailableTime(QTime after) {
}

QTime KPTResource::getBestAvailableTime(QTime duration) {
}


KPTAppointment::KPTAppointment(QTime startTime, QTime duration, KPTResource resource, KPTNode task) :m_extraRepeats(), m_skipRepeats() {
    m_startTime=startTime;
    m_duration=duration;
    m_task=task;
    m_resource=resource;
    m_repeatInterval=new QTime(0);
    m_repeatCount=0;
}

KPTAppointment::~KPTAppointment() {
}

void KPTAppointment::deleteAppointmentFromRepeatList(QTime time) {
}

void KPTAppointment::addAppointmentToRepeatList(QTime time) {
}

KPTRisk::KPTRisk(KPTNode *n, KPTResource *r, RiskType rt) {
    m_node=n;
    m_resource=r;
    m_riskType=rt;
}

KPTRisk::~KPTRisk();
