// This file is part of the KDE project
// SPDX-FileCopyrightText: 2003, 2004 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 2005 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_FUNCTION_REPOSITORY
#define CALLIGRA_SHEETS_FUNCTION_REPOSITORY

#include <QSharedPointer>
#include <QStringList>

#include "sheets_engine_export.h"

namespace Calligra
{
namespace Sheets
{
class Function;
class FunctionDescription;

/**
 * \ingroup Value
 * The function repository.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT FunctionRepository
{
public:
    FunctionRepository();
    ~FunctionRepository();

    static FunctionRepository *self();

    /**
     * Adds \p function to the repository.
     */
    void add(const QSharedPointer<Function> &function);
    void add(FunctionDescription *desc);

    /**
     * Removes \p function from the repository.
     * The Function object and the appropriate description will be destroyed.
     */
    void remove(const QSharedPointer<Function> &function);

    QSharedPointer<Function> function(const QString &name);

    FunctionDescription *functionInfo(const QString &name);

    /** return functions within a group, or all if no group given */
    QStringList functionNames(const QString &group = QString());

    const QStringList &groups() const;
    void addGroup(const QString &groupname);

    /**
     * Loads function descriptions from an XML file.
     */
    void loadFunctionDescriptions(const QString &filename);

private:
    class Private;
    Private *const d;

    // no copy or assign
    FunctionRepository(const FunctionRepository &) = delete;
    FunctionRepository &operator=(const FunctionRepository &) = delete;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FUNCTION_REPOSITORY
