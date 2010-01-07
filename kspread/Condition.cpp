/* This file is part of the KDE project
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
#include "Sheet.h"
#include "Style.h"
#include "StyleManager.h"
#include "Util.h"

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
        : val1(0.0)
        , val2(0.0)
        , strVal1(0)
        , strVal2(0)
        , colorcond(0)
        , fontcond(0)
        , styleName(0)
        , cond(None)
{
}

Conditional::~Conditional()
{
    delete strVal1;
    delete strVal2;
    delete colorcond;
    delete fontcond;
    delete styleName;
}

Conditional::Conditional(const Conditional& o)
{
    strVal1 = o.strVal1 ? new QString(*o.strVal1) : 0;
    strVal2 = o.strVal2 ? new QString(*o.strVal2) : 0;
    styleName = o.styleName ? new QString(*o.styleName) : 0;
    fontcond = o.fontcond ? new QFont(*o.fontcond) : 0;
    colorcond = o.colorcond ? new QColor(*o.colorcond) : 0;
    val1  = o.val1;
    val2  = o.val2;
    cond  = o.cond;
}

Conditional& Conditional::operator=(const Conditional & o)
{
    delete strVal1;
    delete strVal2;
    delete styleName;
    delete fontcond;
    delete colorcond;
    strVal1 = o.strVal1 ? new QString(*o.strVal1) : 0;
    strVal2 = o.strVal2 ? new QString(*o.strVal2) : 0;
    styleName = o.styleName ? new QString(*o.styleName) : 0;
    fontcond = o.fontcond ? new QFont(*o.fontcond) : 0;
    colorcond = o.colorcond ? new QColor(*o.colorcond) : 0;
    val1  = o.val1;
    val2  = o.val2;
    cond  = o.cond;

    return *this;
}

bool Conditional::operator==(const Conditional& other) const
{
    return (cond == other.cond &&
            val1 == other.val1 &&
            val2 == other.val2 &&
            (strVal1 && other.strVal1) ? (*strVal1 == *other.strVal1) : (strVal1 == other.strVal1) &&
            (strVal2 && other.strVal2) ? (*strVal2 == *other.strVal2) : (strVal2 == other.strVal2) &&
            (colorcond && other.strVal2) ? (*colorcond == *other.colorcond) : (colorcond == other.colorcond) &&
            (fontcond && other.fontcond) ? (*fontcond == *other.fontcond) : (fontcond == other.fontcond) &&
            (styleName && other.styleName) ? (*styleName == *other.styleName) : (styleName == other.styleName) );
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

Style* Conditions::testConditions(const Cell& cell, const StyleManager* styleManager) const
{
    Conditional condition;
    if (currentCondition(cell, condition) && condition.styleName)
        return styleManager->style(*condition.styleName);
    else
        return 0;
}

bool Conditions::currentCondition(const Cell& cell, Conditional & condition) const
{
    /* for now, the first condition that is true is the one that will be used */

    QLinkedList<Conditional>::const_iterator it;
    double value   = numToDouble(cell.value().asFloat());
    QString strVal = cell.value().asString();


    for (it = d->conditionList.begin(); it != d->conditionList.end(); ++it) {
        condition = *it;

        if (condition.strVal1 && cell.value().isNumber())
            continue;

        switch (condition.cond) {
        case Conditional::Equal:
            if (condition.strVal1) {
                if (strVal == *condition.strVal1)
                    return true;
            } else
                if (value - condition.val1 < DBL_EPSILON &&
                        value - condition.val1 > (0.0 - DBL_EPSILON)) {
                    return true;
                }
            break;

        case Conditional::Superior:
            if (condition.strVal1) {
                if (strVal > *condition.strVal1)
                    return true;
            } else
                if (value > condition.val1) {
                    return true;
                }
            break;

        case Conditional::Inferior:
            if (condition.strVal1) {
                if (strVal < *condition.strVal1)
                    return true;
            } else
                if (value < condition.val1) {
                    return true;
                }
            break;

        case Conditional::SuperiorEqual :
            if (condition.strVal1) {
                if (strVal >= *condition.strVal1)
                    return true;
            } else
                if (value >= condition.val1) {
                    return true;
                }
            break;

        case Conditional::InferiorEqual :
            if (condition.strVal1) {
                if (strVal <= *condition.strVal1)
                    return true;
            } else
                if (value <= condition.val1) {
                    return true;
                }
            break;

        case Conditional::Between :
            if (condition.strVal1 && condition.strVal2) {
                if (strVal > *condition.strVal1 && strVal < *condition.strVal2)
                    return true;
            } else
                if ((value > qMin(condition.val1, condition.val2))
                        && (value < qMax(condition.val1, condition.val2))) {
                    return true;
                }
            break;

        case Conditional::Different :
            if (condition.strVal1 && condition.strVal2) {
                if (strVal < *condition.strVal1 || strVal > *condition.strVal2)
                    return true;
            } else
                if ((value < qMin(condition.val1, condition.val2))
                        || (value > qMax(condition.val1, condition.val2))) {
                    return true;
                }
            break;
        case Conditional::DifferentTo :
            if (condition.strVal1) {
                if (strVal != *condition.strVal1)
                    return true;
            } else
                if (value != condition.val1) {
                    return true;
                }
            break;

        default:
            break;
        }
    }
    return false;
}

