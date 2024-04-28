/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DateVariableFactory.h"
#include "DateVariable.h"

#include <KoProperties.h>
#include <KoXmlNS.h>

#include <KLocalizedString>

DateVariableFactory::DateVariableFactory()
    : KoInlineObjectFactoryBase("date", TextVariable)
{
    KoInlineObjectTemplate var;
    var.id = "fixed";
    // POST 2.4 change explanation to "data that does not automatically update"
    var.name = i18nc("date that can not be changed later", "Date (Fixed)");
    KoProperties *props = new KoProperties();
    props->setProperty("id", DateVariable::Fixed);
    props->setProperty("definition", "dd/MM/yy");
    var.properties = props;
    addTemplate(var);

    QStringList elementNames;
    elementNames << "date"
                 << "time";
    setOdfElementNames(KoXmlNS::text, elementNames);
}

KoInlineObject *DateVariableFactory::createInlineObject(const KoProperties *properties) const
{
    DateVariable::DateType dt = DateVariable::Fixed;
    if (properties)
        dt = static_cast<DateVariable::DateType>(properties->intProperty("id", dt));

    DateVariable *var = new DateVariable(dt);
    if (properties)
        var->readProperties(properties);
    return var;
}
