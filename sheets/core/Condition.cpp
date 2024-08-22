// This file is part of the KDE project
// SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
// Copyright 1999- 2006 The KSpread Team <calligra-devel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

// Local
#include "Condition.h"

#include "engine/CalculationSettings.h"
#include "engine/Formula.h"
#include "engine/NamedAreaManager.h"
#include "engine/Region.h"
#include "engine/ValueCalc.h"
#include "engine/ValueConverter.h"

#include "Cell.h"
#include "Map.h"
#include "Sheet.h"
#include "Style.h"
#include "StyleManager.h"

using namespace Calligra::Sheets;

/////////////////////////////////////////////////////////////////////////////
//
// Conditional
//
/////////////////////////////////////////////////////////////////////////////

Conditional::Conditional()
    : cond(Validity::None)
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

class Q_DECL_HIDDEN Conditions::Private : public QSharedData
{
public:
    QList<Conditional> conditionList;
    Style defaultStyle;
};

Conditions::Conditions()
    : d(new Private)
{
}

Conditions::Conditions(const Conditions &other)

    = default;

Conditions::~Conditions() = default;

bool Conditions::isEmpty() const
{
    return d->conditionList.isEmpty();
}

Style Conditions::testConditions(const Cell &cell) const
{
    Conditional condition;
    if (currentCondition(cell, condition)) {
        StyleManager *const styleManager = cell.fullSheet()->fullMap()->styleManager();
        Style *const style = styleManager->style(condition.styleName);
        if (style)
            return *style;
    }
    return d->defaultStyle;
}

bool Conditions::currentCondition(const CellBase &cell, Conditional &condition) const
{
    /* for now, the first condition that is true is the one that will be used */

    const Value value = cell.value();
    ValueCalc *const calc = cell.sheet()->map()->calc();

    QList<Conditional>::const_iterator it;
    for (it = d->conditionList.begin(); it != d->conditionList.end(); ++it) {
        condition = *it;
        //         debugSheets << "Checking condition resulting in applying" << it->styleName;

        // The first value of the condition is always used and has to be
        // comparable to the cell's value.
        if (!value.allowComparison(condition.value1)) {
            continue;
        }

        Qt::CaseSensitivity cs = calc->settings()->caseSensitiveComparisons();
        switch (condition.cond) {
        case Validity::Equal:
            if (value.equal(condition.value1, cs)) {
                return true;
            }
            break;
        case Validity::Superior:
            if (value.greater(condition.value1, cs)) {
                return true;
            }
            break;
        case Validity::Inferior:
            if (value.less(condition.value1, cs)) {
                return true;
            }
            break;
        case Validity::SuperiorEqual:
            if (value.compare(condition.value1, cs) >= 0) {
                return true;
            }
            break;
        case Validity::InferiorEqual:
            if (value.compare(condition.value1, cs) <= 0) {
                return true;
            }
            break;
        case Validity::Between: {
            const QVector<Value> values(QVector<Value>() << condition.value1 << condition.value2);
            const Value min = calc->min(values);
            const Value max = calc->max(values);
            if (value.compare(min, cs) >= 0 && value.compare(max, cs) <= 0) {
                return true;
            }
            break;
        }
        case Validity::Different: {
            const QVector<Value> values(QVector<Value>() << condition.value1 << condition.value2);
            const Value min = calc->min(values);
            const Value max = calc->max(values);
            if (value.greater(max, cs) || value.less(min, cs)) {
                return true;
            }
            break;
        }
        case Validity::DifferentTo:
            if (!value.equal(condition.value1, cs)) {
                return true;
            }
            break;
        case Validity::IsTrueFormula:
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

bool Conditions::isTrueFormula(const CellBase &cell, const QString &formula, const QString &baseCellAddress) const
{
    MapBase *const map = cell.sheet()->map();
    ValueCalc *const calc = map->calc();
    Formula f(cell.sheet(), cell);
    f.setExpression('=' + formula);
    Region r = map->regionFromName(baseCellAddress, cell.sheet());
    if (r.isValid() && r.isSingular()) {
        QPoint basePoint = static_cast<Region::Point *>(*r.constBegin())->pos();
        QString newFormula('=');
        const Tokens tokens = f.tokens();
        for (int t = 0; t < tokens.count(); ++t) {
            const Token token = tokens[t];
            if (token.type() == Token::Cell || token.type() == Token::Range) {
                if (map->namedAreaManager()->contains(token.text())) {
                    newFormula.append(token.text());
                    continue;
                }
                const Region region = map->regionFromName(token.text(), cell.sheet());
                if (!region.isValid() || !region.isContiguous()) {
                    newFormula.append(token.text());
                    continue;
                }
                if (region.firstSheet() != r.firstSheet()) {
                    newFormula.append(token.text());
                    continue;
                }
                Region::Element *element = *region.constBegin();
                if (element->type() == Region::Element::Point) {
                    Region::Point *point = static_cast<Region::Point *>(element);
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
                    Region::Range *range = static_cast<Region::Range *>(element);
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

QList<Conditional> Conditions::conditionList() const
{
    return d->conditionList;
}

void Conditions::setConditionList(const QList<Conditional> &list)
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

void Conditions::addCondition(Conditional cond)
{
    d->conditionList.append(cond);
}

void Conditions::operator=(const Conditions &other)
{
    d = other.d;
}

bool Conditions::operator==(const Conditions &other) const
{
    if (d->defaultStyle != other.d->defaultStyle)
        return false;
    if (d->conditionList.count() != other.d->conditionList.count())
        return false;
    QList<Conditional>::ConstIterator end(d->conditionList.end());
    for (QList<Conditional>::ConstIterator it(d->conditionList.begin()); it != end; ++it) {
        bool found = false;
        QList<Conditional>::ConstIterator otherEnd(other.d->conditionList.end());
        for (QList<Conditional>::ConstIterator otherIt(other.d->conditionList.begin()); otherIt != otherEnd; ++otherIt) {
            if ((*it) == (*otherIt))
                found = true;
        }
        if (!found)
            return false;
    }
    return true;
}

size_t Calligra::Sheets::qHash(const Conditions &c, size_t seed)
{
    return qHashMulti(qHash(c.defaultStyle(), seed), qHashRange(c.conditionList().cbegin(), c.conditionList().cend(), seed));
}

size_t Calligra::Sheets::qHash(const Conditional &c, size_t seed)
{
    return qHash(c.value1, seed);
}