QLinkedList<Conditional> Conditions::conditionList() const
{
    return d->conditionList;
}

void Conditions::setConditionList(const QLinkedList<Conditional> & list)
{
    d->conditionList = list;
}

void Conditions::saveOdfConditions(KoGenStyle &currentCellStyle) const
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
        map.insert("style:condition", saveOdfConditionValue(condition));
        map.insert("style:apply-style-name",  *(condition.styleName));
        //map.insert( ""style:base-cell-address", "..." );//todo
        currentCellStyle.addStyleMap(map);
    }
}

QString Conditions::saveOdfConditionValue(Conditional &condition) const
{
    //we can also compare text value.
    //todo adapt it.
    QString value;
    switch (condition.cond) {
    case Conditional::None:
        break;
    case Conditional::Equal:
        value = "cell-content()=";
        if (condition.strVal1)
            value += *condition.strVal1;
        else
            value += QString::number(condition.val1);
        break;
    case Conditional::Superior:
        value = "cell-content()>";
        if (condition.strVal1)
            value += *condition.strVal1;
        else
            value += QString::number(condition.val1);
        break;
    case Conditional::Inferior:
        value = "cell-content()<";
        if (condition.strVal1)
            value += *condition.strVal1;
        else
            value += QString::number(condition.val1);
        break;
    case Conditional::SuperiorEqual:
        value = "cell-content()>=";
        if (condition.strVal1)
            value += *condition.strVal1;
        else
            value += QString::number(condition.val1);
        break;
    case Conditional::InferiorEqual:
        value = "cell-content()<=";
        if (condition.strVal1)
            value += *condition.strVal1;
        else
            value += QString::number(condition.val1);
        break;
    case Conditional::Between:
        value = "cell-content-is-between(";
        if (condition.strVal1) {
            value += *condition.strVal1;
            value += ',';
            if (condition.strVal2)
                value += *condition.strVal2;
        } else {
            value += QString::number(condition.val1);
            value += ',';
            value += QString::number(condition.val2);
        }
        value += ')';
        break;
    case Conditional::DifferentTo:
        value = "cell-content()!="; //FIXME not good here !
        if (condition.strVal1)
            value += *condition.strVal1;
        else
            value += QString::number(condition.val1);
        break;
    case Conditional::Different:
        value = "cell-content-is-not-between(";
        if (condition.strVal1) {
            value += *condition.strVal1;
            value += ',';
            if (condition.strVal2)
                value += *condition.strVal2;
        } else {
            value += QString::number(condition.val1);
            value += ',';
            value += QString::number(condition.val2);
        }
        value += ')';
        break;
    }
    return value;
}


QDomElement Conditions::saveConditions(QDomDocument & doc) const
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
        if (condition.strVal1) {
            child.setAttribute("strval1", *condition.strVal1);
            if (condition.strVal2)
                child.setAttribute("strval2", *condition.strVal2);
        } else {
            child.setAttribute("val1", condition.val1);
            child.setAttribute("val2", condition.val2);
        }
        if (condition.styleName) {
            child.setAttribute("style", *condition.styleName);
        } else {
            child.setAttribute("color", condition.colorcond->name());
            child.appendChild(NativeFormat::createElement("font", *condition.fontcond, doc));
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

Conditional Conditions::loadOdfCondition(const StyleManager* styleManager, const QString &conditionValue, const QString &applyStyleName)
{
    kDebug(36003) << "\tcondition:" << conditionValue;
    Conditional newCondition;
    loadOdfConditionValue(conditionValue, newCondition);
    if (!applyStyleName.isNull()) {
        kDebug(36003) << "\tstyle:" << applyStyleName;
        newCondition.styleName = new QString(applyStyleName);
    }
    d->conditionList.append(newCondition);
    return newCondition;
}

void Conditions::loadOdfConditions(const StyleManager* styleManager, const KoXmlElement & element)
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
            loadOdfCondition(styleManager, conditionValue, applyStyleName);
        }
        node = node.nextSibling();
    }
}

