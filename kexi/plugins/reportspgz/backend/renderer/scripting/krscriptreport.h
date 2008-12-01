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
#ifndef SCRIPTINGKRSCRIPTREPORT_H
#define SCRIPTINGKRSCRIPTREPORT_H

#include <QObject>
#include <kross/core/object.h>

class KRReportData;
class KRObjectData;
namespace Scripting
{

/**
 @author Adam Pigg <adam@piggz.co.uk>
*/
class Report : public QObject
{
    Q_OBJECT
public:
    Report(KRReportData*);

    ~Report();

public slots:
    QString title();
    QString recordSource();
    QObject* objectByName(const QString &);
    QObject* sectionByName(const QString &);


    void initialize(Kross::Object::Ptr);
    void eventOnOpen();
    void eventOnClose();

private:
    KRReportData *_reportdata;
    Kross::Object::Ptr _scriptObject;
};

}

#endif
