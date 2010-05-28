/*
* This file is part of the KDE project
*
* Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*
* Contact: Amit Aggarwal <amitcs06@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
* 02110-1301 USA
*
*/

#include "PresentationVariableFactory.h"
#include "PresentationVariable.h"

#include <KoProperties.h>
#include <KoXmlNS.h>

#include <klocale.h>
#include <kdebug.h>

PresentationVariableFactory::PresentationVariableFactory(QObject *parent)
    : KoInlineObjectFactoryBase(parent, "PresentationVariable", Other)
{
    KoInlineObjectTemplate var1;
    var1.id = "Header";
    var1.name = i18n("Header");
    KoProperties *props = new KoProperties();
    props->setProperty("vartype", 1);
    var1.properties = props;
    //addTemplate(var1);

    KoInlineObjectTemplate var2;
    var2.id = "Footer";
    var2.name = i18n("Footer");
    props = new KoProperties();
    props->setProperty("vartype", 2);
    var2.properties = props;
    //addTemplate(var2);

    QStringList elementNames;
    elementNames << "footer" << "header" << "date-time";
    setOdfElementNames(KoXmlNS::presentation, elementNames);
}

KoInlineObject *PresentationVariableFactory::createInlineObject(const KoProperties *properties) const
{
    PresentationVariable *var = new PresentationVariable();
    if (properties)
        var->setProperties(properties);
    return var;
}

