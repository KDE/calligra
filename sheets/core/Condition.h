// This file is part of the KDE project
// SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
// Copyright 1999- 2006 The KSpread Team <calligra-devel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef CALLIGRA_SHEETS_CONDITION_H
#define CALLIGRA_SHEETS_CONDITION_H

#include <QSharedData>
#include <QVariant>

#include "engine/Validity.h"
#include "engine/Value.h"
#include "sheets_core_export.h"

class QString;

namespace Calligra
{
namespace Sheets
{
class CellBase;
class Cell;
class Style;
class ValueConverter;
class ValueParser;

/**
 * \class Conditional
 * \ingroup Style
 * Conditional formatting.
 * Holds the actual condition and the applicable style for conditional
 * Cell formattings.
 */
class CALLIGRA_SHEETS_CORE_EXPORT Conditional
{
public:
    Value value1;
    Value value2;
    QString styleName;
    Validity::Type cond;
    QString baseCellAddress;

    Conditional();

    bool operator==(const Conditional &other) const;
};

class Conditions;
size_t qHash(const Conditions &conditions, size_t seed = 0);
size_t qHash(const Conditional &condition, size_t seed = 0);

/**
 * \class Conditions
 * \ingroup Style
 * Manages a set of conditions for a cell.
 */
class CALLIGRA_SHEETS_CORE_EXPORT Conditions
{
public:
    /**
     * Constructor.
     */
    Conditions();

    /**
     * Copy Constructor.
     */
    Conditions(const Conditions &other);

    /**
     * Destructor.
     */
    ~Conditions();

    /**
     * \return \c true if there are no conditions defined
     */
    bool isEmpty() const;

    /**
     * \return the style that matches first (or 0 if no condition matches)
     */
    Style testConditions(const Cell &cell) const;

    /**
     * Retrieve the current list of conditions we're checking
     */
    QList<Conditional> conditionList() const;

    /**
     * Replace the current list of conditions with this new one
     */
    void setConditionList(const QList<Conditional> &list);

    /**
     * Add a new condition.
     */
    void addCondition(Conditional cond);

    /**
     * Returns an optional default style, which is returned by testConditions if none of
     * the conditions matches.
     */
    Style defaultStyle() const;

    /**
     * Set an optional default style. This style is returned by testConditions if none of
     * the conditions matches.
     */
    void setDefaultStyle(const Style &style);

    /// \note implementation to make QMap happy (which is needed by RectStorage)
    bool operator<(const Conditions &conditions) const
    {
        return qHash(*this) < qHash(conditions);
    }
    void operator=(const Conditions &other);
    bool operator==(const Conditions &other) const;
    inline bool operator!=(const Conditions &other) const
    {
        return !operator==(other);
    }

private:
    /**
     * Use this function to see what conditions actually apply currently
     *
     * \param condition a reference to a condition that will be set to the
     *                  matching condition.  If none of the conditions are true
     *                  then this parameter is undefined on exit (check the
     *                  return value).
     *
     * \return true if one of the conditions is true, false if not.
     */
    bool currentCondition(const CellBase &cell, Conditional &condition) const;

    bool isTrueFormula(const CellBase &cell, const QString &formula, const QString &baseCellAddress) const;

    class Private;
    QSharedDataPointer<Private> d;
};

} // namespace Sheets
} // namespace Calligra

Q_DECLARE_METATYPE(Calligra::Sheets::Conditions)
Q_DECLARE_TYPEINFO(Calligra::Sheets::Conditions, Q_MOVABLE_TYPE);

#endif // CALLIGRA_SHEETS_CONDITION_H
