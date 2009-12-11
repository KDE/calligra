/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "Filter.h"

#include <QList>
#include <QRect>

#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "CellStorage.h"
#include "Database.h"
#include "Map.h"
#include "Region.h"
#include "Sheet.h"
#include "Value.h"
#include "ValueConverter.h"

using namespace KSpread;

class AbstractCondition
{
public:
    virtual ~AbstractCondition() {}
    enum Type { And, Or, Condition };
    virtual Type type() const = 0;
    virtual bool loadOdf(const KoXmlElement& element) = 0;
    virtual void saveOdf(KoXmlWriter& xmlWriter) = 0;
    virtual bool evaluate(const Database& database, int index) const = 0;
    virtual bool isEmpty() const = 0;
    virtual QHash<QString, Filter::Comparison> conditions(int fieldNumber) const = 0;
    virtual void removeConditions(int fieldNumber) = 0;
    virtual QString dump() const = 0;
};

/**
 * OpenDocument, 8.7.2 Filter And
 */
class Filter::And : public AbstractCondition
{
public:
    And() {}
    And(const And& other);
    virtual ~And() {
        qDeleteAll(list);
    }
    virtual Type type() const {
        return AbstractCondition::And;
    }
    virtual bool loadOdf(const KoXmlElement& parent);
    virtual void saveOdf(KoXmlWriter& xmlWriter) {
        if (!list.count())
            return;
        xmlWriter.startElement("table:filter-and");
        for (int i = 0; i < list.count(); ++i)
            list[i]->saveOdf(xmlWriter);
        xmlWriter.endElement();
    }
    virtual bool evaluate(const Database& database, int index) const {
        for (int i = 0; i < list.count(); ++i) {
            // lazy evaluation, stop on first false
            if (!list[i]->evaluate(database, index))
                return false;
        }
        return true;
    }
    virtual bool isEmpty() const {
        return list.isEmpty();
    }
    virtual QHash<QString, Filter::Comparison> conditions(int fieldNumber) const {
        QHash<QString, Filter::Comparison> result;
        for (int i = 0; i < list.count(); ++i)
            result.unite(list[i]->conditions(fieldNumber));
        return result;
    }
    virtual void removeConditions(int fieldNumber) {
        QList<AbstractCondition*> newList;
        for (int i = 0; i < list.count(); ++i) {
            list[i]->removeConditions(fieldNumber);
            if (!list[i]->isEmpty())
                newList.append(list[i]);
            else
                delete list[i];
        }
        list = newList;
    }
    bool operator!=(const And& other) const {
        return list != other.list;
    }
    virtual QString dump() const {
        QString result = "\t";
        for (int i = 0; i < list.count(); ++i) {
            if (i)
                result += "AND\t";
            result += list[i]->dump();
        }
        return result;
    }

public:
    QList<AbstractCondition*> list; // allowed: Or or Condition
};

/**
 * OpenDocument, 8.7.3 Filter Or
 */
class Filter::Or : public AbstractCondition
{
public:
    Or() {}
    Or(const Or& other);
    virtual ~Or() {
        qDeleteAll(list);
    }
    virtual Type type() const {
        return AbstractCondition::Or;
    }
    virtual bool loadOdf(const KoXmlElement& element);
    virtual void saveOdf(KoXmlWriter& xmlWriter) {
        if (!list.count())
            return;
        xmlWriter.startElement("table:filter-or");
        for (int i = 0; i < list.count(); ++i)
            list[i]->saveOdf(xmlWriter);
        xmlWriter.endElement();
    }
    virtual bool evaluate(const Database& database, int index) const {
        for (int i = 0; i < list.count(); ++i) {
            // lazy evaluation, stop on first true
            if (list[i]->evaluate(database, index))
                return true;
        }
        return false;
    }
    virtual bool isEmpty() const {
        return list.isEmpty();
    }
    virtual QHash<QString, Filter::Comparison> conditions(int fieldNumber) const {
        QHash<QString, Filter::Comparison> result;
        for (int i = 0; i < list.count(); ++i)
            result.unite(list[i]->conditions(fieldNumber));
        return result;
    }
    virtual void removeConditions(int fieldNumber) {
        QList<AbstractCondition*> newList;
        for (int i = 0; i < list.count(); ++i) {
            list[i]->removeConditions(fieldNumber);
            if (!list[i]->isEmpty())
                newList.append(list[i]);
            else
                delete list[i];
        }
        list = newList;
    }
    bool operator!=(const Or& other) const {
        return list != other.list;
    }
    virtual QString dump() const {
        QString result = "\t";
        for (int i = 0; i < list.count(); ++i) {
            if (i)
                result += "OR\t";
            result += list[i]->dump();
        }
        return result;
    }

public:
    QList<AbstractCondition*> list; // allowed: And or Condition
};

