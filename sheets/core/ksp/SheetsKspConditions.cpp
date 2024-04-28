// This file is part of the KDE project
// SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
// Copyright 1999- 2006 The KSpread Team <calligra-devel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "SheetsKsp.h"
#include "SheetsKspPrivate.h"

#include <KoXmlReader.h>

#include "engine/ValueConverter.h"
#include "engine/ValueParser.h"

#include "Condition.h"

namespace Calligra
{
namespace Sheets
{

QDomElement Ksp::saveConditions(Conditions *conds, QDomDocument &doc, ValueConverter *converter)
{
    QDomElement conditions = doc.createElement("condition");
    QDomElement child;
    int num = 0;
    QString name;

    const QList<Conditional> list = conds->conditionList();
    QList<Conditional>::const_iterator it;
    for (it = list.begin(); it != list.end(); ++it) {
        Conditional condition = *it;

        /* the name of the element will be "condition<n>"
         * This is unimportant now but in older versions three conditions were
         * hardcoded with names "first" "second" and "third"
         */
        name.setNum(num);
        name.prepend("condition");

        child = doc.createElement(name);
        child.setAttribute("cond", QString::number((int)condition.cond));

        // TODO: saving in KSpread 1.1 | KSpread 1.2 format
        if (condition.value1.isString()) {
            child.setAttribute("strval1", condition.value1.asString());
            if (!condition.value2.asString().isEmpty()) {
                child.setAttribute("strval2", condition.value2.asString());
            }
        } else {
            child.setAttribute("val1", converter->asString(condition.value1).asString());
            child.setAttribute("val2", converter->asString(condition.value2).asString());
        }
        if (!condition.styleName.isEmpty()) {
            child.setAttribute("style", condition.styleName);
        }

        conditions.appendChild(child);

        ++num;
    }

    if (num == 0) {
        /* there weren't any real conditions -- return a null dom element */
        return QDomElement();
    } else {
        return conditions;
    }
}

void Ksp::loadConditions(Conditions *conditions, const KoXmlElement &element, const ValueParser *parser)
{
    Conditional newCondition;

    KoXmlElement conditionElement;
    forEachElement(conditionElement, element)
    {
        if (!conditionElement.hasAttribute("cond"))
            continue;

        bool ok = true;
        newCondition.cond = (Validity::Type)conditionElement.attribute("cond").toInt(&ok);
        if (!ok)
            continue;

        if (conditionElement.hasAttribute("val1")) {
            newCondition.value1 = parser->parse(conditionElement.attribute("val1"));

            if (conditionElement.hasAttribute("val2"))
                newCondition.value2 = parser->parse(conditionElement.attribute("val2"));
        }

        if (conditionElement.hasAttribute("strval1")) {
            newCondition.value1 = Value(conditionElement.attribute("strval1"));

            if (conditionElement.hasAttribute("strval2"))
                newCondition.value2 = Value(conditionElement.attribute("strval2"));
        }

        if (conditionElement.hasAttribute("style")) {
            newCondition.styleName = conditionElement.attribute("style");
        }

        conditions->addCondition(newCondition);
    }
}

} // Sheets
} // Calligra
