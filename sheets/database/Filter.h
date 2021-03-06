/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_FILTER
#define CALLIGRA_SHEETS_FILTER

#include <QHash>
#include <QString>

#include <KoXmlReader.h>

#include "sheets_odf_export.h"

class KoXmlWriter;

namespace Calligra
{
namespace Sheets
{
class Database;
class Map;
class AbstractCondition;

/**
 * OpenDocument, 8.7.1 Table Filter
 */
class CALLIGRA_SHEETS_ODF_EXPORT Filter
{
public:
    enum Comparison {
        Match,
        NotMatch,
        Equal,
        NotEqual,
        Less,
        Greater,
        LessOrEqual,
        GreaterOrEqual,
        Empty,
        NotEmpty,
        TopValues,
        BottomValues,
        TopPercent,
        BottomPercent
    };

    enum Composition {
        AndComposition,
        OrComposition
    };

    enum Mode {
        Text,
        Number
    };

    /**
     * Constructor.
     */
    Filter();

    /**
     * Constructor.
     */
    Filter(const Filter& other);

    /**
     * Destructor.
     */
    virtual ~Filter();

    void addCondition(Composition composition,
                      int fieldNumber, Comparison comparison, const QString& value,
                      Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive, Mode mode = Text);
    void addSubFilter(Composition composition, const Filter& filter);

    QHash<QString, Comparison> conditions(int fieldNumber) const;
    void removeConditions(int fieldNumber = -1);

    bool isEmpty() const;

    /**
     * \return \c true if the column/row with \p index fulfills all conditions, i.e. it should not
     * be filtered.
     */
    bool evaluate(const Database& database, int index) const;

    bool loadOdf(const KoXmlElement& element, const Map* map);
    void saveOdf(KoXmlWriter& xmlWriter) const;

    bool operator==(const Filter& other) const;
    inline bool operator!=(const Filter& other) const {
        return !operator==(other);
    }

    void dump() const;

private:
    class And;
    class Or;
    class Condition;
    friend class AbstractCondition;

    void operator=(const Filter&);
    static QList<AbstractCondition*> copyList(const QList<AbstractCondition*>& list);
    static bool conditionsEquals(AbstractCondition* a, AbstractCondition* b);

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FILTER