/**
 * OpenDocument, 8.7.4 Filter Condition
 */
class Filter::Condition : public AbstractCondition
{
public:
    Condition()
            : fieldNumber(-1)
            , operation(Match)
            , caseSensitivity(Qt::CaseInsensitive)
            , dataType(Text) {
    }
    Condition(int _fieldNumber, Comparison _comparison, const QString& _value,
              Qt::CaseSensitivity _caseSensitivity, Mode _mode)
            : fieldNumber(_fieldNumber)
            , value(_value)
            , operation(_comparison)
            , caseSensitivity(_caseSensitivity)
            , dataType(_mode) {
    }
    Condition(const Condition& other)
            : AbstractCondition()
            , fieldNumber(other.fieldNumber)
            , value(other.value)
            , operation(other.operation)
            , caseSensitivity(other.caseSensitivity)
            , dataType(other.dataType) {
    }
    virtual ~Condition() {}

    virtual Type type() const {
        return AbstractCondition::Condition;
    }
    virtual bool loadOdf(const KoXmlElement& element) {
        if (element.hasAttributeNS(KoXmlNS::table, "field-number")) {
            bool ok = false;
            fieldNumber = element.attributeNS(KoXmlNS::table, "field-number", QString()).toInt(&ok);
            if (!ok || fieldNumber < 0)
                return false;
        }
        if (element.hasAttributeNS(KoXmlNS::table, "value"))
            value = element.attributeNS(KoXmlNS::table, "value", QString());
        if (element.hasAttributeNS(KoXmlNS::table, "operator")) {
            const QString string = element.attributeNS(KoXmlNS::table, "operator", QString());
            if (string == "match")
                operation = Match;
            else if (string == "!match")
                operation = NotMatch;
            else if (string == "=")
                operation = Equal;
            else if (string == "!=")
                operation = NotEqual;
            else if (string == "<")
                operation = Less;
            else if (string == ">")
                operation = Greater;
            else if (string == "<=")
                operation = LessOrEqual;
            else if (string == ">=")
                operation = GreaterOrEqual;
            else if (string == "empty")
                operation = Empty;
            else if (string == "!empty")
                operation = NotEmpty;
            else if (string == "top values")
                operation = TopValues;
            else if (string == "bottom values")
                operation = BottomValues;
            else if (string == "top percent")
                operation = TopPercent;
            else if (string == "bottom percent")
                operation = BottomPercent;
            else {
                kDebug() << "table:operator: unknown value";
                return false;
            }
        }
        if (element.hasAttributeNS(KoXmlNS::table, "case-sensitive")) {
            if (element.attributeNS(KoXmlNS::table, "case-sensitive", "false") == "true")
                caseSensitivity = Qt::CaseSensitive;
            else
                caseSensitivity = Qt::CaseInsensitive;
        }
        if (element.hasAttributeNS(KoXmlNS::table, "data-type")) {
            if (element.attributeNS(KoXmlNS::table, "data-type", "text") == "number")
                dataType = Number;
            else
                dataType = Text;
        }
        return true;
    }
    virtual void saveOdf(KoXmlWriter& xmlWriter) {
        if (fieldNumber < 0)
            return;
        xmlWriter.startElement("table:filter-condition");
        xmlWriter.addAttribute("table:field-number", fieldNumber);
        xmlWriter.addAttribute("table:value", value);
        switch (operation) {
        case Match:
            xmlWriter.addAttribute("table:operator", "match");
            break;
        case NotMatch:
            xmlWriter.addAttribute("table:operator", "!match");
            break;
        case Equal:
            xmlWriter.addAttribute("table:operator", "=");
            break;
        case NotEqual:
            xmlWriter.addAttribute("table:operator", "!=");
            break;
        case Less:
            xmlWriter.addAttribute("table:operator", "<");
            break;
        case Greater:
            xmlWriter.addAttribute("table:operator", ">");
            break;
        case LessOrEqual:
            xmlWriter.addAttribute("table:operator", "<=");
            break;
        case GreaterOrEqual:
            xmlWriter.addAttribute("table:operator", ">=");
            break;
        case Empty:
            xmlWriter.addAttribute("table:operator", "empty");
            break;
        case NotEmpty:
            xmlWriter.addAttribute("table:operator", "!empty");
            break;
        case TopValues:
            xmlWriter.addAttribute("table:operator", "top values");
            break;
        case BottomValues:
            xmlWriter.addAttribute("table:operator", "bottom values");
            break;
        case TopPercent:
            xmlWriter.addAttribute("table:operator", "top percent");
            break;
        case BottomPercent:
            xmlWriter.addAttribute("table:operator", "bottom percent");
            break;
        }
        if (caseSensitivity == Qt::CaseSensitive)
            xmlWriter.addAttribute("table:case-sensitive", true);
        if (dataType == Number)
            xmlWriter.addAttribute("table:data-type", "number");
        xmlWriter.endElement();
    }
    virtual bool evaluate(const Database& database, int index) const {
        const Sheet* sheet = database.range().lastSheet();
        const QRect range = database.range().lastRange();
        const int start = database.orientation() == Qt::Vertical ? range.left() : range.top();
//         kDebug() <<"index:" << index <<" start:" << start <<" fieldNumber:" << fieldNumber;
        const Value value = database.orientation() == Qt::Vertical
                            ? sheet->cellStorage()->value(start + fieldNumber, index)
                            : sheet->cellStorage()->value(index, start + fieldNumber);
        const QString testString = sheet->map()->converter()->asString(value).asString();
        switch (operation) {
        case Match: {
            const bool result = QString::compare(this->value, testString, caseSensitivity) == 0;
//                 kDebug() <<"Match" << this->value <<"?" << testString <<"" << result;
            if (result)
                return true;
            break;
        }
        case NotMatch: {
            const bool result = QString::compare(this->value, testString, caseSensitivity) != 0;
//                 kDebug() <<"Not Match" << this->value <<"?" << testString <<"" << result;
            if (result)
                return true;
            break;
        }
        default:
            break;
        }
        return false;
    }
    virtual bool isEmpty() const {
        return fieldNumber == -1;
    }
    virtual QHash<QString, Filter::Comparison> conditions(int fieldNumber) const {
        QHash<QString, Filter::Comparison> result;
        if (this->fieldNumber == fieldNumber)
            result.insert(value, operation);
        return result;
    }
    virtual void removeConditions(int fieldNumber) {
        if (this->fieldNumber == fieldNumber) {
//             kDebug() <<"removing condition for fieldNumber" << fieldNumber;
            this->fieldNumber = -1;
        }
    }
    bool operator!=(const Condition& other) const {
        if (fieldNumber == other.fieldNumber)
            return false;
        if (value == other.value)
            return false;
        if (operation == other.operation)
            return false;
        if (caseSensitivity == other.caseSensitivity)
            return false;
        if (dataType == other.dataType)
            return false;
        return true;
    }
    virtual QString dump() const {
        QString result = QString("fieldNumber: %1 ").arg(fieldNumber);
        switch (operation) {
        case Match:     result += "Match"; break;
        case NotMatch:  result += "Not Match"; break;
        default:        break;
        }
        return result + " value: " + value + '\n';
    }

public:
    int fieldNumber;
    QString value; // Value?
    Comparison operation;
    Qt::CaseSensitivity caseSensitivity;
    Mode dataType;
};

