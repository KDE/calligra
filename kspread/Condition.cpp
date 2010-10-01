/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
   Copyright 1998, 1999 Torben Weis <weis@kde.org>
   Copyright 1999- 2006 The KSpread Team <koffice-devel@kde.org>

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

// Local
#include "Condition.h"

#include <float.h>

#include "Cell.h"
#include "Formula.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "Region.h"
#include "Sheet.h"
#include "Style.h"
#include "StyleManager.h"
#include "Util.h"
#include "ValueCalc.h"
#include "ValueConverter.h"
#include "ValueParser.h"

#include <KoGenStyles.h>

#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <kdebug.h>
#include <qdom.h>

using namespace KSpread;

/////////////////////////////////////////////////////////////////////////////
//
// Conditional
//
/////////////////////////////////////////////////////////////////////////////

Conditional::Conditional()
    : cond(None)
{
}

bool Conditional::operator==(const Conditional &other) const
{
    if (cond != other.cond) {
        return false;
    }
    if (!value1.equal(other.value1)) {
        return false;
    }
    if (!value2.equal(other.value2)) {
        return false;
    }
    return styleName == other.styleName;
}
/////////////////////////////////////////////////////////////////////////////
//
// Conditions
//
/////////////////////////////////////////////////////////////////////////////

class Conditions::Private : public QSharedData
{
public:
    QLinkedList<Conditional> conditionList;
    Style defaultStyle;
};

Conditions::Conditions()
        : d(new Private)
{
}

Conditions::Conditions(const Conditions& other)
        : d(other.d)
{
}

Conditions::~Conditions()
{
}

bool Conditions::isEmpty() const
{
    return d->conditionList.isEmpty();
}

Style Conditions::testConditions( const Cell& cell ) const
{
    Conditional condition;
    if (currentCondition(cell, condition)) {
        StyleManager *const styleManager = cell.sheet()->map()->styleManager();
        Style *const style = styleManager->style(condition.styleName);
        if (style)
            return *style;
    }
    return d->defaultStyle;
}

bool Conditions::currentCondition(const Cell& cell, Conditional & condition) const
{
    /* for now, the first condition that is true is the one that will be used */

    const Value value = cell.value();
    ValueCalc *const calc = cell.sheet()->map()->calc();

    QLinkedList<Conditional>::const_iterator it;
    for (it = d->conditionList.begin(); it != d->conditionList.end(); ++it) {
        condition = *it;
//         kDebug() << "Checking condition resulting in applying" << it->styleName;

        // The first value of the condition is always used and has to be
        // comparable to the cell's value.
        if (!value.allowComparison(condition.value1)) {
            continue;
        }

        switch (condition.cond) {
        case Conditional::Equal:
            if (value.equal(condition.value1)) {
                return true;
            }
            break;
        case Conditional::Superior:
            if (value.greater(condition.value1)) {
                return true;
            }
            break;
        case Conditional::Inferior:
            if (value.less(condition.value1)) {
                return true;
            }
            break;
        case Conditional::SuperiorEqual:
            if (value.compare(condition.value1) >= 0) {
                return true;
            }
            break;
        case Conditional::InferiorEqual:
            if (value.compare(condition.value1) <= 0) {
                return true;
            }
            break;
        case Conditional::Between: {
            const QVector<Value> values(QVector<Value>() << condition.value1 << condition.value2);
            const Value min = calc->min(values);
            const Value max = calc->max(values);
            if (value.compare(min) >= 0 && value.compare(max) <= 0) {
                return true;
            }
            break;
        }
        case Conditional::Different: {
            const QVector<Value> values(QVector<Value>() << condition.value1 << condition.value2);
            const Value min = calc->min(values);
            const Value max = calc->max(values);
            if (value.greater(max) || value.less(min)) {
                return true;
            }
            break;
        }
        case Conditional::DifferentTo:
            if (!value.equal(condition.value1)) {
                return true;
            }
            break;
        case Conditional::IsTrueFormula:
            // TODO: do some caching
            if (isTrueFormula(cell, condition.value1.asString(), condition.baseCellAddress)) {
                return true;
            }
            break;
        default:
            break;
        }
    }
    return false;
}

