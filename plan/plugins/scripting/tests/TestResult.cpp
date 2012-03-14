/*
 * This file is part of KPlato
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 * Copyright (c) 2008 Dag Andersen <danders@get2net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "TestResult.h"

#include <kdebug.h>

extern "C"
{
    KDE_EXPORT QObject* krossmodule()
    {
        return new TestResult();
    }
}

TestResult::TestResult(QObject* parent)
    : QObject(parent)
    , m_result( false )
{
    setObjectName( "TestResult" );
}

TestResult::~TestResult()
{
}

bool TestResult::isOk() const
{
    return m_result;
}

void TestResult::setResult( bool ok)
{
    m_result = ok;
}

void TestResult::setMessage( const QString &m )
{
    qDebug()<<m;
    message = m;
}


#include "TestResult.moc"
