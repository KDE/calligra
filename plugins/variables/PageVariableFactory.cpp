/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Pierre Ducroquet <pinaraf@gmail.com>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "PageVariableFactory.h"

#include "PageVariable.h"
#include "VariablesDebug.h"

#include <KoProperties.h>
#include <KoXmlNS.h>

#include <KLocalizedString>

PageVariableFactory::PageVariableFactory()
    : KoInlineObjectFactoryBase("page", TextVariable)
{
    KoInlineObjectTemplate var1;
    var1.id = "pagecount";
    var1.name = i18n("Page Count");
    KoProperties *props = new KoProperties();
    props->setProperty("vartype", 1);
    var1.properties = props;
    addTemplate(var1);

    KoInlineObjectTemplate var2;
    var2.id = "pagenumber";
    var2.name = i18n("Page Number");
    props = new KoProperties();
    props->setProperty("vartype", 2);
    var2.properties = props;
    addTemplate(var2);

    // KoInlineObjectTemplate var3;
    // var3.id = "pagecontinuation";
    // var3.name = i18n("Page Continuation");
    // props = new KoProperties();
    // props->setProperty("vartype", 3);
    // var3.properties = props;
    // addTemplate(var3);

    QStringList elementNames;
    elementNames << "page-count"
                 << "page-number"
                 << "page-continuation-string";
    setOdfElementNames(KoXmlNS::text, elementNames);
}

KoInlineObject *PageVariableFactory::createInlineObject(const KoProperties *properties) const
{
    PageVariable *var = new PageVariable();
    if (properties)
        var->readProperties(properties);
    return var;
}