bool Conditions::isTrueFormula(const Cell &cell, const QString &formula, const QString &baseCellAddress) const
{
    Map* const map = cell.sheet()->map();
    ValueCalc *const calc = map->calc();
    Formula f(cell.sheet(), cell);
    f.setExpression('=' + formula);
    Region r(baseCellAddress, map, cell.sheet());
    if (r.isValid() && r.isSingular()) {
        QPoint basePoint = static_cast<Region::Point*>(*r.constBegin())->pos();
        QString newFormula('=');
        const Tokens tokens = f.tokens();
        for (int t = 0; t < tokens.count(); ++t) {
            const Token token = tokens[t];
            if (token.type() == Token::Cell || token.type() == Token::Range) {
                if (map->namedAreaManager()->contains(token.text())) {
                    newFormula.append(token.text());
                    continue;
                }
                const Region region(token.text(), map, cell.sheet());
                if (!region.isValid() || !region.isContiguous()) {
                    newFormula.append(token.text());
                    continue;
                }
                if (region.firstSheet() != r.firstSheet()) {
                    newFormula.append(token.text());
                    continue;
                }
                Region::Element* element = *region.constBegin();
                if (element->type() == Region::Element::Point) {
                    Region::Point* point = static_cast<Region::Point*>(element);
                    QPoint pos = point->pos();
                    if (!point->isRowFixed()) {
                        int delta = pos.y() - basePoint.y();
                        pos.setY(cell.row() + delta);
                    }
                    if (!point->isColumnFixed()) {
                        int delta = pos.x() - basePoint.x();
                        pos.setX(cell.column() + delta);
                    }
                    newFormula.append(Region(pos, cell.sheet()).name());
                } else {
                    Region::Range* range = static_cast<Region::Range*>(element);
                    QRect r = range->rect();
                    if (!range->isTopFixed()) {
                        int delta = r.top() - basePoint.y();
                        r.setTop(cell.row() + delta);
                    }
                    if (!range->isBottomFixed()) {
                        int delta = r.bottom() - basePoint.y();
                        r.setBottom(cell.row() + delta);
                    }
                    if (!range->isLeftFixed()) {
                        int delta = r.left() - basePoint.x();
                        r.setLeft(cell.column() + delta);
                    }
                    if (!range->isRightFixed()) {
                        int delta = r.right() - basePoint.x();
                        r.setRight(cell.column() + delta);
                    }
                    newFormula.append(Region(r, cell.sheet()).name());
                }
            } else {
                newFormula.append(token.text());
            }
        }
        f.setExpression(newFormula);
    }
    Value val = f.eval();
    return calc->conv()->asBoolean(val).asBoolean();
}

QLinkedList<Conditional> Conditions::conditionList() const
{
    return d->conditionList;
}

void Conditions::setConditionList(const QLinkedList<Conditional> & list)
{
    d->conditionList = list;
}

Style Conditions::defaultStyle() const
{
    return d->defaultStyle;
}

void Conditions::setDefaultStyle(const Style &style)
{
    d->defaultStyle = style;
}

void Conditions::saveOdfConditions(KoGenStyle &currentCellStyle, ValueConverter *converter) const
{
    //todo fix me with kspread old format!!!
    if (d->conditionList.isEmpty())
        return;
    QLinkedList<Conditional>::const_iterator it;
    int i = 0;
    for (it = d->conditionList.begin(); it != d->conditionList.end(); ++it, ++i) {
        Conditional condition = *it;
        //<style:map style:condition="cell-content()=45" style:apply-style-name="Default" style:base-cell-address="Sheet1.E10"/>
        QMap<QString, QString> map;
        map.insert("style:condition", saveOdfConditionValue(condition, converter));
        map.insert("style:apply-style-name", condition.styleName);
        if (!condition.baseCellAddress.isEmpty())
            map.insert("style:base-cell-address", condition.baseCellAddress);
        currentCellStyle.addStyleMap(map);
    }
}

QString Conditions::saveOdfConditionValue(const Conditional &condition, ValueConverter* converter) const
{
    //we can also compare text value.
    //todo adapt it.
    QString value;
    switch (condition.cond) {
    case Conditional::None:
        break;
    case Conditional::Equal:
        value = "cell-content()=" + converter->asString(condition.value1).asString();
        break;
    case Conditional::Superior:
        value = "cell-content()>" + converter->asString(condition.value1).asString();
        break;
    case Conditional::Inferior:
        value = "cell-content()<" + converter->asString(condition.value1).asString();
        break;
    case Conditional::SuperiorEqual:
        value = "cell-content()>=" + converter->asString(condition.value1).asString();
        break;
    case Conditional::InferiorEqual:
        value = "cell-content()<=" + converter->asString(condition.value1).asString();
        break;
    case Conditional::Between:
        value = "cell-content-is-between(";
        value += converter->asString(condition.value1).asString();
        value += ',';
        value += converter->asString(condition.value2).asString();
        value += ')';
        break;
    case Conditional::DifferentTo:
        value = "cell-content()!=" + converter->asString(condition.value1).asString();
        break;
    case Conditional::Different:
        value = "cell-content-is-not-between(";
        value += converter->asString(condition.value1).asString();
        value += ',';
        value += converter->asString(condition.value2).asString();
        value += ')';
        break;
    case Conditional::IsTrueFormula:
        value = "is-true-formula(";
        value += Odf::encodeFormula(condition.value1.asString());
        value += ")";
    }
    return value;
}


