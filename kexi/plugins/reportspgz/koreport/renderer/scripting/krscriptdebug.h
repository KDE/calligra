/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#ifndef KRSCRIPTDEBUG_H
#define KRSCRIPTDEBUG_H

#include <QObject>

/**
 @author Adam Pigg <adam@piggz.co.uk>
*/
class KRScriptDebug : public QObject
{
    Q_OBJECT
public:
    KRScriptDebug(QObject *parent = 0);

    ~KRScriptDebug();

public slots:
    void print(const QString&);
    void message(const QString &, const QString&);

};

#endif