Filter::And::And(const And& other)
        : AbstractCondition()
{
    for (int i = 0; i < other.list.count(); ++i) {
        if (!other.list[i])
            continue;
        else if (other.list[i]->type() == AbstractCondition::And)
            continue;
        else if (other.list[i]->type() == AbstractCondition::Or)
            list.append(new Filter::Or(*static_cast<Filter::Or*>(other.list[i])));
        else
            list.append(new Filter::Condition(*static_cast<Filter::Condition*>(other.list[i])));
    }
}

bool Filter::And::loadOdf(const KoXmlElement& parent)
{
    KoXmlElement element;
    AbstractCondition* condition;
    forEachElement(element, parent) {
        if (element.namespaceURI() != KoXmlNS::table)
            continue;
        if (element.localName() == "filter-or")
            condition = new Filter::Or();
        else if (element.localName() == "filter-condition")
            condition = new Filter::Condition();
        else
            continue;
        if (condition->loadOdf(element))
            list.append(condition);
        else
            delete condition;
    }
    return !list.isEmpty();
}

Filter::Or::Or(const Or& other)
        : AbstractCondition()
{
    for (int i = 0; i < other.list.count(); ++i) {
        if (!other.list[i])
            continue;
        else if (other.list[i]->type() == AbstractCondition::And)
            list.append(new Filter::And(*static_cast<Filter::And*>(other.list[i])));
        else if (other.list[i]->type() == AbstractCondition::Or)
            continue;
        else
            list.append(new Filter::Condition(*static_cast<Filter::Condition*>(other.list[i])));
    }
}

