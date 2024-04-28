/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <mail@dipe.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChapterVariableFactory.h"
#include "ChapterVariable.h"
#include "VariablesDebug.h"

#include <KoProperties.h>
#include <KoXmlNS.h>

#include <KLocalizedString>

ChapterVariableFactory::ChapterVariableFactory()
    : KoInlineObjectFactoryBase("chapter", TextVariable)
{
    KoInlineObjectTemplate var1;
    var1.id = "chapter";
    var1.name = i18n("Chapter");
    KoProperties *props = new KoProperties();
    props->setProperty("vartype", KoInlineObject::Chapter);
    props->setProperty("format", 2);
    props->setProperty("level", 1);
    var1.properties = props;
    addTemplate(var1);

    QStringList elementNames;
    elementNames << "chapter";
    setOdfElementNames(KoXmlNS::text, elementNames);
}

KoInlineObject *ChapterVariableFactory::createInlineObject(const KoProperties *properties) const
{
    ChapterVariable *var = new ChapterVariable();
    if (properties)
        var->readProperties(properties);
    return var;
}
