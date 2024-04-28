/*
* This file is part of the KDE project
*
* SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
*
* Contact: Amit Aggarwal <amitcs06@gmail.com>
*
SPDX-License-Identifier: LGPL-2.1-or-later
*
*/

#include "PresentationVariableFactory.h"
#include "PresentationVariable.h"

#include <KoProperties.h>
#include <KoXmlNS.h>

#include <KLocalizedString>

PresentationVariableFactory::PresentationVariableFactory()
    : KoInlineObjectFactoryBase("PresentationVariable", Other)
{
    KoInlineObjectTemplate var1;
    var1.id = "Header";
    var1.name = i18n("Header");
    KoProperties *props = new KoProperties();
    props->setProperty("vartype", 1);
    var1.properties = props;
    addTemplate(var1);

    KoInlineObjectTemplate var2;
    var2.id = "Footer";
    var2.name = i18n("Footer");
    props = new KoProperties();
    props->setProperty("vartype", 2);
    var2.properties = props;
    addTemplate(var2);

    QStringList elementNames;
    elementNames << "footer"
                 << "header"
                 << "date-time";
    setOdfElementNames(KoXmlNS::presentation, elementNames);
}

KoInlineObject *PresentationVariableFactory::createInlineObject(const KoProperties *properties) const
{
    PresentationVariable *var = new PresentationVariable();
    if (properties)
        var->setProperties(properties);
    return var;
}