bool Filter::Or::loadOdf(const KoXmlElement& parent)
{
    KoXmlElement element;
    AbstractCondition* condition;
    forEachElement(element, parent) {
        if (element.namespaceURI() != KoXmlNS::table)
            continue;
        if (element.localName() == "filter-and")
            condition = new Filter::And();
        else if (element.localName() == "filter-condition")
            condition = new Filter::Condition();
        else
            continue;
        if (condition->loadOdf(element))
            list.append(condition);
        else
            delete condition;
    }
    return !list.isEmpty();
}


class Filter::Private
{
public:
    Private()
            : condition(0)
            , conditionSource(Self)
            , displayDuplicates(true) {
    }

    AbstractCondition* condition;
    Region targetRangeAddress;
    enum { Self, CellRange } conditionSource;
    Region conditionSourceRangeAddress;
    bool displayDuplicates;
};

Filter::Filter()
        : d(new Private)
{
}

Filter::Filter(const Filter& other)
        : d(new Private)
{
    if (!other.d->condition)
        d->condition = 0;
    else if (other.d->condition->type() == AbstractCondition::And)
        d->condition = new And(*static_cast<And*>(other.d->condition));
    else if (other.d->condition->type() == AbstractCondition::Or)
        d->condition = new Or(*static_cast<Or*>(other.d->condition));
    else
        d->condition = new Condition(*static_cast<Condition*>(other.d->condition));
    d->targetRangeAddress = other.d->targetRangeAddress;
    d->conditionSource = other.d->conditionSource;
    d->conditionSourceRangeAddress = other.d->conditionSourceRangeAddress;
    d->displayDuplicates = other.d->displayDuplicates;
}

Filter::~Filter()
{
    delete d->condition;
    delete d;
}

void Filter::addCondition(Composition composition,
                          int fieldNumber, Comparison comparison, const QString& value,
                          Qt::CaseSensitivity caseSensitivity, Mode mode)
{
    Condition* condition = new Condition(fieldNumber, comparison, value, caseSensitivity, mode);
    if (!d->condition) {
        d->condition = condition;
    } else if (composition == AndComposition) {
        if (d->condition->type() == AbstractCondition::And) {
            static_cast<And*>(d->condition)->list.append(condition);
        } else {
            And* andComposition = new And();
            andComposition->list.append(d->condition);
            andComposition->list.append(condition);
            d->condition = andComposition;
        }
    } else { // composition == OrComposition
        if (d->condition->type() == AbstractCondition::Or) {
            static_cast<Or*>(d->condition)->list.append(condition);
        } else {
            Or* orComposition = new Or();
            orComposition->list.append(d->condition);
            orComposition->list.append(condition);
            d->condition = orComposition;
        }
    }
}

