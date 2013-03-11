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

#ifndef TESTMODULE_H
#define TESTMODULE_H

#include "kplatoscripting_export.h"

#include <QString>
#include <QStringList>
#include <QObject>

#include <KoScriptingModule.h>


class KPLATOSCRIPTING_TEST_EXPORT TestResult : public QObject
{
    Q_OBJECT
public:
    explicit TestResult(QObject* parent = 0);
    virtual ~TestResult();

    bool isOk() const;
    QString message;

public slots:
    void setMessage( const QString &message );
    void setResult( bool ok );

private:
    bool m_result;
};

#endif
