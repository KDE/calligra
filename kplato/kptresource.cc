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
#include "kptresource.h"
#include "kpttask.h"


KPTResourceGroup::KPTResourceGroup() {
}

KPTResourceGroup::~KPTResourceGroup() {
}

void KPTResourceGroup::addResource(KPTResource*, KPTRisk*) {
}

KPTResource* KPTResourceGroup::getResource(int) {
    return 0L;
}

KPTRisk* KPTResourceGroup::getRisk(int) {
    return 0L;
}

void KPTResourceGroup::removeResource(int) {
}
	
void KPTResourceGroup::addRequiredResource(KPTResourceGroup*) {
}

KPTResourceGroup* KPTResourceGroup::getRequiredResource(int) {
    return 0L;
}

void KPTResourceGroup::removeRequiredResource(int) {
}

KPTResource::KPTResource() : m_appointments(), m_workingHours() {
}

KPTResource::~KPTResource() {
}

void KPTResource::setName(QString) {
}

void KPTResource::addWorkingHour(KPTDuration from, KPTDuration until) {
}

KPTDuration *KPTResource::getFirstAvailableTime(KPTDuration after) {
    return 0L;
}

KPTDuration *KPTResource::getBestAvailableTime(KPTDuration duration) {
    return 0L;
}


KPTAppointment::KPTAppointment(KPTDuration startTime, KPTDuration duration, KPTResource *resource, KPTTask *taskNode) :m_extraRepeats(), m_skipRepeats() {
    m_startTime=startTime;
    m_duration=duration;
    m_task=taskNode;
    m_resource=resource;
    m_repeatInterval=KPTDuration();
    m_repeatCount=0;
}

KPTAppointment::~KPTAppointment() {
}

void KPTAppointment::deleteAppointmentFromRepeatList(KPTDuration time) {
}

void KPTAppointment::addAppointmentToRepeatList(KPTDuration time) {
}


KPTRisk::KPTRisk(KPTNode *n, KPTResource *r, RiskType rt) {
    m_node=n;
    m_resource=r;
    m_riskType=rt;
}

KPTRisk::~KPTRisk() {
}
