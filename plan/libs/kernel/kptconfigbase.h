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

class KPLATOKERNEL_EXPORT ConfigBase : public QObject
{
    Q_OBJECT
public:
    ConfigBase();
    virtual ~ConfigBase();

    void setReadWrite(bool readWrite) { m_readWrite = readWrite; }
    Task &taskDefaults() const {
        const_cast<ConfigBase*>( this )->setDefaultValues( *m_taskDefaults );
        return *m_taskDefaults;
    }
    void setTaskDefaults( Task * );

    virtual void setDefaultValues( Task & ) {}
    virtual QPair<int, int> durationUnitRange() const { return QPair<int, int>(); }
    virtual int minimumDurationUnit() const { return Duration::Unit_h; }
    virtual int maximumDurationUnit() const { return Duration::Unit_Y; }

    QBrush summaryTaskLevelColor( int level ) const;
    virtual bool summaryTaskLevelColorsEnabled() const;
    virtual QBrush summaryTaskDefaultColor() const;
    virtual QBrush summaryTaskLevelColor_1() const;
    virtual QBrush summaryTaskLevelColor_2() const;
    virtual QBrush summaryTaskLevelColor_3() const;
    virtual QBrush summaryTaskLevelColor_4() const;

    virtual QBrush taskNormalColor() const;
    virtual QBrush taskErrorColor() const;
    virtual QBrush taskCriticalColor() const;
    virtual QBrush taskFinishedColor() const;

    virtual QBrush milestoneNormalColor() const;
    virtual QBrush milestoneErrorColor() const;
    virtual QBrush milestoneCriticalColor() const;
    virtual QBrush milestoneFinishedColor() const;

    void setLocale( KLocale *locale );
    const KLocale *locale() const;
    KLocale *locale();

    static QBrush gradientBrush( const QColor &c );

protected:
    bool m_readWrite;

private:
    Task *m_taskDefaults;

    KLocale *m_locale;

};

}  //KPlato namespace

#endif // CONFIGBASE_H
