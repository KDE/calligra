/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KPTCONFIGBASE_H
#define KPTCONFIGBASE_H

#include "kpttask.h"

class KLocale;

namespace KPlato
{

class KPLATOKERNEL_EXPORT ConfigBase 
{
public:
    ConfigBase();
    virtual ~ConfigBase();

    void setReadWrite(bool readWrite) { m_readWrite = readWrite; }
    Task &taskDefaults() { setDefaultValues( *m_taskDefaults ); return *m_taskDefaults; }
    void setTaskDefaults( Task * );

    virtual void setDefaultValues( Task & ) {}
    virtual QPair<int, int> durationUnitRange() const { return QPair<int, int>(); }
    virtual int minimumDurationUnit() const { return Duration::Unit_h; }
    virtual int maximumDurationUnit() const { return Duration::Unit_Y; }

    
    void setLocale( KLocale *locale );
    const KLocale *locale() const { return m_locale; }
    KLocale *locale() { return m_locale; }

protected:
    bool m_readWrite;

private:
    Task *m_taskDefaults;

    KLocale *m_locale;

};

}  //KPlato namespace

#endif // CONFIGBASE_H