void Filter::addSubFilter(Composition composition, const Filter& filter)
{
    if (!d->condition) {
        if (!filter.d->condition)
            d->condition = 0;
        else if (filter.d->condition->type() == AbstractCondition::And)
            d->condition = new And(*static_cast<And*>(filter.d->condition));
        else if (filter.d->condition->type() == AbstractCondition::Or)
            d->condition = new Or(*static_cast<Or*>(filter.d->condition));
        else // if (filter.d->condition->type() == AbstractCondition::Condition)
            d->condition = new Condition(*static_cast<Condition*>(filter.d->condition));
    } else if (composition == AndComposition) {
        if (filter.d->condition && d->condition->type() == AbstractCondition::And) {
            if (filter.d->condition->type() == AbstractCondition::And)
                static_cast<And*>(d->condition)->list += static_cast<And*>(filter.d->condition)->list;
            else if (filter.d->condition->type() == AbstractCondition::Or)
                static_cast<And*>(d->condition)->list.append(new Or(*static_cast<Or*>(filter.d->condition)));
            else // if (filter.d->condition->type() == AbstractCondition::Condition)
                static_cast<And*>(d->condition)->list.append(new Condition(*static_cast<Condition*>(filter.d->condition)));
        } else if (filter.d->condition) {
            And* andComposition = new And();
            andComposition->list.append(d->condition);
            if (filter.d->condition->type() == AbstractCondition::And)
                andComposition->list += static_cast<And*>(filter.d->condition)->list;
            else if (filter.d->condition->type() == AbstractCondition::Or)
                andComposition->list.append(new Or(*static_cast<Or*>(filter.d->condition)));
            else // if (filter.d->condition->type() == AbstractCondition::Condition)
                andComposition->list.append(new Condition(*static_cast<Condition*>(filter.d->condition)));
            d->condition = andComposition;
        }
    } else { // composition == OrComposition
        if (filter.d->condition && d->condition->type() == AbstractCondition::Or) {
            if (filter.d->condition->type() == AbstractCondition::And)
                static_cast<Or*>(d->condition)->list.append(new And(*static_cast<And*>(filter.d->condition)));
            else if (filter.d->condition->type() == AbstractCondition::Or)
                static_cast<Or*>(d->condition)->list += static_cast<Or*>(filter.d->condition)->list;
            else // if (filter.d->condition->type() == AbstractCondition::Condition)
                static_cast<Or*>(d->condition)->list.append(new Condition(*static_cast<Condition*>(filter.d->condition)));
        } else if (filter.d->condition) {
            Or* orComposition = new Or();
            orComposition->list.append(d->condition);
            if (filter.d->condition->type() == AbstractCondition::And)
                orComposition->list.append(new And(*static_cast<And*>(filter.d->condition)));
            else if (filter.d->condition->type() == AbstractCondition::Or)
                orComposition->list += static_cast<Or*>(filter.d->condition)->list;
            else // if (filter.d->condition->type() == AbstractCondition::Condition)
                orComposition->list.append(new Condition(*static_cast<Condition*>(filter.d->condition)));
            d->condition = orComposition;
        }
    }
}

QHash<QString, Filter::Comparison> Filter::conditions(int fieldNumber) const
{
    return d->condition ? d->condition->conditions(fieldNumber) : QHash<QString, Comparison>();
}

void Filter::removeConditions(int fieldNumber)
{
    if (fieldNumber == -1) {
//         kDebug() <<"removing all conditions";
        delete d->condition;
        d->condition = 0;
        return;
    }
    if (!d->condition)
        return;
    d->condition->removeConditions(fieldNumber);
    if (d->condition->isEmpty()) {
        delete d->condition;
        d->condition = 0;
    }
}

bool Filter::isEmpty() const
{
    return d->condition ? d->condition->isEmpty() : true;
}

