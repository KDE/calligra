// This file is part of the KDE project
// SPDX-FileCopyrightText: 2003, 2004 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 2005 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_FUNCTION_DESCRIPTION
#define CALLIGRA_SHEETS_FUNCTION_DESCRIPTION

#include <QList>
#include <QStringList>

#include "sheets_engine_export.h"

class QDomElement;

namespace Calligra
{
namespace Sheets
{

enum ParameterType { KSpread_Int, KSpread_Float, KSpread_String, KSpread_Boolean, KSpread_Any, KSpread_Date };

/**
 * \ingroup Value
 * A function parameter.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT FunctionParameter
{
public:
    FunctionParameter();
    FunctionParameter(const FunctionParameter &param);
    explicit FunctionParameter(const QDomElement &element);

    QString helpText() const
    {
        return m_help;
    }
    ParameterType type() const
    {
        return m_type;
    }
    bool hasRange() const
    {
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
class CALLIGRA_SHEETS_ENGINE_EXPORT FunctionDescription
{
public:
    FunctionDescription();
    explicit FunctionDescription(const QDomElement &element);
    FunctionDescription(const FunctionDescription &desc);

    const QStringList &examples()
    {
        return m_examples;
    }
    const QStringList &syntax()
    {
        return m_syntax;
    }
    const QStringList &related()
    {
        return m_related;
    }
    const QStringList &helpText() const
    {
        return m_help;
    }
    QString name() const
    {
        return m_name;
    }
    ParameterType type() const
    {
        return m_type;
    }

    int params() const
    {
        return m_params.count();
    }
    FunctionParameter &param(int i)
    {
        return m_params[i];
    }

    void setGroup(const QString &g)
    {
        m_group = g;
    }
    QString group() const
    {
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

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FUNCTION_DESCRIPTION