QDomElement Conditions::saveConditions(QDomDocument &doc, ValueConverter *converter) const
{
    QDomElement conditions = doc.createElement("condition");
    QLinkedList<Conditional>::const_iterator it;
    QDomElement child;
    int num = 0;
    QString name;

    for (it = d->conditionList.begin(); it != d->conditionList.end(); ++it) {
        Conditional condition = *it;

        /* the name of the element will be "condition<n>"
            * This is unimportant now but in older versions three conditions were
            * hardcoded with names "first" "second" and "third"
        */
        name.setNum(num);
        name.prepend("condition");

        child = doc.createElement(name);
        child.setAttribute("cond", (int) condition.cond);

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

Conditional Conditions::loadOdfCondition(const QString &conditionValue, const QString &applyStyleName,
                                         const QString& baseCellAddress, const ValueParser *parser)
{
    //kDebug(36003) << "\tcondition:" << conditionValue;
    Conditional newCondition;
    loadOdfConditionValue(conditionValue, newCondition, parser);
    if (!applyStyleName.isNull()) {
        //kDebug(36003) << "\tstyle:" << applyStyleName;
        newCondition.styleName = applyStyleName;
    }
    newCondition.baseCellAddress = baseCellAddress;
    d->conditionList.append(newCondition);
    return newCondition;
}

void Conditions::loadOdfConditions(const KoXmlElement &element, const ValueParser *parser, const StyleManager *styleManager)
{
    kDebug(36003) << "Loading conditional styles";
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
            loadOdfCondition(conditionValue, applyStyleName, baseCellAddress, parser);
        }
        node = node.nextSibling();
    }
}

void Conditions::loadOdfConditionValue(const QString &styleCondition, Conditional &newCondition, const ValueParser *parser)
{
    QString val(styleCondition);
    if (val.contains("cell-content()")) {
        val = val.remove("cell-content()");
        loadOdfCondition(val, newCondition, parser);
    } else if (val.contains("value()")) {
        val = val.remove("value()");
        loadOdfCondition(val, newCondition, parser);
    }

    //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
    //for the moment we support just int/double value, not text/date/time :(
    if (val.contains("cell-content-is-between(")) {
        val = val.remove("cell-content-is-between(");
        val = val.remove(')');
        QStringList listVal = val.split(',', QString::SkipEmptyParts);
        loadOdfValidationValue(listVal, newCondition, parser);
        newCondition.cond = Conditional::Between;
    } else if (val.contains("cell-content-is-not-between(")) {
        val = val.remove("cell-content-is-not-between(");
        val = val.remove(')');
        QStringList listVal = val.split(',', QString::SkipEmptyParts);
        loadOdfValidationValue(listVal, newCondition, parser);
        newCondition.cond = Conditional::Different;
    } else if (val.startsWith("is-true-formula(")) {
        val = val.mid(16);
        if (val.endsWith(")")) val = val.left(val.length() - 1);
        newCondition.cond = Conditional::IsTrueFormula;
        newCondition.value1 = Value(Odf::decodeFormula(val));
    }
}

void Conditions::loadOdfCondition(QString &valExpression, Conditional &newCondition, const ValueParser *parser)
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
    } else
        kDebug(36003) << " I don't know how to parse it :" << valExpression;
    //kDebug(36003) << "\tvalue:" << value;

    if (value.length() > 1 && value[0] == '"' && value[value.length()-1] == '"') {
        newCondition.value1 = Value(value.mid(1, value.length()-2));
    } else {
        newCondition.value1 = parser->parse(value);
    }
}

void Conditions::loadOdfValidationValue(const QStringList &listVal, Conditional &newCondition, const ValueParser *parser)
{
    kDebug(36003) << " listVal[0] :" << listVal[0] << " listVal[1] :" << listVal[1];
    newCondition.value1 = parser->parse(listVal[0]);
    newCondition.value2 = parser->parse(listVal[1]);
}

void Conditions::loadConditions(const KoXmlElement &element, const ValueParser *parser)
{
    Conditional newCondition;

    KoXmlElement conditionElement;
    forEachElement(conditionElement, element) {
        if (!conditionElement.hasAttribute("cond"))
            continue;

        bool ok = true;
        newCondition.cond = (Conditional::Type) conditionElement.attribute("cond").toInt(&ok);
        if(!ok)
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

        d->conditionList.append(newCondition);
    }
}

void Conditions::operator=(const Conditions & other)
{
    d = other.d;
}

bool Conditions::operator==(const Conditions& other) const
{
    if (d->conditionList.count() != other.d->conditionList.count())
        return false;
    QLinkedList<Conditional>::ConstIterator end(d->conditionList.end());
    for (QLinkedList<Conditional>::ConstIterator it(d->conditionList.begin()); it != end; ++it) {
        bool found = false;
        QLinkedList<Conditional>::ConstIterator otherEnd(other.d->conditionList.end());
        for (QLinkedList<Conditional>::ConstIterator otherIt(other.d->conditionList.begin()); otherIt != otherEnd; ++otherIt) {
            if ((*it) == (*otherIt))
                found = true;
        }
        if (!found)
            return false;
    }
    return true;
}

uint KSpread::qHash(const Conditions &c)
{
    uint res = 0;
    foreach (const Conditional& co, c.conditionList()) {
        res ^= qHash(co);
    }
    return res;
}

uint KSpread::qHash(const Conditional& c)
{
    return static_cast<uint>(c.cond) ^ qHash(c.value1) ^ qHash(c.value2) ^ qHash(c.styleName);
}