bool Filter::evaluate(const Database& database, int index) const
{
    return d->condition ? d->condition->evaluate(database, index) : true;
}

bool Filter::loadOdf(const KoXmlElement& element, const Map* map)
{
    if (element.hasAttributeNS(KoXmlNS::table, "target-range-address")) {
        const QString address = element.attributeNS(KoXmlNS::table, "target-range-address", QString());
        // only absolute addresses allowed; no fallback sheet needed
        d->targetRangeAddress = Region(Region::loadOdf(address), map);
        if (!d->targetRangeAddress.isValid())
            return false;
    }
    if (element.hasAttributeNS(KoXmlNS::table, "condition-source")) {
        if (element.attributeNS(KoXmlNS::table, "condition-source", "self") == "cell-range")
            d->conditionSource = Private::CellRange;
        else
            d->conditionSource = Private::Self;
    }
    if (element.hasAttributeNS(KoXmlNS::table, "condition-source-range-address")) {
        const QString address = element.attributeNS(KoXmlNS::table, "condition-source-range-address", QString());
        // only absolute addresses allowed; no fallback sheet needed
        d->conditionSourceRangeAddress = Region(Region::loadOdf(address), map);
    }
    if (element.hasAttributeNS(KoXmlNS::table, "display-duplicates")) {
        if (element.attributeNS(KoXmlNS::table, "display-duplicates", "true") == "false")
            d->displayDuplicates = false;
        else
            d->displayDuplicates = true;
    }
    KoXmlElement conditionElement;
    forEachElement(conditionElement, element) {
        if (conditionElement.localName() == "filter-and") {
            d->condition = new And();
            break;
        } else if (conditionElement.localName() == "filter-or") {
            d->condition = new Or();
            break;
        } else if (conditionElement.localName() == "filter-condition") {
            d->condition = new Condition();
            break;
        }
    }
    if (!d->condition)
        return false;
    if (!d->condition->loadOdf(conditionElement.toElement())) {
        delete d->condition;
        d->condition = 0;
        return false;
    }
    return true;
}

void Filter::saveOdf(KoXmlWriter& xmlWriter) const
{
    if (!d->condition)
        return;
    xmlWriter.startElement("table:filter");
    if (!d->targetRangeAddress.isEmpty())
        xmlWriter.addAttribute("table:target-range-address", d->targetRangeAddress.saveOdf());
    if (d->conditionSource != Private::Self)
        xmlWriter.addAttribute("table:condition-source", "cell-range");
    if (!d->conditionSourceRangeAddress.isEmpty())
        xmlWriter.addAttribute("table:condition-source-range-address", d->conditionSourceRangeAddress.saveOdf());
    if (!d->displayDuplicates)
        xmlWriter.addAttribute("table:display-duplicates", "false");
    d->condition->saveOdf(xmlWriter);
    xmlWriter.endElement();
}

bool Filter::operator==(const Filter& other) const
{
    if (d->targetRangeAddress != d->targetRangeAddress)
        return false;
    if (d->conditionSource != other.d->conditionSource)
        return false;
    if (d->conditionSourceRangeAddress != other.d->conditionSourceRangeAddress)
        return false;
    if (d->displayDuplicates != other.d->displayDuplicates)
        return false;
    if (!d->condition || !other.d->condition)
        return d->condition == other.d->condition;
    if (d->condition->type() != other.d->condition->type())
        return false;
    if (d->condition->type() == AbstractCondition::And &&
            *static_cast<And*>(d->condition) != *static_cast<And*>(other.d->condition))
        return false;
    if (d->condition->type() == AbstractCondition::Or &&
            *static_cast<Or*>(d->condition) != *static_cast<Or*>(other.d->condition))
        return false;
    if (d->condition->type() == AbstractCondition::Condition &&
            *static_cast<Condition*>(d->condition) != *static_cast<Condition*>(other.d->condition))
        return false;
    return true;
}

void Filter::dump() const
{
    if (d->condition)
        kDebug() << "Condition:" + d->condition->dump();
    else
        kDebug() << "Condition: 0";
}
