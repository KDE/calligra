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
#include <qdatetime.h>
#include "kptresource.h"
#include "kpttask.h"


KPTResourceGroup::KPTResourceGroup() {
}

KPTResourceGroup::~KPTResourceGroup() {
}

	
void KPTResourceGroup::setName(QString) {
}

QString &KPTResourceGroup::name() {
}
	
	
void KPTResourceGroup::addResource(KPTResource*, KPTRisk*) {
}

KPTResource* KPTResourceGroup::getResource(int) {
}

KPTRisk* KPTResourceGroup::getRisk(int) {
}

void KPTResourceGroup::removeResource(int) {
}
	
void KPTResourceGroup::addRequiredResource(KPTResourceGroup*) {
}

KPTResourceGroup* KPTResourceGroup::getRequiredResource(int) {
}

void KPTResourceGroup::removeRequiredResource(int) {
}

KPTResource::KPTResource() : m_appointments(), m_workingHours() {
}

KPTResource::~KPTResource() {
}

void KPTResource::setName(QString) {
}

QString &KPTResource::name() {
}

void KPTResource::setAvailableFrom(QDateTime *af) {
}

QDateTime *KPTResource::availableFrom() {
}

void KPTResource::setAvailableUntil(QDateTime *au) {
}

QDateTime *KPTResource::availableUntil() {
}

void KPTResource::addWorkingHour(QDateTime *from, QDateTime *until) {
}

QDateTime *KPTResource::getFirstAvailableTime(QDateTime *after) {
}

QDateTime *KPTResource::getBestAvailableTime(QDateTime *duration) {
}


KPTAppointment::KPTAppointment(QDateTime *startTime, QDateTime *duration, KPTResource *resource, KPTTask *taskNode) :m_extraRepeats(), m_skipRepeats() {
    m_startTime=startTime;
    m_duration=duration;
    m_task=taskNode;
    m_resource=resource;
    //m_repeatInterval=new QDateTime(0);
    m_repeatCount=0;
}

KPTAppointment::~KPTAppointment() {
}

void KPTAppointment::deleteAppointmentFromRepeatList(QDateTime *time) {
}

void KPTAppointment::addAppointmentToRepeatList(QDateTime *time) {
}


KPTRisk::KPTRisk(KPTNode *n, KPTResource *r, RiskType rt) {
    m_node=n;
    m_resource=r;
    m_riskType=rt;
}

KPTRisk::~KPTRisk() {
}
