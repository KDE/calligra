/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   Copyright 1998, 1999 Torben Weis <weis@kde.org>
   Copyright 1999- 2006 The KSpread Team <calligra-devel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "SheetsOdf.h"
#include "SheetsOdfPrivate.h"

#include "Condition.h"
#include "Util.h"
#include "StyleManager.h"
#include "ValueConverter.h"
#include "ValueParser.h"

#include <KoGenStyles.h>
#include <KoXmlNS.h>

// This file contains functionality to load/save a Condition

namespace Calligra {
namespace Sheets {

namespace Odf {
    void loadCondition(QString &valExpression, Conditional &newCondition, const ValueParser *parser);
    void loadConditionValue(const QString &styleCondition, Conditional &newCondition, const ValueParser *parser);
    void loadValidationValue(const QStringList &listVal, Conditional &newCondition, const ValueParser *parser);
    QString saveConditionValue(const Conditional &conditionalStyle, ValueConverter *converter);
}

void Odf::saveConditions(const Conditions *conditions, KoGenStyle &currentCellStyle, ValueConverter *converter)
{
    //todo fix me with kspread old format!!!
    if (conditions->isEmpty())
        return;
    const QLinkedList<Conditional> list = conditions->conditionList();
    QLinkedList<Conditional>::const_iterator it;
    int i = 0;
    for (it = list.begin(); it != list.end(); ++it, ++i) {
        Conditional conditional = *it;
        //<style:map style:condition="cell-content()=45" style:apply-style-name="Default" style:base-cell-address="Sheet1.E10"/>
        QMap<QString, QString> map;
        map.insert("style:condition", saveConditionValue(conditional, converter));
        map.insert("style:apply-style-name", conditional.styleName);
        if (!conditional.baseCellAddress.isEmpty())
            map.insert("style:base-cell-address", conditional.baseCellAddress);
        currentCellStyle.addStyleMap(map);
    }
}

QString Odf::saveConditionValue(const Conditional &conditional, ValueConverter* converter)
{
    //we can also compare text value.
    //todo adapt it.
    QString v1 = converter->asString(conditional.value1).asStringWithDoubleQuotes();
    QString v2 = converter->asString(conditional.value2).asStringWithDoubleQuotes();
    QString value;
    switch (conditional.cond) {
    case Conditional::None:
        break;
    case Conditional::Equal:
        value = "cell-content()=" + v1;
        break;
    case Conditional::Superior:
        value = "cell-content()>" + v1;
        break;
    case Conditional::Inferior:
        value = "cell-content()<" + v1;
        break;
    case Conditional::SuperiorEqual:
        value = "cell-content()>=" + v1;
        break;
    case Conditional::InferiorEqual:
        value = "cell-content()<=" + v1;
        break;
    case Conditional::Between:
        value = "cell-content-is-between(" + v1 + ',' + v2 + ')';
        break;
    case Conditional::DifferentTo:
        value = "cell-content()!=" + v1;
        break;
    case Conditional::Different:
        value = "cell-content-is-not-between(" + v1 + ',' + v2 + ')';
        break;
    case Conditional::IsTrueFormula:
        value = "is-true-formula(" +
                Odf::encodeFormula(conditional.value1.asString()) +
                ')';
    }
    return value;
}

Conditional Odf::loadCondition(Conditions *condition, const QString &conditionValue, const QString &applyStyleName,
                                         const QString& baseCellAddress, const ValueParser *parser)
{
    //debugSheetsODF << "\tcondition:" << conditionValue;
    Conditional newCondition;
    loadConditionValue(conditionValue, newCondition, parser);
    if (!applyStyleName.isNull()) {
        //debugSheetsODF << "\tstyle:" << applyStyleName;
        newCondition.styleName = applyStyleName;
    }
    newCondition.baseCellAddress = baseCellAddress;
    condition->addCondition(newCondition);
    return newCondition;
}

void Odf::loadConditions(Conditions *condition, const KoXmlElement &element, const ValueParser *parser, const StyleManager *styleManager)
{
    debugSheetsODF << "Loading conditional styles";
    KoXmlNode node(element);

    while (!node.isNull()) {
        KoXmlElement elementItem = node.toElement();
        if (elementItem.tagName() == "map" && elementItem.namespaceURI() == KoXmlNS::style) {
            QString conditionValue = elementItem.attributeNS(KoXmlNS::style, "condition", QString());
            QString applyStyleName;
            if (elementItem.hasAttributeNS(KoXmlNS::style, "apply-style-name"))
                applyStyleName = elementItem.attributeNS(KoXmlNS::style, "apply-style-name", QString());
            if (!applyStyleName.isEmpty() && styleManager) {
                QString odfStyle = styleManager->openDocumentName(applyStyleName);
                if (!odfStyle.isEmpty()) applyStyleName = odfStyle;
            }
            QString baseCellAddress = elementItem.attributeNS(KoXmlNS::style, "base-cell-address");
            loadCondition(condition, conditionValue, applyStyleName, baseCellAddress, parser);
        }
        node = node.nextSibling();
    }
}

void Odf::loadConditionValue(const QString &styleCondition, Conditional &newCondition, const ValueParser *parser)
{
    QString val(styleCondition);
    if (val.contains("cell-content()")) {
        val.remove("cell-content()");
        loadCondition(val, newCondition, parser);
    } else if (val.contains("value()")) {
        val.remove("value()");
        loadCondition(val, newCondition, parser);
    }

    //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
    //for the moment we support just int/double value, not text/date/time :(
    if (val.contains("cell-content-is-between(")) {
        val.remove("cell-content-is-between(");
        val.remove(')');
        QStringList listVal = val.split(',', QString::SkipEmptyParts);
        loadValidationValue(listVal, newCondition, parser);
        newCondition.cond = Conditional::Between;
    } else if (val.contains("cell-content-is-not-between(")) {
        val.remove("cell-content-is-not-between(");
        val.remove(')');
        QStringList listVal = val.split(',', QString::SkipEmptyParts);
        loadValidationValue(listVal, newCondition, parser);
        newCondition.cond = Conditional::Different;
    } else if (val.startsWith(QLatin1String("is-true-formula("))) {
        val.remove(0, 16);
        if (val.endsWith(QLatin1Char(')'))) val.chop(1);
        newCondition.cond = Conditional::IsTrueFormula;
        newCondition.value1 = Value(Odf::decodeFormula(val));
    }
}

void Odf::loadCondition(QString &valExpression, Conditional &newCondition, const ValueParser *parser)
{
    QString value;
    if (valExpression.indexOf("<=") == 0) {
        value = valExpression.remove(0, 2);
        newCondition.cond = Conditional::InferiorEqual;
    } else if (valExpression.indexOf(">=") == 0) {
        value = valExpression.remove(0, 2);
        newCondition.cond = Conditional::SuperiorEqual;
    } else if (valExpression.indexOf("!=") == 0) {
        //add Differentto attribute
        value = valExpression.remove(0, 2);
        newCondition.cond = Conditional::DifferentTo;
    } else if (valExpression.indexOf('<') == 0) {
        value = valExpression.remove(0, 1);
        newCondition.cond = Conditional::Inferior;
    } else if (valExpression.indexOf('>') == 0) {
        value = valExpression.remove(0, 1);
        newCondition.cond = Conditional::Superior;
    } else if (valExpression.indexOf('=') == 0) {
        value = valExpression.remove(0, 1);
        newCondition.cond = Conditional::Equal;
    } else {
        warnSheets << " I don't know how to parse it :" << valExpression;
    }
    //debugSheetsODF << "\tvalue:" << value;

    if (value.length() > 1 && value[0] == '"' && value[value.length()-1] == '"') {
        newCondition.value1 = Value(value.mid(1, value.length()-2));
    } else {
        newCondition.value1 = parser->parse(value);
    }
}

void Odf::loadValidationValue(const QStringList &listVal, Conditional &newCondition, const ValueParser *parser)
{
    debugSheetsODF << " listVal[0] :" << listVal[0] << " listVal[1] :" << listVal[1];
    newCondition.value1 = parser->parse(listVal[0]);
    newCondition.value2 = parser->parse(listVal[1]);
}




}  // Sheets
}  // Calligra

