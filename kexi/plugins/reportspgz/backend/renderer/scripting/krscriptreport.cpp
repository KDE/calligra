/*
 * Kexi Report Plugin
 * Copyright (C) 2001-2007 by OpenMFG, LLC
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
#include "krscriptreport.h"
#include <krreportdata.h>
#include <krobjectdata.h>
#include "krscriptlabel.h"
#include "krscriptfield.h"
#include "krscripttext.h"
#include "krscriptbarcode.h"
#include "krscriptimage.h"
#include "krscriptline.h"
#include "krscriptchart.h"
#include "krscriptsection.h"

namespace Scripting
{

Report::Report(KRReportData *r)
{
    _reportdata = r;
    _scriptObject = 0;
}


Report::~Report()
{

}

QString Report::title()
{
    return _reportdata->title;
}

QString Report::name()
{
    return _reportdata->name();
}

QString Report::recordSource()
{
    return _reportdata->query();
}

QObject* Report::objectByName(const QString &n)
{
    QList<KRObjectData *>obs = _reportdata->objects();
    foreach(KRObjectData *o, obs) {
        if (o->entityName() == n) {
            switch (o->type()) {
            case KRObjectData::EntityLabel:
                return new Scripting::Label(o->toLabel());
                break;
            case KRObjectData::EntityField:
                return new Scripting::Field(o->toField());
                break;
            case KRObjectData::EntityText:
                return new Scripting::Field(o->toField());
                break;
            case KRObjectData::EntityBarcode:
                return new Scripting::Barcode(o->toBarcode());
                break;
            case KRObjectData::EntityLine:
                return new Scripting::Line(o->toLine());
                break;
            case KRObjectData::EntityChart:
                return new Scripting::Chart(o->toChart());
                break;
            case KRObjectData::EntityImage:
                return new Scripting::Image(o->toImage());
                break;
            default:
                return new QObject();
            }
        }
    }
    return 0;
}

QObject* Report::sectionByName(const QString &n)
{
    KRSectionData *sec = _reportdata->section(n);
    if (sec) {
        return new Scripting::Section(sec);
    } else {
        return new QObject();
    }
}

void Report::initialize(Kross::Object::Ptr ptr)
{
        _scriptObject = ptr;
}

void Report::eventOnOpen()
{
    if (_scriptObject)
	_scriptObject->callMethod("OnOpen");
}

void Report::eventOnClose()
{
    if (_scriptObject)
	_scriptObject->callMethod("OnClose");
}


}
