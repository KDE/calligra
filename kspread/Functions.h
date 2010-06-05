/* This file is part of the KDE project
   Copyright (C) 2003,2004 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#ifndef KSPREAD_FUNCTIONS
#define KSPREAD_FUNCTIONS

#include <QList>
#include <QSharedPointer>
#include <QStringList>
#include <QVector>

#include "kspread_export.h"
#include "Region.h"

class QDomElement;

namespace KSpread
{
class Sheet;
class Value;
class ValueCalc;
class Function;

typedef QVector<Value> valVector;

struct rangeInfo {
    int col1, col2, row1, row2;
    int columns() {
        return col2 - col1 + 1;
    }
    int rows() {
        return row2 - row1 + 1;
    }
};
struct FuncExtra {
    // here we'll add all the extras a function may need
    Function* function;
    QVector<rangeInfo> ranges;
    QVector<Region> regions;
    Sheet *sheet;
    int myrow, mycol;
};

typedef Value(*FunctionPtr)(valVector, ValueCalc *, FuncExtra *);

/**
 * \ingroup Value
 * A function pointer and context.
 */
class KSPREAD_EXPORT Function
{
public:
    Function(const QString& name, FunctionPtr ptr);
    virtual ~Function();
    /**
    setParamCount sets allowed parameter count for a function.
    if max=0, it means max=min. If max=-1, there is no upper limit.
    */
    void setParamCount(int min, int max = 0);
    /** is it okay for the function to receive this many parameters ? */
    bool paramCountOkay(int count);
    /** when set to true, the function can receive arrays. When set to
    false, the auto-array mechamism will be used for arrays (so the
    function will receive simple values, not arrays). */
    void setAcceptArray(bool accept = true);
    bool needsExtra();
    void setNeedsExtra(bool extra);
    QString name() const;
    QString localizedName() const;
    QString helpText() const;
    void setHelpText(const QString& text);
    Value exec(valVector args, ValueCalc *calc, FuncExtra *extra = 0);
    
    QString alternateName() const;
    void setAlternateName(const QString &name);

private:
    Q_DISABLE_COPY(Function)

    class Private;
    Private * const d;
};

enum ParameterType { KSpread_Int, KSpread_Float, KSpread_String, KSpread_Boolean, KSpread_Any };

/**
 * \ingroup Value
 * A function parameter.
 */
class KSPREAD_EXPORT FunctionParameter
{
public:
    FunctionParameter();
    FunctionParameter(const FunctionParameter& param);
    FunctionParameter(const QDomElement& element);

    QString helpText() const {
        return m_help;
    }
    ParameterType type() const {
        return m_type;
    }
    bool hasRange() const {
        return m_range;
    }

private:
    QString m_help;
    ParameterType m_type;
    bool m_range;
};

/**
 * \ingroup Value
 * A function description.
 */
class KSPREAD_EXPORT FunctionDescription
{
public:
    FunctionDescription();
    FunctionDescription(const QDomElement& element);
    FunctionDescription(const FunctionDescription& desc);

    const QStringList& examples() {
        return m_examples;
    }
    const QStringList& syntax() {
        return m_syntax;
    }
    const QStringList& related() {
        return m_related;
    }
    const QStringList& helpText() const {
        return m_help;
    }
    QString name() const {
        return m_name;
    }
    ParameterType type() const {
        return m_type;
    }

    int params() const {
        return m_params.count();
    }
    FunctionParameter& param(int i) {
        return m_params[ i ];
    }

    void setGroup(const QString& g) {
        m_group = g;
    }
    QString group() const {
        return m_group;
    }

    QString toQML() const;

private:
    QString m_group;
    QStringList m_examples;
    QStringList m_syntax;
    QStringList m_related;
    QStringList m_help;
    QString m_name;
    ParameterType m_type;
    QList<FunctionParameter> m_params;
};

/**
 * \ingroup Value
 * The function repository.
 */
class KSPREAD_EXPORT FunctionRepository
{
public:
    FunctionRepository();
    ~FunctionRepository();

    static FunctionRepository *self();

    /**
     * Adds \p function to the repository.
     */
    void add(const QSharedPointer<Function>& function);
    void add(FunctionDescription *desc);

    /**
     * Removes \p function from the repository.
     * The Function object and the appropriate description will be destroyed.
     */
    void remove(const QSharedPointer<Function>& function);

    QSharedPointer<Function> function(const QString& name);

    FunctionDescription *functionInfo(const QString& name);

    /** return functions within a group, or all if no group given */
    QStringList functionNames(const QString& group = QString());

    const QStringList &groups() const;
    void addGroup(const QString& groupname);

    /**
     * Loads function descriptions from an XML file.
     */
    void loadFunctionDescriptions(const QString& filename);

private:

    class Private;
    Private * const d;

    // no copy or assign
    FunctionRepository(const FunctionRepository&);
    FunctionRepository& operator=(const FunctionRepository&);
};


} // namespace KSpread


#endif // KSPREAD_FUNCTIONS
