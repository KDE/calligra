/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   Copyright 1999- 2006 The KSpread Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SheetsOdf.h"
#include "SheetsOdfPrivate.h"

#include "engine/Validity.h"

#include "Condition.h"
#include "StyleManager.h"

#include <KoGenStyles.h>
#include <KoXmlNS.h>

// This file contains functionality to load/save a Condition

namespace Calligra {
namespace Sheets {

namespace Odf {
    void loadCondition(QString &valExpression, Conditional &newCondition);
    void loadConditionValue(const QString &styleCondition, Conditional &newCondition);
    void loadValidationValue(const QStringList &listVal, Conditional &newCondition);
    QString saveConditionValue(const Conditional &conditionalStyle, CalculationSettings *cs);
}

void Odf::saveConditions(const Conditions *conditions, KoGenStyle &currentCellStyle, CalculationSettings *cs)
{
    //todo fix me with kspread old format!!!
    if (conditions->isEmpty())
        return;
    const QList<Conditional> list = conditions->conditionList();
    QList<Conditional>::const_iterator it;
    int i = 0;
    for (it = list.begin(); it != list.end(); ++it, ++i) {
        Conditional conditional = *it;
        //<style:map style:condition="cell-content()=45" style:apply-style-name="Default" style:base-cell-address="Sheet1.E10"/>
        QMap<QString, QString> map;
        map.insert("style:condition", saveConditionValue(conditional, cs));
        map.insert("style:apply-style-name", conditional.styleName);
        if (!conditional.baseCellAddress.isEmpty())
            map.insert("style:base-cell-address", conditional.baseCellAddress);
        currentCellStyle.addStyleMap(map);
    }
}

QString Odf::saveConditionValue(const Conditional &conditional, CalculationSettings *cs)
{
    QString v1 = toSaveString(conditional.value1, conditional.value1.format(), cs);
    QString v2 = toSaveString(conditional.value2, conditional.value2.format(), cs);
    if (conditional.value1.isString()) v1 = Value(v1).asStringWithDoubleQuotes();
    if (conditional.value2.isString()) v2 = Value(v2).asStringWithDoubleQuotes();
    QString value;
    switch (conditional.cond) {
    case Validity::None:
        break;
    case Validity::Equal:
        value = "cell-content()=" + v1;
        break;
    case Validity::Superior:
        value = "cell-content()>" + v1;
        break;
    case Validity::Inferior:
        value = "cell-content()<" + v1;
        break;
    case Validity::SuperiorEqual:
        value = "cell-content()>=" + v1;
        break;
    case Validity::InferiorEqual:
        value = "cell-content()<=" + v1;
        break;
    case Validity::Between:
        value = "cell-content-is-between(" + v1 + ',' + v2 + ')';
        break;
    case Validity::DifferentTo:
        value = "cell-content()!=" + v1;
        break;
    case Validity::Different:
        value = "cell-content-is-not-between(" + v1 + ',' + v2 + ')';
        break;
    case Validity::IsTrueFormula:
        value = "is-true-formula(" +
                Odf::encodeFormula(conditional.value1.asString()) +
                ')';
    }
    return value;
}

Conditional Odf::loadCondition(Conditions *condition, const QString &conditionValue, const QString &applyStyleName,
                                         const QString& baseCellAddress)
{
    //debugSheetsODF << "\tcondition:" << conditionValue;
    Conditional newCondition;
    loadConditionValue(conditionValue, newCondition);
    if (!applyStyleName.isNull()) {
        //debugSheetsODF << "\tstyle:" << applyStyleName;
        newCondition.styleName = applyStyleName;
    }
    newCondition.baseCellAddress = baseCellAddress;
    condition->addCondition(newCondition);
    return newCondition;
}

void Odf::loadConditions(Conditions *condition, const KoXmlElement &element, const StyleManager *styleManager)
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
            loadCondition(condition, conditionValue, applyStyleName, baseCellAddress);
        }
        node = node.nextSibling();
    }
}

void Odf::loadConditionValue(const QString &styleCondition, Conditional &newCondition)
{
    QString val(styleCondition);
    if (val.contains("cell-content()")) {
        val.remove("cell-content()");
        loadCondition(val, newCondition);
    } else if (val.contains("value()")) {
        val.remove("value()");
        loadCondition(val, newCondition);
    }

    //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
    //for the moment we support just int/double value, not text/date/time :(
    if (val.contains("cell-content-is-between(")) {
        val.remove("cell-content-is-between(");
        val.remove(')');
        QStringList listVal = val.split(',', Qt::SkipEmptyParts);
        loadValidationValue(listVal, newCondition);
        newCondition.cond = Validity::Between;
    } else if (val.contains("cell-content-is-not-between(")) {
        val.remove("cell-content-is-not-between(");
        val.remove(')');
        QStringList listVal = val.split(',', Qt::SkipEmptyParts);
        loadValidationValue(listVal, newCondition);
        newCondition.cond = Validity::Different;
    } else if (val.startsWith(QLatin1String("is-true-formula("))) {
        val.remove(0, 16);
        if (val.endsWith(QLatin1Char(')'))) val.chop(1);
        newCondition.cond = Validity::IsTrueFormula;
        newCondition.value1 = Value(Odf::decodeFormula(val));
    }
}

void Odf::loadCondition(QString &valExpression, Conditional &newCondition)
{
    QString value;
    if (valExpression.indexOf("<=") == 0) {
        value = valExpression.remove(0, 2);
        newCondition.cond = Validity::InferiorEqual;
    } else if (valExpression.indexOf(">=") == 0) {
        value = valExpression.remove(0, 2);
        newCondition.cond = Validity::SuperiorEqual;
    } else if (valExpression.indexOf("!=") == 0) {
        //add Differentto attribute
        value = valExpression.remove(0, 2);
        newCondition.cond = Validity::DifferentTo;
    } else if (valExpression.indexOf('<') == 0) {
        value = valExpression.remove(0, 1);
        newCondition.cond = Validity::Inferior;
    } else if (valExpression.indexOf('>') == 0) {
        value = valExpression.remove(0, 1);
        newCondition.cond = Validity::Superior;
    } else if (valExpression.indexOf('=') == 0) {
        value = valExpression.remove(0, 1);
        newCondition.cond = Validity::Equal;
    } else {
        warnSheets << " I don't know how to parse it :" << valExpression;
    }
    //debugSheetsODF << "\tvalue:" << value;

    if (value.length() > 1 && value[0] == '"' && value[value.length()-1] == '"') {
        newCondition.value1 = Value(value.mid(1, value.length()-2));
    } else {
        bool ok;
        Value v = Value(value.toDouble(&ok));
        if (!ok) v = Value(value);
        newCondition.value1 = v;
    }
}

void Odf::loadValidationValue(const QStringList &listVal, Conditional &newCondition)
{
    debugSheetsODF << " listVal[0] :" << listVal[0] << " listVal[1] :" << listVal[1];
    bool ok;
    Value v1 = Value(listVal[0].toDouble(&ok));
    if (!ok) v1 = Value(listVal[0]);
    Value v2 = Value(listVal[1].toDouble(&ok));
    if (!ok) v2 = Value(listVal[1]);
    newCondition.value1 = v1;
    newCondition.value2 = v2;
}




}  // Sheets
}  // Calligra

