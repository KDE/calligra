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

#ifndef kptduration
#define kptduration

#include <qdatetime.h> 

class KPTDuration;

/**
 * This class is a wrapper class for the QDateTime class
 */
class KPTDuration {
    public:
        KPTDuration();
        KPTDuration(const KPTDuration &d);
        KPTDuration(int h, int m, int s=0, int ms=0);
        ~KPTDuration();

        void add(KPTDuration time);
        void substract(KPTDuration time);
        void const set(KPTDuration newTime);
        void const set(QDateTime newTime);

        bool   operator==( const KPTDuration &d ) const { return m_theTime == d.m_theTime; }
        bool   operator!=( const KPTDuration &d ) const { return m_theTime != d.m_theTime; }
        bool   operator<( const KPTDuration &d ) const { return m_theTime < d.m_theTime; }
        bool   operator<=( const KPTDuration &d ) const { return m_theTime <= d.m_theTime; }
        bool   operator>( const KPTDuration &d ) const { return m_theTime > d.m_theTime; }
        bool   operator>=( const KPTDuration &d ) const { return m_theTime >= d.m_theTime; }
        KPTDuration &operator = ( const KPTDuration &d ) { set(d); return *this;}

        QString toString() const { return m_theTime.toString(); }

    private:
        QDateTime zero, m_theTime;
};
#endif
