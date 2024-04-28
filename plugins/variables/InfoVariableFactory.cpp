/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Pierre Ducroquet <pinaraf@gmail.com>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "InfoVariableFactory.h"

#include "InfoVariable.h"
#include "VariablesDebug.h"

#include <KLocalizedString>
#include <KoProperties.h>
#include <KoXmlNS.h>

#include <QStringList>

InfoVariableFactory::InfoVariableFactory()
    : KoInlineObjectFactoryBase("info", TextVariable)
{
    KoInlineObjectTemplate var1;
    var1.id = "author";
    var1.name = i18n("Author Name");
    KoProperties *props = new KoProperties();
    props->setProperty("vartype", KoInlineObject::AuthorName);
    var1.properties = props;
    addTemplate(var1);

    KoInlineObjectTemplate var2;
    var2.id = "title";
    var2.name = i18n("Title");
    props = new KoProperties();
    props->setProperty("vartype", KoInlineObject::Title);
    var2.properties = props;
    addTemplate(var2);

    KoInlineObjectTemplate var3;
    var3.id = "subject";
    var3.name = i18n("Subject");
    props = new KoProperties();
    props->setProperty("vartype", KoInlineObject::Subject);
    var3.properties = props;
    addTemplate(var3);

    KoInlineObjectTemplate var4;
    var4.id = "file-name";
    var4.name = i18n("File Name");
    props = new KoProperties();
    props->setProperty("vartype", KoInlineObject::DocumentURL);
    var4.properties = props;
    addTemplate(var4);

    KoInlineObjectTemplate var5;
    var5.id = "keywords";
    var5.name = i18n("Keywords");
    props = new KoProperties();
    props->setProperty("vartype", KoInlineObject::Keywords);
    var5.properties = props;
    addTemplate(var5);

    KoInlineObjectTemplate var6;
    var6.id = "comments";
    var6.name = i18n("Comments");
    props = new KoProperties();
    props->setProperty("vartype", KoInlineObject::Comments);
    var6.properties = props;
    addTemplate(var6);

    QStringList elementNames(InfoVariable::tags());
    setOdfElementNames(KoXmlNS::text, elementNames);
}

KoInlineObject *InfoVariableFactory::createInlineObject(const KoProperties *properties) const
{
    InfoVariable *var = new InfoVariable();
    if (properties)
        var->readProperties(properties);
    return var;
}
