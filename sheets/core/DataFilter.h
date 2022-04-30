/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_FILTER
#define CALLIGRA_SHEETS_FILTER

#include <QMap>
#include <QString>

#include "sheets_core_export.h"

namespace Calligra
{
namespace Sheets
{
class Database;
class Map;
class Region;
class AbstractCondition
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

    virtual ~AbstractCondition() {}
    enum Type { And, Or, Condition };
    virtual Type type() const = 0;
    virtual bool evaluate(const Database& database, int index) const = 0;
    virtual bool isEmpty() const = 0;
    virtual QMap<QString, Comparison> conditions(int fieldNumber) const = 0;
    virtual void removeConditions(int fieldNumber) = 0;
    virtual QString dump() const = 0;
    virtual bool allowsChildren() { return false; }
    virtual QList<AbstractCondition*> children() { return QList<AbstractCondition*>(); }

    static bool listsAreEqual(const QList<AbstractCondition*>& a, const QList<AbstractCondition*>& b);
};


/**
 * OpenDocument, 8.7.1 Table Filter
 */
class CALLIGRA_SHEETS_CORE_EXPORT Filter
{
public:

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

    /** Assignment operator. */
    void operator=(const Filter&);

    void addCondition(Composition composition,
                      int fieldNumber, AbstractCondition::Comparison comparison, const QString& value,
                      Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive, Mode mode = Text);
    void addSubFilter(Composition composition, const Filter& filter);

    QMap<QString, AbstractCondition::Comparison> conditions(int fieldNumber) const;
    void removeConditions(int fieldNumber = -1);

    AbstractCondition *rootCondition() const;
    void setRootCondition(AbstractCondition *cond);

    bool isEmpty() const;

    // These are preserved through loads and saves, but are otherwise not supported at this time.
    Region targetRangeAddress() const;
    void setTargetRangeAddress(const Region &address);
    Region sourceRangeAddress() const;
    void setSourceRangeAddress(const Region &address);
    bool displayDuplicates() const;
    void setDisplayDuplicates(bool val);
    bool conditionSourceIsRange() const;
    void setConditionSourceIsRange(bool val);


    /**
     * \return \c true if the column/row with \p index fulfills all conditions, i.e. it should not
     * be filtered.
     */
    bool evaluate(const Database& database, int index) const;

    bool operator==(const Filter& other) const;
    inline bool operator!=(const Filter& other) const {
        return !operator==(other);
    }

    void dump() const;


    /**
     * OpenDocument, 8.7.2 Filter And
     */
    class And : public AbstractCondition
    {
    public:
        And() {}
        And(const And& other);
        And& operator=(const And& other);
        ~And() override;
        Type type() const override;
        bool evaluate(const Database& database, int index) const override;
        bool isEmpty() const override;
        QMap<QString, AbstractCondition::Comparison> conditions(int fieldNumber) const override;
        void removeConditions(int fieldNumber) override;
        bool operator!=(const And& other) const;
        QString dump() const override;

        bool allowsChildren() override { return true; }
        QList<AbstractCondition*> children() override { return list; }
    public:
        QList<AbstractCondition*> list; // allowed: Or or Condition
    };


    /**
     * OpenDocument, 8.7.3 Filter Or
     */
    class Or : public AbstractCondition
    {
    public:
        Or() {}
        Or(const Or& other);
        Or& operator=(const Or& other);
        ~Or();
        Type type() const override;
        bool evaluate(const Database& database, int index) const override;
        bool isEmpty() const override;
        QMap<QString, AbstractCondition::Comparison> conditions(int fieldNumber) const override;
        void removeConditions(int fieldNumber) override;
        bool operator!=(const Or& other) const;
        QString dump() const override;

        bool allowsChildren() override { return true; }
        QList<AbstractCondition*> children() override { return list; }
    public:
        QList<AbstractCondition*> list; // allowed: And or Condition
    };


    /**
     * OpenDocument, 8.7.4 Filter Condition
     */
    class Condition : public AbstractCondition
    {
    public:
        Condition();
        Condition(int _fieldNumber, Comparison _comparison, const QString& _value,
                  Qt::CaseSensitivity _caseSensitivity, Mode _mode);
        Condition(const Condition& other);
        Condition& operator=(const Condition& other);
        Type type() const override;
        bool evaluate(const Database& database, int index) const override;
        bool isEmpty() const override;
        QMap<QString, Comparison> conditions(int fieldNumber) const override;
        void removeConditions(int fieldNumber) override;
        bool operator==(const Condition& other) const;
        bool operator!=(const Condition& other) const;
        QString dump() const override;

    public:
        int fieldNumber;
        QString value; // Value?
        Comparison operation;
        Qt::CaseSensitivity caseSensitivity;
        Mode dataType;
    };

private:
    friend class AbstractCondition;

    static QList<AbstractCondition*> copyList(const QList<AbstractCondition*>& list);
    static bool conditionsEquals(AbstractCondition* a, AbstractCondition* b);

    void copyFrom (const Filter &other);

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FILTER