void Conditions::loadOdfConditionValue(const QString &styleCondition, Conditional &newCondition)
{
    QString val(styleCondition);
    if (val.contains("cell-content()")) {
        val = val.remove("cell-content()");
        loadOdfCondition(val, newCondition);
    }
    else if (val.contains("value()")) {
        val = val.remove("value()");
        loadOdfCondition(val, newCondition);
    }

    //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
    //for the moment we support just int/double value, not text/date/time :(
    if (val.contains("cell-content-is-between(")) {
        val = val.remove("cell-content-is-between(");
        val = val.remove(')');
        QStringList listVal = val.split(',', QString::SkipEmptyParts);
        loadOdfValidationValue(listVal, newCondition);
        newCondition.cond = Conditional::Between;
    }
    else if (val.contains("cell-content-is-not-between(")) {
        val = val.remove("cell-content-is-not-between(");
        val = val.remove(')');
        QStringList listVal = val.split(',', QString::SkipEmptyParts);
        loadOdfValidationValue(listVal, newCondition);
        newCondition.cond = Conditional::Different;
    }
}

void Conditions::loadOdfCondition(QString &valExpression, Conditional &newCondition)
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
    kDebug(36003) << "\tvalue:" << value;
    bool ok = false;
    newCondition.val1 = value.toDouble(&ok);
    if (!ok) {
        newCondition.val1 = value.toInt(&ok);
        if (!ok) {
            newCondition.strVal1 = new QString(value);
            kDebug(36003) << " Try to parse this value :" << value;
        }
    }
}

void Conditions::loadOdfValidationValue(const QStringList &listVal, Conditional &newCondition)
{
    bool ok = false;
    kDebug(36003) << " listVal[0] :" << listVal[0] << " listVal[1] :" << listVal[1];

    newCondition.val1 = listVal[0].toDouble(&ok);
    if (!ok) {
        newCondition.val1 = listVal[0].toInt(&ok);
        if (!ok) {
            newCondition.strVal1 = new QString(listVal[0]);
            kDebug(36003) << " Try to parse this value :" << listVal[0];
        }
    }
    ok = false;
    newCondition.val2 = listVal[1].toDouble(&ok);
    if (!ok) {
        newCondition.val2 = listVal[1].toInt(&ok);
        if (!ok) {
            newCondition.strVal2 = new QString(listVal[1]);
            kDebug(36003) << " Try to parse this value :" << listVal[1];
        }
    }
}

void Conditions::loadConditions(const StyleManager* styleManager, const KoXmlElement & element)
{
    Conditional newCondition;

    KoXmlElement conditionElement;
    forEachElement(conditionElement, element) {
        newCondition.strVal1   = 0;
        newCondition.strVal2   = 0;
        newCondition.styleName = 0;
        newCondition.fontcond  = 0;
        newCondition.colorcond = 0;

        if (!conditionElement.hasAttribute("cond"))
            continue;
        
        bool ok = true;
        newCondition.cond = (Conditional::Type) conditionElement.attribute("cond").toInt(&ok);
        if(!ok)
            continue;

        if (conditionElement.hasAttribute("val1")) {
            newCondition.val1 = conditionElement.attribute("val1").toDouble(&ok);

            if (conditionElement.hasAttribute("val2"))
                newCondition.val2 = conditionElement.attribute("val2").toDouble(&ok);
        }

        if (conditionElement.hasAttribute("strval1")) {
            newCondition.strVal1 = new QString(conditionElement.attribute("strval1"));

            if (conditionElement.hasAttribute("strval2"))
                newCondition.strVal2 = new QString(conditionElement.attribute("strval2"));
        }

        if (conditionElement.hasAttribute("color")) {
            QColor color(conditionElement.attribute("color"));
            if (color.isValid())
                newCondition.colorcond = new QColor(color);
        }

        KoXmlElement font = conditionElement.namedItem("font").toElement();
        if (!font.isNull())
            newCondition.fontcond = new QFont(NativeFormat::toFont(font));

        if (conditionElement.hasAttribute("style")) {
            newCondition.styleName = new QString(conditionElement.attribute("style"));
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
