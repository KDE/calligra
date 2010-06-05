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

#ifndef KSPREAD_CONDITION_H
#define KSPREAD_CONDITION_H

#include <QDomElement>
#include <QLinkedList>
#include <QSharedData>
#include <QVariant>

#include "kspread_export.h"
#include <KoXmlReader.h>

class QColor;
class QDomDocument;
class QFont;
class QString;
class KoGenStyle;

namespace KSpread
{
class Cell;
class Style;
class StyleManager;

/**
 * \class Conditional
 * \ingroup Style
 * Conditional formatting.
 * Holds the actual condition and the applicable style for conditional
 * Cell formattings.
 */
class KSPREAD_EXPORT Conditional
{
public:
    enum Type { None, Equal, Superior, Inferior, SuperiorEqual,
                InferiorEqual, Between, Different, DifferentTo
              };

    double         val1;
    double         val2;
    QString *      strVal1;
    QString *      strVal2;
    QColor  *      colorcond;
    QFont   *      fontcond;
    QString *      styleName;
    Type           cond;

    Conditional();
    ~Conditional();
    Conditional(const Conditional&);
    Conditional& operator=(const Conditional&);
    bool operator==(const Conditional& other) const;
    inline bool operator!=(const Conditional& other) const {
        return !operator==(other);
    }
};


/**
 * \class Conditions
 * \ingroup Style
 * Manages a set of conditions for a cell.
 */
class KSPREAD_EXPORT Conditions
{
public:
    /**
     * Constructor.
     */
    Conditions();

    /**
     * Copy Constructor.
     */
    Conditions(const Conditions& other);

    /**
     * Destructor..
     */
    ~Conditions();

    /**
     * \return \c true if there are no conditions defined
     */
    bool isEmpty() const;

    /**
     * \return the style that matches first (or 0 if no condition matches)
     */
    Style* testConditions(const Cell& cell, const StyleManager* styleManager) const;

    /**
     * Retrieve the current list of conditions we're checking
     */
    QLinkedList<Conditional> conditionList() const;

    /**
     * Replace the current list of conditions with this new one
     */
    void setConditionList(const QLinkedList<Conditional> & list);

    /**
     * \ingroup NativeFormat
     * Takes a parsed DOM element and recreates the conditions structure out of
     * it
     */
    void loadConditions(const StyleManager* styleManager, const KoXmlElement & element);

    /**
     * \ingroup NativeFormat
     * Saves the conditions to a DOM tree structure.
     * \return the DOM element for the conditions.
     */
    QDomElement saveConditions(QDomDocument & doc) const;

    /**
     * \ingroup OpenDocument
     * Loads the condtional formatting.
     */
    Conditional loadOdfCondition(const StyleManager* styleManager, const QString &conditionValue, const QString &applyStyleName);

    /**
     * \ingroup OpenDocument
     * Loads the condtional formattings.
     */
    void loadOdfConditions(const StyleManager* styleManager, const KoXmlElement & element);

    /**
     * \ingroup OpenDocument
     * Saves the condtional formattings.
     */
    void saveOdfConditions(KoGenStyle &currentCellStyle) const;

    /// \note fake implementation to make QMap happy
    bool operator<(const Conditions&) const {
        return true;
    }
    void operator=(const Conditions& other);
    bool operator==(const Conditions& other) const;
    inline bool operator!=(const Conditions& other) const {
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
    bool currentCondition(const Cell& cell, Conditional & condition) const;

    /**
     * \ingroup OpenDocument
     */
    void loadOdfCondition(QString &valExpression, Conditional &newCondition);

    /**
     * \ingroup OpenDocument
     */
    void loadOdfConditionValue(const QString &styleCondition, Conditional &newCondition);

    /**
     * \ingroup OpenDocument
     */
    void loadOdfValidationValue(const QStringList &listVal, Conditional &newCondition);

    /**
     * \ingroup OpenDocument
     */
    QString saveOdfConditionValue(Conditional &cond) const;

    class Private;
    QSharedDataPointer<Private> d;
};

} // namespace KSpread

Q_DECLARE_METATYPE(KSpread::Conditions)
Q_DECLARE_TYPEINFO(KSpread::Conditions, Q_MOVABLE_TYPE);

#endif // KSPREAD_CONDITION_H
