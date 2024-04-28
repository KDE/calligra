/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <mail@dipe.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "UserVariableFactory.h"

#include "UserVariable.h"
#include "VariablesDebug.h"

#include <KoProperties.h>
#include <KoXmlNS.h>

#include <KLocalizedString>

UserVariableFactory::UserVariableFactory()
    : KoInlineObjectFactoryBase("user", TextVariable)
{
    KoInlineObjectTemplate var1;
    var1.id = "userfieldget";
    var1.name = i18n("Custom");
    KoProperties *props = new KoProperties();
    props->setProperty("varproperty", KoInlineObject::UserGet);
    props->setProperty("varname", QString());
    var1.properties = props;
    addTemplate(var1);

    /*
    KoInlineObjectTemplate var2;
    var2.id = "userfieldinput";
    var2.name = i18n("User Input");
    props = new KProperties();
    props->setProperty("varproperty", KoInlineObject::UserField);
    var2.properties = props;
    addTemplate(var2);
    */

    QStringList elementNames;
    elementNames << "user-field-get"
                 << "user-field-input";
    setOdfElementNames(KoXmlNS::text, elementNames);
}

KoInlineObject *UserVariableFactory::createInlineObject(const KoProperties *properties) const
{
    UserVariable *var = new UserVariable();
    if (properties)
        var->readProperties(properties);
    return var;
}
