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

#ifndef kptresource_h
#define kptresource_h

class KPTRisk;
class KPTEffort;
class KPTAppointment;
class KPTRisk;

class KPTResource {
    public:

        KPTResource();
        ~KPTResource();

        KPTRisk *risk();
        void setRisk(KPTRisk *risk);

        void setName(QString);
        QString &name();

        void setAvailableFrom(QTime af);
        QTime availableFrom();
        void setAvailableUntil(QTime au);
        QTime availableUntil();

        void addWorkingHour(QTime from, QTime until);

        QTime getFirstAvailableTime(QTime after=0);
        QTime getBestAvailableTime(QTime duration);

    private:
        QList<KPTAppointment> m_appointments;
        KPTRisk *m_risk;
        QSting m_name;
        QTime m_availableFrom;
        QTime m_availableUntil;
        QList<QTime> m_workingHours;
};

class KPTAppointment {
    public:
        KPTAppointment(QTime startTime, QTime duration, KPTResource resource, KPTNode task=0);
        ~KPTAppointment();

        // get/set member values.
        QTime startTime() { return m_startTime; }
        void setStartTime(QTime st) { m_startTime=st; }

        QTime duration() { return m_duration; }
        void setDuration(QTime d) { m_duration=d; }

        KPTTask *task() { return m_task; }
        void setTask(KPTTask *t) { m_task = t; }

        QTime repeatInterval() {return m_repeatInterval;}
        void setRepeatInterval(QTime ri) {m_repeatInterval=ri;}

        int repeatCount() { return m_repeatCount; }
        void setRepeatCount(int rc) { m_repeatCount=rc; }

        void deleteAppointmentFromRepeatList(QTime time);
        void addAppointmentToRepeatList(QTime time);

    private:
        QTime m_startTime;
        QTime m_duration;
        KPTTask m_task;
        KPTResource m_resource;
        QTime m_repeatInterval;
        int m_repeatCount;
        QList<QTime> m_extraRepeats;
        QList<QTime> m_skipRepeats;
}


/** 
 * Risk is associated with a resource/task pairing to indicate the planner's confidence in the 
 * estimated effort. Risk can be one of none, low, or high. Some factors that may be taken into 
 * account for risk are the experience of the person and the reliability of equipment.
 */
class KPTRisk {
    public:

        enum RiskType {
            NONE=0,
            LOW=1,
            HIGH=2;
        }

        KPTRisk(KPTNode *n, KPTResource *r, RiskType rt=0);
        ~KPTRisk();

        RiskType riskType() { return m_riskType; }

        KPTNode *node() { return m_node; }
        KPTResource *resource() { return m_resource; }

    private:
        KPTNode *m_node;
        KPTResource *m_resource;
        RiskType m_riskType;
}

#endif
