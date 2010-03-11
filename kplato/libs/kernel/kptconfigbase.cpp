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

#include "kptconfigbase.h"

#include <KGlobal>
#include <KLocale>
#include <kdebug.h>

namespace KPlato
{

ConfigBase::ConfigBase()
    : m_taskDefaults( new Task() ),
    m_locale( 0 )
{
    m_readWrite = true;
    // set some reasonable defaults
    m_taskDefaults->estimate()->setType( Estimate::Type_Effort );
    m_taskDefaults->estimate()->setUnit( Duration::Unit_h );
    m_taskDefaults->estimate()->setExpectedEstimate( 1.0 );
    m_taskDefaults->estimate()->setPessimisticRatio( 0 );
    m_taskDefaults->estimate()->setOptimisticRatio( 0 );
}

ConfigBase::~ConfigBase()
{
    delete m_taskDefaults;
    delete m_locale;
}

void ConfigBase::setTaskDefaults( Task *task )
{
    if ( m_taskDefaults != task ) {
        delete m_taskDefaults;
        m_taskDefaults = task;
    }
}

void ConfigBase::setLocale( KLocale *locale )
{
    if ( locale != m_locale ) {
        delete m_locale;
        m_locale = locale;
    }
}

const KLocale *ConfigBase::locale() const
{
    return m_locale ? m_locale : KGlobal::locale();
}

KLocale *ConfigBase::locale()
{
    return m_locale ? m_locale : KGlobal::locale();
}

}  //KPlato namespace

#include "kptconfigbase.moc"
